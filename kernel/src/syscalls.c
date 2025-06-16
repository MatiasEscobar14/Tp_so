#include "syscalls.h"
#include "pcb.h"
#include "planificadorLargoPlazo.h"
#include "kernel_memoria.h"
#include "kernel_cpu.h"
#include "kernel_IO.h"
void crear_proceso_sys(char *nombre_archivo, int tam_proceso)
{
	t_pcb *un_pcb = NULL;
	log_info(kernel_logger, "El PATH del proceso es: [%s] y el tamaño del mismo es: [%d] \n", nombre_archivo, tam_proceso);
	un_pcb = crear_pcb(nombre_archivo, tam_proceso);
    log_info(kernel_logger, "Se creo la pcb: %d", un_pcb->pid);
	agregar_pcb_lista(un_pcb, lista_new, &mutex_lista_new);
	planificadorLargoPlazo();
}

void dump_memory_sys(int pid)
{
	t_buffer *un_buffer = new_buffer();
	add_int_to_buffer(un_buffer, pid);
	t_paquete *un_paquete = crear_paquete(DUMP_MEMORY_KM, un_buffer);

	// TODO: crear conexion con memoria
	socket_memoria = crear_conexion(kernel_logger, "Memoria Server", IP_MEMORIA, PUERTO_MEMORIA);
	enviar_paquete(un_paquete, socket_memoria);
	atender_kernel_memoria();
	eliminar_paquete(un_paquete);
	liberar_conexion(socket_memoria);

	log_info(kernel_logger, "Se aviso a Memoria para que haga dump del proceso");
}

void syscall_io(t_syscall_io *param){
	log_info(kernel_logger, "Entre a syscal_io");

    // 1. Buscar TODAS las instancias con ese nombre
    t_list* modulos_con_nombre = buscar_modulos_io_por_nombre(param->nombre_io);
    if (list_is_empty(modulos_con_nombre)) {
        log_error(kernel_logger, "Dispositivo IO '%s' no encontrado.", param->nombre_io);

        t_pcb* pcb = buscar_pcb_por_pid(param->pid);
        if (pcb) {
            remover_pcb_lista(pcb, lista_execute, &mutex_lista_execute);
            cambiar_estado(pcb, EXIT_PROCCES);
            agregar_pcb_lista(pcb, lista_exit, &mutex_lista_exit);
        }

        list_destroy(modulos_con_nombre);
        free(param->nombre_io);
        free(param);
        return;
    }

    // 2. Cambiar proceso a BLOCKED
    t_pcb* pcb = buscar_pcb_por_pid(param->pid);
    if (!pcb) {
        log_error(kernel_logger, "PCB con PID %d no encontrado", param->pid);
        list_destroy(modulos_con_nombre);
        free(param->nombre_io);
        free(param);
        return;
    }

    remover_pcb_lista(pcb, lista_execute, &mutex_lista_execute);
    cambiar_estado(pcb, BLOCKED_PROCCES);
    agregar_pcb_lista(pcb, lista_blocked, &mutex_lista_blocked);

    // 3. Obtener la cola de espera por nombre
    t_io_espera_por_nombre* espera_nombre = buscar_o_crear_cola_io(param->nombre_io);

    pthread_mutex_lock(&espera_nombre->mutex);

    // 4. Encolar el proceso en la cola común
    t_io_esperando* espera = malloc(sizeof(t_io_esperando));
    espera->pcb = pcb;
    espera->milisegundos = param->miliseg;
    queue_push(espera_nombre->cola_espera, espera);

    // 5. Ver si hay alguna instancia libre
    
    for (int i = 0; i < list_size(modulos_con_nombre); i++) {
        t_modulo_io* modulo = list_get(modulos_con_nombre, i);

        pthread_mutex_lock(&modulo->mutex);
        if (modulo->libre && !queue_is_empty(espera_nombre->cola_espera)) {
            t_io_esperando* trabajo = queue_pop(espera_nombre->cola_espera);
            modulo->libre = false;
            modulo->pcb_ejecutando = trabajo->pcb;
            enviar_pcb_a_modulo_io(modulo, trabajo->pcb, trabajo->milisegundos);
            free(trabajo);
            pthread_mutex_unlock(&modulo->mutex);
            break;
        }
        pthread_mutex_unlock(&modulo->mutex);
    }

    pthread_mutex_unlock(&espera_nombre->mutex);
    list_destroy(modulos_con_nombre);

    log_info(kernel_logger, "Proceso PID %d enviado a IO: %s por %d milisegundos",  
             param->pid, param->nombre_io, param->miliseg); 

    free(param->nombre_io);
    free(param);
}


