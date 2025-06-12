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
			nuevo_modulo->libre = 1;
			pthread_mutex_init(&nuevo_modulo->mutex, NULL);

			pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
			list_add(lista_modulos_io_conectadas, nuevo_modulo);
			pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);

			t_io_espera_por_nombre *espera = buscar_o_crear_cola_io(nombre);

			pthread_mutex_lock(&espera->mutex);
			pthread_mutex_lock(&nuevo_modulo->mutex);

			// Si hay procesos esperando y esta instancia está libre, asignarle uno
			if (nuevo_modulo->libre && !queue_is_empty(espera->cola_espera))
			{
				t_io_esperando *trabajo = queue_pop(espera->cola_espera);
				nuevo_modulo->libre = false;
				nuevo_modulo->pcb_ejecutando = trabajo->pcb;

				enviar_pcb_a_modulo_io(nuevo_modulo, trabajo->pcb, trabajo->milisegundos);
				free(trabajo);
			}
			pthread_mutex_unlock(&nuevo_modulo->mutex);
			pthread_mutex_unlock(&espera->mutex);

			imprimir_modulos_io();

			break;
		case FIN_IO:
			t_buffer *buffer = recv_buffer(socket);
			int pid = extraer_int_buffer(buffer);
			log_info(kernel_logger, "Recibido FIN_IO del módulo IO para el PID %d", pid);

			// 1. Buscar el PCB
			t_pcb *pcb = buscar_pcb_por_pid(pid);
			if (!pcb)
			{
				log_error(kernel_logger, "PCB con PID %d no encontrado en FIN_IO", pid);
				break;
			}

			// 2. Buscar el módulo IO por socket
			t_modulo_io *modulo = obtener_modulo_io_por_socket(socket);
			if (!modulo)
			{
				log_error(kernel_logger, "No se encontró módulo IO para socket %d", socket);
				break;
			}

			// 3. Cambiar estado del proceso: BLOCKED -> READY
			remover_pcb_lista(pcb, lista_blocked, &mutex_lista_blocked);
			cambiar_estado(pcb, READY_PROCCES);
			agregar_pcb_lista(pcb, lista_ready, &mutex_lista_ready);

			// 4. Obtener la cola de espera por nombre del módulo
			log_info(kernel_logger, "ANTES DE BUSCAR LA COLA EN COMUN POR NOMBRE");
			t_io_espera_por_nombre *espera_nombre = buscar_o_crear_cola_io(modulo->nombre);
			log_info(kernel_logger, "DESPUES DE BUSCAR LA COLA EN COMUN POR NOMBRE");
			// 5. Liberar el módulo y asignar siguiente trabajo si existe
			pthread_mutex_lock(&espera_nombre->mutex);
			pthread_mutex_lock(&modulo->mutex);

			log_info(kernel_logger, "DESPUES DE LOS MUTEX");

			// Marcar módulo como libre
			modulo->libre = true;
			modulo->pcb_ejecutando = NULL;

			// Si hay más procesos esperando por este tipo de IO, asignar el siguiente
			if (!queue_is_empty(espera_nombre->cola_espera))
			{
				t_io_esperando *trabajo = queue_pop(espera_nombre->cola_espera);
				modulo->libre = false;
				modulo->pcb_ejecutando = trabajo->pcb;
				enviar_pcb_a_modulo_io(modulo, trabajo->pcb, trabajo->milisegundos);
				free(trabajo); // liberar memoria auxiliar
				log_info(kernel_logger, "Asignado siguiente trabajo PID %d a módulo IO %s",
						 trabajo->pcb->pid, modulo->nombre);
			}

			pthread_mutex_unlock(&modulo->mutex);
			pthread_mutex_unlock(&espera_nombre->mutex);

			log_info(kernel_logger, "SYSCALL IO FINALIZADA - PID %d liberado del IO %s",
					 pid, modulo->nombre);

			// Métricas de estado
			log_info(kernel_logger, "Métricas de Estado PID %d: NEW (%d) (%.2f), READY (%d) (%.2f), "
									"EXEC (%d) (%.2f), BLOCKED (%d) (%.2f), SUSP_READY (%d) (%.2f), "
									"SUSP_BLOCKED (%d) (%.2f), EXIT (%d) (%.2f)",
					 pcb->pid,
					 pcb->metricas_estado[NEW_PROCCES], pcb->metricas_tiempo[NEW_PROCCES],
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
		case -1: // caso de desconexión IO
		{
			log_warning(kernel_logger, "Módulo IO desconectado en socket %d", socket);

			t_modulo_io *modulo_io = obtener_modulo_io_por_socket(socket);
			if (modulo_io != NULL)
			{
				// Obtener la cola de espera compartida por nombre
				t_io_espera_por_nombre *espera_nombre = buscar_o_crear_cola_io(modulo_io->nombre);

				// Proteger acceso a la cola y al módulo
				pthread_mutex_lock(&espera_nombre->mutex);
				pthread_mutex_lock(&modulo_io->mutex);

				// 1. El proceso que estaba ejecutando en esta IO → EXIT (SIEMPRE)
				t_pcb *pcb_ejecutando = modulo_io->pcb_ejecutando;
				if (pcb_ejecutando != NULL)
				{
					cambiar_estado(pcb_ejecutando, EXIT_PROCCES);
					remover_pcb_lista(pcb_ejecutando, lista_blocked, &mutex_lista_blocked);
					agregar_pcb_lista(pcb_ejecutando, lista_exit, &mutex_lista_exit);
					modulo_io->pcb_ejecutando = NULL;
				}

				pthread_mutex_unlock(&modulo_io->mutex);

				// 2. Remover este módulo de la lista ANTES de contar instancias
				pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
				bool removed = false;
				for (int i = 0; i < list_size(lista_modulos_io_conectadas); i++)
				{
					t_modulo_io *modulo = list_get(lista_modulos_io_conectadas, i);
					if (modulo->socket_fd == socket)
					{
						list_remove(lista_modulos_io_conectadas, i);
						removed = true;
						break;
					}
				}
				pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);

				// 3. Verificar si quedan más instancias disponibles de este tipo
				t_list *modulos_restantes = buscar_modulos_io_por_nombre(modulo_io->nombre);
				bool no_quedan_instancias = list_is_empty(modulos_restantes);
				list_destroy(modulos_restantes);

				// 4. Si hay procesos encolados Y no quedan más instancias → EXIT
				if (!queue_is_empty(espera_nombre->cola_espera) && no_quedan_instancias)
				{
					log_warning(kernel_logger, "No quedan instancias de IO %s - enviando procesos encolados a EXIT",
								modulo_io->nombre);

					while (!queue_is_empty(espera_nombre->cola_espera))
					{
						t_io_esperando *trabajo = queue_pop(espera_nombre->cola_espera);
						cambiar_estado(trabajo->pcb, EXIT_PROCCES);
						remover_pcb_lista(trabajo->pcb, lista_blocked, &mutex_lista_blocked);
						agregar_pcb_lista(trabajo->pcb, lista_exit, &mutex_lista_exit);
						free(trabajo);
					}
				}
				else if (!no_quedan_instancias)
				{
					log_info(kernel_logger, "Quedan %d instancias de IO %s - procesos encolados siguen esperando",
							 list_size(modulos_restantes), modulo_io->nombre);
				}

				pthread_mutex_unlock(&espera_nombre->mutex);

				// 5. Limpiar recursos del módulo desconectado
				if (removed)
				{
					pthread_mutex_destroy(&modulo_io->mutex);
					free(modulo_io->nombre);
					free(modulo_io);
					log_info(kernel_logger, "Módulo IO removido correctamente");
				}

				// 6. Si no quedan instancias y no hay cola, limpiar estructura de espera
				if (no_quedan_instancias && queue_is_empty(espera_nombre->cola_espera))
				{
					pthread_mutex_lock(&mutex_lista_io_esperas);
					for (int i = 0; i < list_size(lista_io_esperas); i++)
					{
						t_io_espera_por_nombre *espera = list_get(lista_io_esperas, i);
						if (strcmp(espera->nombre, modulo_io->nombre) == 0)
						{
							list_remove(lista_io_esperas, i);

							pthread_mutex_destroy(&espera->mutex);
							queue_destroy(espera->cola_espera);
							free(espera->nombre);
							free(espera);
							log_info(kernel_logger, "Estructura de espera para IO %s limpiada", modulo_io->nombre);
							break;
						}
					}
					pthread_mutex_unlock(&mutex_lista_io_esperas);
				}
			}
			else
			{
				log_warning(kernel_logger, "No se encontró módulo IO para socket %d", socket);
			}

			close(socket);
			break;
		}
		default:
			control_key = 0;
			break;
		}
	}

	log_warning(kernel_logger, "El cliente (%d) se desconectó de Kernel Server IO", socket);

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

char *nombre_io_global = NULL;

bool misma_io(void *elemento)
{
	t_io_espera_por_nombre *io = (t_io_espera_por_nombre *)elemento;
	return strcmp(io->nombre, nombre_io_global) == 0;
}

t_io_espera_por_nombre *buscar_o_crear_cola_io(char *nombre_io)
{
	t_io_espera_por_nombre *espera = NULL;

	pthread_mutex_lock(&mutex_lista_io_esperas);

	nombre_io_global = nombre_io;
	espera = list_find(lista_io_esperas, misma_io);
	nombre_io_global = NULL;

	if (espera == NULL)
	{
		espera = malloc(sizeof(t_io_espera_por_nombre));
		espera->nombre = strdup(nombre_io);
		espera->cola_espera = queue_create();
		pthread_mutex_init(&espera->mutex, NULL);
		list_add(lista_io_esperas, espera);

		log_info(kernel_logger, "Creada nueva cola de espera para IO '%s'", nombre_io);
	}

	pthread_mutex_unlock(&mutex_lista_io_esperas);

	return espera;
}


t_list* buscar_modulos_io_por_nombre(char* nombre) {
    t_list* modulos_con_nombre = list_create();
    
    pthread_mutex_lock(&mutex_lista_modulos_io_conectadas);
    for (int i = 0; i < list_size(lista_modulos_io_conectadas); i++) {
        t_modulo_io* modulo = list_get(lista_modulos_io_conectadas, i);
        if (strcmp(modulo->nombre, nombre) == 0) {
            list_add(modulos_con_nombre, modulo);
        }
    }
    pthread_mutex_unlock(&mutex_lista_modulos_io_conectadas);
    
    return modulos_con_nombre;
}
