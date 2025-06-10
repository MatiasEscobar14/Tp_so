#include "kernel_IO.h"

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
			if (pcb == NULL)
			{
				log_error(kernel_logger, "No se encontró el PCB para PID %d", pid);
				break;
			}

			cambiar_estado(pcb, READY_PROCCES);
			agregar_pcb_lista(pcb, lista_ready, mutex_lista_ready);

			// Liberar el IO
			t_modulo_io *modulo = obtener_modulo_io_por_socket(socket); // función que busca en la lista por socket_fd
			if (modulo != NULL)
			{
				pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
				modulo->libre = 1;

				// Si hay más procesos esperando por este IO, mandá el siguiente
				if (!list_is_empty(modulo->cola_espera))
				{
					t_io_esperando *espera = list_remove(modulo->cola_espera, 0);
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
					remover_pcb_lista(pcb_ejecutando, lista_blocked, mutex_lista_blocked);
					agregar_pcb_lista(pcb_ejecutando, lista_exit, mutex_lista_exit);
				}
				while (!list_is_empty(modulo_io ->cola_espera))
				{
					t_io_esperando *espera = list_remove(modulo_io ->cola_espera, 0);
					log_info(kernel_logger, "Proceso PID %d pasa a EXIT por desconexión de IO",
							 espera->pcb->pid);
					cambiar_estado(espera->pcb, EXIT_PROCCES);
					remover_pcb_lista(espera->pcb, lista_blocked, mutex_lista_blocked);
					agregar_pcb_lista(espera->pcb, lista_exit, mutex_lista_exit);
					free(espera);
				}

				list_remove_element(lista_modulos_io_conectadas, modulo_io );
				free(modulo_io );

				pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);
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
