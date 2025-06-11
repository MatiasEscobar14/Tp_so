#include "kernel_IO.h"
#include "pcb.h"

pthread_mutex_t mutex_lista_modulos_io = PTHREAD_MUTEX_INITIALIZER;

void atender_kernel_io(int *socket_cliente)
{
	op_code cop;
	int socket = *socket_cliente;
	int control_key = 1;
	free(socket_cliente); // Liberar el puntero pasado al hilo
	t_modulo_io *nuevo_modulo = malloc(sizeof(t_modulo_io));
	t_buffer *un_buffer;

	while (control_key)
	{
		if (recv(socket, &cop, sizeof(op_code), 0) != sizeof(op_code))
		{
			log_debug(kernel_logger, "Cliente desconectado.\n");
			break;
		}

		switch (cop)
		{
		case HANDSHAKE:
			un_buffer = recv_buffer(socket);
			char *nombre = extraer_string_buffer(un_buffer);
			int socket_fd = socket;
			log_info(kernel_logger, "socket_fd recibido %d", socket_fd);
			nuevo_modulo->nombre = strdup(nombre);
			nuevo_modulo->socket_fd = socket_fd;
			nuevo_modulo->pcb_ejecutando = NULL;
			nuevo_modulo->cola_espera = queue_create();
			nuevo_modulo->libre = 1;

			if (pthread_mutex_init(&nuevo_modulo->mutex, NULL) != 0) {
				log_error(kernel_logger, "Error inicializando mutex para módulo IO %s", nombre);
				
				free(nuevo_modulo->nombre);
				queue_destroy(nuevo_modulo->cola_espera);
				free(nuevo_modulo);
				free(nombre);
				free(un_buffer);
				break;
			}

			pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
			list_add(lista_modulos_io_conectadas, nuevo_modulo);
			pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);

			imprimir_modulos_io();

			break;
		case FIN_IO:
			t_buffer *buffer = recv_buffer(socket);
			int pid = extraer_int_buffer(buffer);
			log_info(kernel_logger, "Recibido FIN_IO del módulo IO para el PID %d", pid);

			t_pcb *pcb = buscar_pcb_por_pid(pid);

			cambiar_estado(pcb, READY_PROCCES);
			agregar_pcb_lista(pcb, lista_ready, mutex_lista_ready);

			// Liberar el IO
			t_modulo_io *modulo = obtener_modulo_io_por_socket(socket); // función que busca en la lista por socket_fd
			if (modulo != NULL)
			{
				pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
				modulo->libre = 1;

				// Si hay más procesos esperando por este IO, mandá el siguiente
				if (!queue_is_empty(modulo->cola_espera))
				{
					t_io_esperando *espera = queue_pop(modulo->cola_espera);
					enviar_pcb_a_modulo_io(modulo, espera->pcb, espera->milisegundos);
					modulo->libre = 0;
					free(espera); // liberar memoria auxiliar
				}
				pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);
			}
			else
			{
				log_warning(kernel_logger, "No se encontró módulo IO para socket %d", socket);
			}
			// eliminar_buffer(buffer);
			log_info(kernel_logger,"SYSCALL IO FINALIZADAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

			log_info(kernel_logger, "Métricas de Estado: ## (%d) - Métricas de estado: NEW (%d) (%f), READY (%d) (%f),EXEC (%d) (%f), BLOCKED (%d) (%f), SUSP_READY (%d) (%f), SUSP_BLOCKED (%d) (%f),EXIT (%d) (%f),",
				pcb->pid, pcb->metricas_estado[NEW_PROCCES],pcb->metricas_tiempo[NEW_PROCCES],
				pcb->metricas_estado[READY_PROCCES], pcb->metricas_tiempo[READY_PROCCES],
				pcb->metricas_estado[EXEC_PROCCES], pcb->metricas_tiempo[EXEC_PROCCES],
				pcb->metricas_estado[BLOCKED_PROCCES], pcb->metricas_tiempo[BLOCKED_PROCCES],
				pcb->metricas_estado[SUSP_READY_PROCESS], pcb->metricas_tiempo[SUSP_READY_PROCESS],
				pcb->metricas_estado[SUSP_BLOCKED_PROCESS], pcb->metricas_tiempo[SUSP_BLOCKED_PROCESS],
				pcb->metricas_estado[EXIT_PROCCES], pcb->metricas_tiempo[EXIT_PROCCES]);
			break;
		case MENSAJE:
			// manejar mensaje
			break;
		case PAQUETE:
			// manejar paquete
			break;
		case -1: // caso de desconexion IO
			log_warning(kernel_logger, "Módulo IO desconectado en socket %d", socket);

			t_modulo_io *modulo_io = obtener_modulo_io_por_socket(socket);
			if (modulo_io  != NULL)
			{
				pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);

				// Buscar proceso que estaba ejecutando en esta IO
				t_pcb *pcb_ejecutando = modulo_io ->pcb_ejecutando; // Si guardas cuál está ejecutando
				if (pcb_ejecutando != NULL)
				{
					log_info(kernel_logger, "Proceso PID %d pasa a EXIT por desconexión de IO",
							 pcb_ejecutando->pid);
					cambiar_estado(pcb_ejecutando, EXIT_PROCCES);
					remover_pcb_lista(pcb_ejecutando, lista_blocked, &mutex_lista_blocked);
					agregar_pcb_lista(pcb_ejecutando, lista_exit, mutex_lista_exit);
				}
				pthread_mutex_lock(&modulo_io->mutex);
				while (!queue_is_empty(modulo_io ->cola_espera))
				{
					t_io_esperando *espera = queue_pop(modulo_io ->cola_espera);
					log_info(kernel_logger, "Proceso PID %d pasa a EXIT por desconexión de IO",
							 espera->pcb->pid);
					cambiar_estado(espera->pcb, EXIT_PROCCES);
					remover_pcb_lista(espera->pcb, lista_blocked, &mutex_lista_blocked);
					agregar_pcb_lista(espera->pcb, lista_exit, mutex_lista_exit);
					free(espera);
				}
				pthread_mutex_unlock(&modulo_io->mutex);

				list_remove_element(lista_modulos_io_conectadas, modulo_io );
			
				pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);

				free(modulo_io->nombre);
        		queue_destroy(modulo_io->cola_espera);
        		free(modulo_io);
			}
			close(socket);
			break;
		default:
			control_key = 0;
			break;
		}
	}

	log_warning(kernel_logger, "El cliente (%d) se desconectó de Kernel Server IO", socket);
	pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
	for (int i = 0; i < list_size(lista_modulos_io_conectadas); i++)
	{
		t_modulo_io *modulo = list_get(lista_modulos_io_conectadas, i);
		if (modulo->socket_fd == socket)
		{
			list_remove(lista_modulos_io_conectadas, i);
			free(modulo);
			break;
		}
	}
	pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);

	imprimir_modulos_io();

	close(socket);
	pthread_exit(NULL);
}

void server_escuchar_io()
{

	while (1)
	{
		int cliente_socket = esperar_cliente(kernel_logger, "Kernel Server", socket_io);

		if (cliente_socket != -1)
		{
			int *socket_cliente_ptr = malloc(sizeof(int)); // Necesario para pasar puntero único al hilo
			*socket_cliente_ptr = cliente_socket;

			pthread_t hilo;
			if (pthread_create(&hilo, NULL, (void *)atender_kernel_io, socket_cliente_ptr) != 0)
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

void imprimir_modulos_io()
{
	for (int i = 0; i < list_size(lista_modulos_io_conectadas); i++)
	{
		t_modulo_io *modulo = list_get(lista_modulos_io_conectadas, i);
		printf("Módulo IO conectado: %s, estado: %d\n", modulo->nombre, modulo->libre);
	}
}

t_modulo_io *obtener_modulo_io_por_socket(int socket)
{
	t_modulo_io *modulo_encontrado = NULL;

	pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);

	for (int i = 0; i < list_size(lista_modulos_io_conectadas); i++)
	{
		t_modulo_io *modulo = list_get(lista_modulos_io_conectadas, i);
		if (modulo->socket_fd == socket)
		{
			modulo_encontrado = modulo;
			break;
		}
	}

	pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);

	if (!modulo_encontrado)
	{
		log_error(kernel_logger, "No se encontró ningún módulo IO con socket %d", socket);
	}

	return modulo_encontrado;
}
