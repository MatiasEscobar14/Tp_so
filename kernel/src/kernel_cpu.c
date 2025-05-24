#include "kernel_cpu.h"
pthread_mutex_t mutex_lista_modulos_cpu = PTHREAD_MUTEX_INITIALIZER;
void atender_kernel_cpu_dispatch(int *socket_cliente) {
    op_code cop;
	int control_key = 1;
	int socket = *socket_cliente;
	int pid;
	char* nombre_archivo;
	int tamanio_proceso;
	//t_pcb* un_pcb = NULL;
	free(socket_cliente); // Liberar el puntero pasado al hilo
	t_modulo_cpu *nuevo_modulo = malloc(sizeof(t_modulo_cpu));
	t_buffer *un_buffer;
    while (control_key) {
		//int cod_op = recibir_operacion(socket_cpu_dispatch);

		if (recv(socket, &cop, sizeof(op_code), 0) != sizeof(op_code))
		{
			log_debug(kernel_logger, "Cliente desconectado.\n");
			break;
		}
		log_info(kernel_logger, "Código de operación recibido: %d", cop);

		switch (cop) {
			case HANDSHAKE:
			un_buffer = recv_buffer(socket);
			if (!un_buffer) {
					log_error(kernel_logger, "Error al recibir buffer del HANDSHAKE.");
					break;
				}
			int identificador = extraer_int_buffer(un_buffer);
			int socket_fd_dispatch = extraer_int_buffer(un_buffer);
			nuevo_modulo->identificador = identificador;
			nuevo_modulo->socket_fd_dispatch = socket;


			log_info(kernel_logger, "HandShake recibido de CPU: %d con socket: %d", nuevo_modulo->identificador, nuevo_modulo->socket_fd_dispatch);
			pthread_mutex_lock(&mutex_lista_modulos_cpu);
			list_add(lista_modulos_cpu, nuevo_modulo);
			pthread_mutex_unlock(&mutex_lista_modulos_cpu);
			log_info(kernel_logger, "CPU %d registrada con socket %d", identificador, socket);
			imprimir_modulos_cpu();
			free(un_buffer);
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
	log_warning(kernel_logger, "El cliente (%d) se desconectó de Kernel Server Cpu dispatch", socket);

	 // Eliminar el módulo de la lista por su socket
    pthread_mutex_lock(&mutex_lista_modulos_cpu);
    for (int i = 0; i < list_size(lista_modulos_cpu); i++) {
        t_modulo_cpu *modulo = list_get(lista_modulos_cpu, i);
        if (modulo->socket_fd == socket) {
            list_remove(lista_modulos_cpu, i);
            free(modulo); // Liberar memoria del módulo
            break;
        }
    }
    pthread_mutex_unlock(&mutex_lista_modulos_cpu);

	imprimir_modulos_cpu();
	close(socket);
	pthread_exit(NULL);
}

void imprimir_modulos_cpu()
{
	 pthread_mutex_lock(&mutex_lista_modulos_cpu);
	 if (list_size(lista_modulos_cpu) == 0) {
        log_info(kernel_logger, "No hay CPUs conectadas.");
    }

    for (int i = 0; i < list_size(lista_modulos_cpu); i++)
    {
        t_modulo_cpu *modulo = list_get(lista_modulos_cpu, i);
        printf("Módulo CPU conectado: ID=%d\n",modulo->identificador);
    }
    pthread_mutex_unlock(&mutex_lista_modulos_cpu);
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

t_modulo_cpu* buscar_modulo_cpu_por_identificador(int identificador) {
	t_modulo_cpu* modulo_buscado;
	bool encontrado = false;

	pthread_mutex_lock(&mutex_lista_modulos_cpu);
	for (int i = 0; i < list_size(lista_modulos_cpu); i++) {
		t_modulo_cpu *modulo = list_get(lista_modulos_cpu, i);
		if (modulo->identificador == identificador) {
			modulo_buscado = *modulo;
			encontrado = true;
			break;
		}
	}
	pthread_mutex_unlock(&mutex_lista_modulos_cpu);

	if (!encontrado) {
		log_error(kernel_logger, "No se encontró el módulo CPU con ID %d", identificador);
	}

	return modulo_buscado;
}

