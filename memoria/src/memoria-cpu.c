/*#include "/home/utnso/tp-2025-1c-Linux-Learners/memoria/include/memoria-cpu.h"

static void procesar_conexion_memoria(void *void_args)
{
	t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger_memoria = args->log;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	op_code cop;
	while (cliente_socket != -1)
	{
		if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code))
		{
			log_debug(logger_memoria, "Cliente desconectado.\n");
			return;
		}

		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(logger_memoria, cliente_socket);
			break;

        case PEDIDO_INSTRUCCION: 
			printf("entre al case pedido de instruccion");
			uint32_t pid, pc;
			recibir_pedido_instruccion(&pid, &pc, cliente_socket); //(2)
			printf("paso bien el pedido de instruccion");
			// log_debug(logger_memoria, "Se recibio un pedido de instruccion para el PID %d y PC %d", pid, pc);
			proceso_memoria = obtener_proceso_pid(pid); //(3)
			if (proceso_memoria == NULL)
			{
				log_error(logger_memoria, "No se encontro el proceso con PID %d", pid);
				break;
			}
			else
			{
				t_instruccion *instruccion = obtener_instruccion_del_proceso_pc(proceso_memoria, pc); //(3)
				if (instruccion != NULL)
				{
					enviar_instruccion(cliente_socket, instruccion); //(4)
					// log_debug(logger_memoria, "Se envia la instruccion a CPU de PC %d para el PID %d y es: %s - %s - %s", pc, pid, instruccion_to_string(instruccion->nombre), instruccion->parametro1, instruccion->parametro2);
				}
				else
				{
					log_error(logger_memoria, "No se encontro la instruccion con PC %d para el PID %d", pc, pid);
				}
				break;
			}

			case INICIALIZAR_PROCESO: 
			proceso_memoria = recibir_proceso_memoria(cliente_socket); //(5)
			proceso_memoria = iniciar_proceso_path(proceso_memoria); //(6)
			break;
            
            case FINALIZAR_PROCESO:
			uint32_t pid_a_finalizar;
			recibir_finalizar_proceso(&pid_a_finalizar, cliente_socket);
			proceso_memoria = obtener_proceso_pid(pid_a_finalizar);
			if (proceso_memoria == NULL)
			{
				log_error(logger_memoria, "No se encontro el proceso con PID %d", pid_a_finalizar);
				break;
			}
			else
			{
				liberar_estructura_proceso_memoria(proceso_memoria);
				break;
			}
}
}
}


void recibir_pedido_instruccion(uint32_t *pid, uint32_t *pc, int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pedido_instruccion(pid, pc, paquete->buffer);
    eliminar_paquete(paquete);
}

void deserializar_pedido_instruccion(uint32_t *pid, uint32_t *pc, t_buffer *buffer)
{
    int desplazamiento = 0;
    memcpy(pid, buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(pc, buffer->stream + desplazamiento, sizeof(uint32_t));
}

static uint32_t pid_a_buscar;

bool id_process(void *elemento)
{
    return ((t_proceso_memoria *)elemento)->pid == pid_a_buscar;
}

t_proceso_memoria *obtener_proceso_pid(uint32_t pid_pedido)
{
    t_proceso_memoria *proceso;
    pid_a_buscar = pid_pedido;

    pthread_mutex_lock(&mutex_procesos);
    proceso = list_find(lista_procesos, id_process);
    pthread_mutex_unlock(&mutex_procesos);

    return proceso;
}

void enviar_instruccion(int socket, t_instruccion *instruccion)
{
    t_buffer* un_buffer = new_buffer();
    t_paquete *paquete = crear_paquete(INSTRUCCION, un_buffer);
    serializar_instruccion(paquete, instruccion);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_instruccion(t_paquete *paquete, t_instruccion *instruccion)
{
    paquete->buffer->size = sizeof(nombre_instruccion) +
                            sizeof(uint32_t) * 5 +
                            instruccion->longitud_parametro1 +
                            instruccion->longitud_parametro2 +
                            instruccion->longitud_parametro3 +
                            instruccion->longitud_parametro4 +
                            instruccion->longitud_parametro5;

    paquete->buffer->stream = malloc(paquete->buffer->size);

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->nombre), sizeof(nombre_instruccion));
    desplazamiento += sizeof(nombre_instruccion);

    uint32_t longitud_parametro1 = instruccion->longitud_parametro1;                          
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro1, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro2 = instruccion->longitud_parametro2;                         
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro2, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro3 = instruccion->longitud_parametro3;                         
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro3, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro4 = instruccion->longitud_parametro4;                         
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro4, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro5 = instruccion->longitud_parametro5;                          
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro5, sizeof(uint32_t)); 
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro1, instruccion->longitud_parametro1);
    desplazamiento += instruccion->longitud_parametro1;

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro2, instruccion->longitud_parametro2);
    desplazamiento += instruccion->longitud_parametro2;

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro3, instruccion->longitud_parametro3);
    desplazamiento += instruccion->longitud_parametro3;

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro4, instruccion->longitud_parametro4);
    desplazamiento += instruccion->longitud_parametro4;

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro5, instruccion->longitud_parametro5);
}

t_instruccion *obtener_instruccion_del_proceso_pc(t_proceso_memoria *proceso, uint32_t pc)
{
    usleep(RETARDO_MEMORIA * 1000); // 1000 * 1000 = 1 segundo
    return list_get(proceso->instrucciones, pc);
}


//case inicializar proceso

t_proceso_memoria *recibir_proceso_memoria(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_proceso_memoria *proceso = deserializar_proceso(paquete->buffer);
    eliminar_paquete(paquete);
    return proceso;
}

t_proceso_memoria *deserializar_proceso(t_buffer *buffer)
{
    t_proceso_memoria *proceso = malloc(sizeof(t_proceso_memoria));
    if (proceso == NULL)
    {
        return NULL;
    }

    uint32_t long_path;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(proceso->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(long_path), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    proceso->path = malloc(long_path);

    memcpy(proceso->path, stream + desplazamiento, long_path);

    return proceso;
}

t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo)
{
    pthread_mutex_lock(&mutex_procesos);
    proceso_nuevo->instrucciones = parsear_instrucciones(proceso_nuevo->path);
    log_debug(logger_memoria, "Instrucciones bien parseadas para el proceso PID [%d]", proceso_nuevo->pid);
    list_add(lista_procesos, proceso_nuevo);
    pthread_mutex_unlock(&mutex_procesos);
    iniciar_estructura_proceso_memoria(proceso_nuevo);
    return proceso_nuevo;
}

char *leer_archivo(char *path)
{
    char instrucciones[100];
    strcpy(instrucciones, path);
    FILE *archivo = fopen(instrucciones, "r");

    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
    }

    fseek(archivo, 0, SEEK_END);

    int cant_elementos = ftell(archivo);
    rewind(archivo);

    char *cadena = calloc(cant_elementos + 1, sizeof(char));
    if (cadena == NULL)
    {
        perror("Error en la reserva de memoria \n");
        fclose(archivo);
        return NULL;
    }

    int cant_elementos_leidos = fread(cadena, sizeof(char), cant_elementos, archivo);
    if (cant_elementos != cant_elementos_leidos)
    {
        perror("Error leyendo el archivo \n");
        fclose(archivo);
        free(cadena);
        return NULL;
    }
    fclose(archivo);
    return cadena;
}

// LEE EL ARCHIVO E INGRESA CADA INSTRUCCION EN LA LISTA DE INSTRUCCIONES DEL PROCESO
t_list *parsear_instrucciones(char *path)
{
    t_list *instrucciones = list_create();
    char *path_archivo = string_new();
    string_append(&path_archivo, PATH_INSTRUCCIONES);
    string_append(&path_archivo, path);
    char *codigo_leido = leer_archivo(path_archivo);
    char **split_instrucciones = string_split(codigo_leido, "\n");
    int indice_split = 0;
    while (split_instrucciones[indice_split] != NULL)
    {
        char **palabras = string_split(split_instrucciones[indice_split], " ");
        if (string_equals_ignore_case(palabras[0], "NOOP"))
        {
            list_add(instrucciones, armar_estructura_instruccion(NOOP, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "WRITE"))
        {
            list_add(instrucciones, armar_estructura_instruccion(WRITE, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "READ"))
        {
            list_add(instrucciones, armar_estructura_instruccion(READ, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "GOTO"))
        {
            list_add(instrucciones, armar_estructura_instruccion(GOTO, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "IO"))
        {
            list_add(instrucciones, armar_estructura_instruccion(IO, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "INIT_PROC"))
        {
            list_add(instrucciones, armar_estructura_instruccion(INIT_PROC, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "DUMP_MEMORY"))
        {
            list_add(instrucciones, armar_estructura_instruccion(DUMP_MEMORY, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "EXIT"))
        {
            list_add(instrucciones, armar_estructura_instruccion(EXIT, "", "", "", "", ""));
        }

        indice_split++;
        string_iterate_lines(palabras, (void (*)(char *))free);
        free(palabras);
    }
    free(codigo_leido);
    string_iterate_lines(split_instrucciones, (void (*)(char *))free);
    free(split_instrucciones);
    free(path_archivo);
    return instrucciones;
}

void iniciar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria)
{
    proceso_memoria->tabla_paginas = list_create();
    proceso_memoria->tamanio = 0;
    log_info(logger_memoria, "PID: %d - Tamaño: %d", proceso_memoria->pid, proceso_memoria->tamanio);
}

t_instruccion *armar_estructura_instruccion(nombre_instruccion instruccion, char *parametro1, char *parametro2, char *parametro3, char *parametro4, char *parametro5)
{
    t_instruccion *estructura = (t_instruccion *)malloc(sizeof(t_instruccion));

    estructura->nombre = instruccion;

    estructura->parametro1 = (parametro1[0] != '\0') ? strdup(parametro1) : string_new(); //(parametro1 && parametro1[0] != '\0') ? strdup(parametro1) : NULL;
    estructura->longitud_parametro1 = strlen(estructura->parametro1) + 1;

    estructura->parametro2 = (parametro2[0] != '\0') ? strdup(parametro2) : string_new(); //(parametro2 && parametro2[0] != '\0') ? strdup(parametro2) : NULL;
    estructura->longitud_parametro2 = strlen(estructura->parametro2) + 1;

    estructura->parametro3 = (parametro3[0] != '\0') ? strdup(parametro3) : string_new();
    estructura->longitud_parametro3 = strlen(estructura->parametro3) + 1;

    estructura->parametro4 = (parametro4[0] != '\0') ? strdup(parametro4) : string_new();
    estructura->longitud_parametro4 = strlen(estructura->parametro4) + 1;

    estructura->parametro5 = (parametro5[0] != '\0') ? strdup(parametro5) : string_new();
    estructura->longitud_parametro5 = strlen(estructura->parametro5) + 1;

    return estructura;
}

void recibir_finalizar_proceso(uint32_t *pid, int socket)
{
    uint32_t size;
    recv(socket, &size, sizeof(uint32_t), 0);
    recv(socket, pid, size, 0);
    log_debug(logger_memoria, "Finalizar proceso %d", *pid);
}

// tambien deberia eliminarlo de la lista de procesos totales
bool _buscar_proceso(void *element)
{
    return element == proceso_memoria;
}

void liberar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria)
{
    // Liberar elementos de la tabla de páginas
    while (proceso_memoria->tabla_paginas->elements_count > 0)
    {
        uint32_t *nro_marco = (uint32_t *)list_remove(proceso_memoria->tabla_paginas, 0);
        liberar_marco(*nro_marco);
        free(nro_marco);
    }
    list_destroy(proceso_memoria->tabla_paginas);

    // Liberar elementos de la lista de instrucciones
    list_destroy_and_destroy_elements(proceso_memoria->instrucciones, (void *)liberar_instruccion);

    list_remove_by_condition(lista_procesos, _buscar_proceso);

    free(proceso_memoria->path);
    free(proceso_memoria);
}

void liberar_instruccion(t_instruccion *instruccion)
{
    free(instruccion->parametro1);
    free(instruccion->parametro2);
    free(instruccion->parametro3);
    free(instruccion->parametro4);
    free(instruccion->parametro5);
    free(instruccion);
}*/
