#include "syscalls.h"
#include "pcb.h"
#include "planificadorLargoPlazo.h"
#include "kernel_memoria.h"
#include "kernel_cpu.h"
void crear_proceso_sys(char *nombre_archivo, int tam_proceso)
{
	t_pcb *un_pcb = NULL;
	log_info(kernel_logger, "El PATH del proceso es: [%s] y el tamaño del mismo es: [%d] \n", nombre_archivo, tam_proceso);
	un_pcb = crear_pcb(nombre_archivo, tam_proceso);
	agregar_pcb_lista(un_pcb, lista_new, mutex_lista_new);
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

	// PASO 1: Validar que la IO existe (como dice el enunciado)
    log_info(kernel_logger, "DEBUG: Voy a buscar PID %d", param->pid);
    t_modulo_io *modulo_io = buscar_modulo_io_por_nombre(param->nombre_io);
    log_info(kernel_logger, "MODULO OBTENIDO: %s", modulo_io->nombre);
    if (!modulo_io) {
        // Si no existe, enviar proceso a EXIT
        log_error(kernel_logger, "Dispositivo IO '%s' no encontrado.", param->nombre_io);
        
        t_pcb *pcb = buscar_pcb_por_pid(param->pid);


        if (pcb) {
            //falta remover de la lista
            remover_pcb_lista(pcb, lista_execute, &mutex_lista_execute);
            cambiar_estado(pcb, EXIT_PROCCES);
            agregar_pcb_lista(pcb, lista_exit, mutex_lista_exit);
        }
        
        free(param->nombre_io);
        free(param);
        return;
    }
    log_info(kernel_logger, "Antes de buscar_pcb_por_pid");
	// PASO 2: Si existe la IO, buscar el PCB y cambiar a BLOCKED
    t_pcb *pcb = buscar_pcb_por_pid(param->pid);
    log_info(kernel_logger, "Antes de cambiar estado: PCB PID=%d, tiempo_inicio_estado=%ld", pcb->pid, pcb->tiempo_inicio_estado);
    if (!pcb) {
        log_error(kernel_logger, "PCB con PID %d no encontrado", param->pid);
        free(param->nombre_io);
        free(param);
        return;
    }
	// PASO 3: Cambiar proceso a BLOCKED y agregar a cola
    //falta remover de la listta
    remover_pcb_lista(pcb, lista_execute, &mutex_lista_execute);
    cambiar_estado(pcb, BLOCKED_PROCCES);
    agregar_pcb_lista(pcb, lista_blocked, mutex_lista_blocked);

    pthread_mutex_lock(&modulo_io->mutex);

	// PASO 4: encolarlo
    t_io_esperando *espera = malloc(sizeof(t_io_esperando));
    espera->pcb = pcb;
    espera->milisegundos = param->miliseg;
    queue_push(modulo_io->cola_espera, espera);

	// PASO 5: Si hay instancia libre, enviar trabajo inmediatamente
	if (modulo_io->libre && !queue_is_empty(modulo_io->cola_espera)) {
        t_io_esperando *trabajo_a_ejecutar = queue_pop(modulo_io->cola_espera);
        modulo_io->libre = false;
        pthread_mutex_unlock(&modulo_io->mutex);
        enviar_pcb_a_modulo_io(modulo_io, trabajo_a_ejecutar->pcb, trabajo_a_ejecutar->milisegundos);
        free(trabajo_a_ejecutar);
    }else{
        pthread_mutex_unlock(&modulo_io->mutex);
    }
    
    log_info(kernel_logger, "Proceso ## (%d) enviado a dispositivo IO '%s'",
             param->pid, param->nombre_io);
    
    free(param->nombre_io);
    free(param);
}
