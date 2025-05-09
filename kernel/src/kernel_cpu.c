#include "kernel_cpu.h"

void attend_kernel_cpu_dispatch(){

    bool control_key = 1;
	t_buffer* un_buffer;
	int pid;
	char* nombre_archivo;
	int tamanio_proceso;
	//t_pcb* un_pcb = NULL;
	
    while (control_key) {
		int cod_op = recibir_operacion(socket_cpu_dispatch);
		switch (cod_op) {
		    case MENSAJE:
			    //
			    break;
		    case PAQUETE:
			    //
			    break;
			case INIT_PROC:
				un_buffer = recv_buffer(socket_cpu_dispatch);
				pid = extraer_int_buffer(un_buffer);
				nombre_archivo = extraer_string_buffer(un_buffer);
				tamanio_proceso = extraer_int_buffer(un_buffer);

				log_info(kernel_logger, "Syscall recibida: ## (%d) - Solicitó syscall: INIT_PROC", pid);
			    crear_proceso_sys(nombre_archivo, tamanio_proceso);
				//aviso_finalizacion_syscall?
				//free(nombre_archivo);
				free(un_buffer);
			
			    break;
			case EXIT:
				un_buffer = recv_buffer(socket_cpu_dispatch);
				pid = extraer_int_buffer(un_buffer);
				log_info(kernel_logger, "Syscall recibida: ## (%d) - Solicitó syscall: EXIT", pid);
				finalizar_proceso(pid);
				break;
			case DUMP_MEMORY:
				un_buffer = recv_buffer(socket_cpu_dispatch);
				pid = extraer_int_buffer(un_buffer);
				log_info(kernel_logger, "Syscall recibida: ## (%d) - Solicitó syscall: DUMP_MEMORY", pid);
				bloquear_proceso_syscall(pid);
				dump_memory_sys(pid);
				sem_wait(&sem_rpta_dump_memory);

				break;
		    default:
			    log_warning(kernel_logger,"OPERACION DESCONOCIDA - KERNEL - CPU DISPATCH");
				control_key = 0;
			    break;
		}

	}
	//list_iterator_destroy(iterator);
}

