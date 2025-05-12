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
			int socket_fd = extraer_int_buffer(un_buffer);

			nuevo_modulo->nombre = strdup(nombre);
			nuevo_modulo->socket_fd = socket_fd;

			pthread_mutex_lock(&mutex_lista_modulos_io);
			list_add(lista_modulos_io, nuevo_modulo);
			pthread_mutex_unlock(&mutex_lista_modulos_io);

			imprimir_modulos_io();

			break;
		case MENSAJE:
			// manejar mensaje
			break;
		case PAQUETE:
			// manejar paquete
			break;
		default:
			control_key = 0;
			break;
		}
	}

	log_warning(kernel_logger, "El cliente (%d) se desconectó de Kernel Server IO", socket);

	
	pthread_mutex_lock(&mutex_lista_modulos_io);
	list_remove(lista_modulos_io, nuevo_modulo);
	pthread_mutex_unlock(&mutex_lista_modulos_io);

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
	for (int i = 0; i < list_size(lista_modulos_io); i++)
	{
		t_modulo_io *modulo = list_get(lista_modulos_io, i);
		printf("Módulo IO conectado: %s\n", modulo->nombre);
	}
}
