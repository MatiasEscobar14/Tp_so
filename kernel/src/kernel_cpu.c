#include "kernel_cpu.h"
pthread_mutex_t mutex_lista_modulos_cpu = PTHREAD_MUTEX_INITIALIZER;
void atender_kernel_cpu_dispatch(int *socket_cliente) {
    bool control_key = 1;
	int socket = *socket_cliente;
	int pid;
	char* nombre_archivo;
	int tamanio_proceso;
	//t_pcb* un_pcb = NULL;
	free(socket_cliente); // Liberar el puntero pasado al hilo
	t_modulo_cpu *nuevo_modulo = malloc(sizeof(t_modulo_cpu));
	t_buffer *un_buffer;
    while (control_key) {
		int cod_op = recibir_operacion(socket_cpu_dispatch);
		switch (cod_op) {
			case HANDSHAKE:
			un_buffer = recv_buffer(socket);
			char *nombre = extraer_string_buffer(un_buffer);
			int socket_fd = extraer_int_buffer(un_buffer);

			nuevo_modulo->nombre = strdup(nombre);
			nuevo_modulo->socket_fd = socket_fd;

			pthread_mutex_lock(&mutex_lista_modulos_cpu);
			list_add(lista_cpu_conectadas, nuevo_modulo);
			pthread_mutex_unlock(&mutex_lista_modulos_cpu);

			//imprimir_modulos_io();
				break;
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
	log_warning(kernel_logger, "El cliente (%d) se desconectó de Kernel Server IO", socket);

	pthread_mutex_lock(&mutex_lista_modulos_cpu);
	list_remove(lista_modulos_cpu, nuevo_modulo);
	pthread_mutex_unlock(&mutex_lista_modulos_cpu);

	imprimir_modulos_cpu();
	close(socket);
	pthread_exit(NULL);
}
void imprimir_modulos_cpu()
{
	for (int i = 0; i < list_size(lista_modulos_cpu); i++)
	{
		t_modulo_cpu *modulo = list_get(lista_modulos_cpu, i);
		printf("Módulo CPU conectado: %s\n", modulo->nombre);
	}
}


void server_escuchar_cpu_dispatch(){
while (1)
	{
		int cliente_socket = esperar_cliente(kernel_logger, "Kernel Server", socket_cpu_dispatch);

		if (cliente_socket != -1)
		{
			int *socket_cliente_ptr = malloc(sizeof(int)); // Necesario para pasar puntero único al hilo
			*socket_cliente_ptr = cliente_socket;

			pthread_t hilo;
			if (pthread_create(&hilo, NULL, (void *)atender_kernel_cpu_dispatch, socket_cliente_ptr) != 0)
			{
				log_error(kernel_logger, "Error al crear hilo");
				close(cliente_socket);
				free(socket_cliente_ptr);
				continue;
			}

			pthread_detach(hilo);
		}
	}	
}

