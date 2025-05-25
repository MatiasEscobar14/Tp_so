#include "syscalls.h"
#include "pcb.h"
#include "planificadorLargoPlazo.h"
#include "kernel_memoria.h"
#include "kernel_cpu.h"
void crear_proceso_sys(char* nombre_archivo, int tam_proceso){
	t_pcb* un_pcb = NULL;
	log_info(kernel_logger, "El PATH del proceso es: [%s] y el tamaño del mismo es: [%d] \n", nombre_archivo, tam_proceso);
	un_pcb = crear_pcb(nombre_archivo, tam_proceso);
	agregar_pcb_lista(un_pcb, lista_new, mutex_lista_new);
	planificadorLargoPlazo();
}

void dump_memory_sys(int pid){
	t_buffer* un_buffer = new_buffer();
	add_int_to_buffer(un_buffer, pid);
    t_paquete* un_paquete = crear_paquete(DUMP_MEMORY_KM, un_buffer);

	//TODO: crear conexion con memoria
    socket_memoria = crear_conexion(kernel_logger, "Memoria Server", IP_MEMORIA, PUERTO_MEMORIA);
	enviar_paquete(un_paquete, socket_memoria);
    atender_kernel_memoria();
    eliminar_paquete(un_paquete);
    liberar_conexion(socket_memoria);

	log_info(kernel_logger, "Se aviso a Memoria para que haga dump del proceso");
}


void syscall_io(t_syscall_io* param){
	t_pcb* pcb = buscar_pcb_por_pid(param->pid); 

	log_info(kernel_logger, "Syscall recibida: ## (%d) - Solicitó syscall: IO '%s' por %d ms", param->pid, param->nombre_io, param->miliseg);
	
	
	pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
	
	
	t_modulo_io* modulo_io = buscar_modulo_io_por_nombre(param->nombre_io);

	if(!modulo_io){
		log_error(kernel_logger, "Dispositivo OP '%s' no encontrado.", param->nombre_io);
		cambiar_estado(pcb, EXIT_PROCCES);
    	agregar_pcb_lista(pcb, lista_exit,   mutex_lista_exit);
		pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);
		free(param->nombre_io);
		free(param);
		pthread_exit(NULL);
		//TODO deberiamos finalizar proceso o simplemente cambiar el estado a EXIT?
			return;
		} 
	
		cambiar_estado(pcb, BLOCKED_PROCCES);
    	agregar_pcb_lista(pcb, lista_blocked, mutex_lista_blocked);

		pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
		queue_push(modulo_io->cola_espera, pcb);
		pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);

		if (modulo_io->libre && !queue_is_empty(modulo_io->cola_espera)) {
    		t_pcb* pcb_a_ejecutar = queue_pop(modulo_io->cola_espera);
    		modulo_io->libre = false;
			enviar_pcb_a_modulo_io(modulo_io, pcb_a_ejecutar, param->miliseg);
		}

		log_info(kernel_logger, "Fin de IO: ## (<%d>) finalizó IO y pasa a READY", param->pid);

}
