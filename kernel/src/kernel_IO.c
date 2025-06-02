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
			nuevo_modulo->cola_espera = queue_create();
			nuevo_modulo->libre = 1;
			nuevo_modulo->en_ejecucion = NULL;

			pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
			list_add(lista_modulos_io_conectadas, nuevo_modulo);
			pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);

			log_info(kernel_logger, "Módulo IO conectado: %s, estado: %d", nuevo_modulo->nombre, nuevo_modulo->libre);
			imprimir_modulos_io();
			free(nombre);
			
			break;
		case FIN_IO:

    		log_info(kernel_logger, "Esperando buffer FIN_IO del módulo IO (socket %d)", socket);
			t_buffer *buffer = recv_buffer(socket);
			int pid = extraer_int_buffer(buffer);
			log_info(kernel_logger, "Recibido FIN_IO del módulo IO para el PID %d", pid);

			t_modulo_io *io = obtener_modulo_io_por_socket(int socket);
			if (!io)
			{
				log_error(kernel_logger, "No se encontró el io para socket %d", socket);
				break;
			}

			t_pcb* pcb = buscar_pcb_por_pid(pid);
			
			cambiar_estado(pcb,EXIT_PROCCES);
			/* TODO Al momento de recibir un mensaje de una IO se deberá verificar que el mismo sea una confirmación
			de fin de IO, en caso afirmativo, se deberá validar si hay más procesos esperando realizar dicha IO.
			En caso de que el mensaje corresponda a una desconexión de la IO, el proceso que estaba ejecutando en
			dicha IO, se deberá pasar al estado EXIT.*/
			if (!queue_is_empty(io->cola_espera)) {
    		t_pcb* siguiente = queue_pop(io->cola_espera);
    		io->en_ejecucion = siguiente;

		    // Enviás al módulo IO: PID + tiempo que tiene que dormir
    		t_buffer* buffer = new_buffer();
    		add_int_to_buffer(buffer, siguiente->pid);
    		add_int_to_buffer(buffer, siguiente->tiempo_io);

    		t_paquete* paquete = crear_paquete(REALIZAR_IO, buffer);
    		enviar_paquete(paquete, io->socket);
    		eliminar_paquete(paquete);
			} else {
 	   			io->en_ejecucion = NULL; // no hay nadie más
			}

	
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
