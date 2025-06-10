/*#include "utils_cpu.h"

bool esSyscall = false;
bool envioPcb = false;

bool interrupciones[5] = {0, 0, 0, 0, 0};

void procesar_conexion_cpu(void *void_args)
{
	t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger = args->log;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	op_code cop;
	while (cliente_socket != -1)
	{
		if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code))
		{
			log_debug(cpu_logger, "Cliente desconectado.\n");
			return;
		}

		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(logger, cliente_socket);
			break;

        case PCB:
			pthread_mutex_lock(&mutex_pcb_actual); 
			recibir_pcb(pcb_actual, cliente_socket); //(2)
			pthread_mutex_unlock(&mutex_pcb_actual); 

			while (!hayInterrupciones() && pcb_actual != NULL && !esSyscall)
			{
				ejecutar_ciclo_instruccion(cliente_socket); //(3)
			}

			log_debug(logger, "PID: %d, Contexto: %s\n", pcb_actual->pid, motivo_desalojo_to_string(pcb_actual->contexto_ejecucion->motivo_desalojo));

			// Envia el PCB actualizado si no ejecuto una syscall de pedido de recurso o IO
			if (!envioPcb)
			{
				enviar_pcb(pcb_actual, cliente_socket); //(4)
			}

			envioPcb = false;
			esSyscall = false;

			pthread_mutex_lock(&mutex_interrupt); 
			limpiar_interrupciones(); //(5)
			pthread_mutex_unlock(&mutex_interrupt); 
			break;

    }}}


void ejecutar_ciclo_instruccion(int socket)
{
    t_instruccion *instruccion = fetch(pcb_actual->pid, pcb_actual->contexto_ejecucion->registros->program_counter); 
    // TODO decode: manejo de TLB y MMU
    execute(instruccion, socket); 
    liberar_instruccion(instruccion); //(11)
}

t_instruccion *fetch(uint32_t pid, uint32_t pc)
{
    pedir_instruccion_memoria(pid, pc, socket_memoria); //(12)

    op_code codigo_op = recibir_operacion(socket_memoria); //(13)

    t_instruccion *instruccion;

    if (codigo_op == INSTRUCCION)
    {
        instruccion = deserializar_instruccion(socket_memoria); //(14)
    }
    else
    {
        log_warning(cpu_logger, "Operación desconocida. No se pudo recibir la instruccion de memoria.");
        exit(EXIT_FAILURE);
    }

    log_info(cpu_logger, "PID: %d - FETCH - Program Counter: %d", pid, pc);

    return instruccion;
}

void execute(t_instruccion *instruccion, int socket)
{
    int tamanio;
    int valor;
    int tiempo;

    switch (instruccion->nombre)
    {
    case NOOP:
        loguear_y_sumar_pc(instruccion);
        _noop(); 
        break;
    case WRITE:
        loguear_y_sumar_pc(instruccion);
        _write(instruccion->parametro1, instruccion->parametro2, socket_memoria); 
        break;
    case READ:
        loguear_y_sumar_pc(instruccion);
        int tamanio = atoi(instruccion->parametro2);
        _read(instruccion->parametro1, tamanio, socket_memoria); 
        break;
    case GOTO:
        loguear_y_sumar_pc(instruccion);
        int valor = atoi(instruccion->parametro1);
        _goto(valor); 
        break;
    case IO:
        loguear_y_sumar_pc(instruccion);
        int tiempo = atoi(instruccion->parametro2);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL; 
        esSyscall = true;
        envioPcb = true;
        _IO(instruccion->parametro1, tiempo); 
        break;
    case INIT_PROC:
        loguear_y_sumar_pc(instruccion);
        int tamanio_param = atoi(instruccion->parametro2);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
        esSyscall = true;
        envioPcb = true;
        _init_proc(instruccion->parametro1, tamanio_param);
        break;
    case DUMP_MEMORY:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
        esSyscall = true;
        envioPcb = true;
        _dump_memory();
        break;
    case EXIT:
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb_actual->pid, instruccion_to_string(instruccion->nombre));
        esSyscall = true;
        pcb_actual->contexto_ejecucion->motivo_desalojo = FINALIZACION;
        pcb_actual->contexto_ejecucion->motivo_finalizacion = SUCCESS;
        break;
    default:
        break;
    }
}

void log_instruccion_ejecutada(nombre_instruccion nombre, char *param1, char *param2, char *param3, char *param4, char *param5)
{
    char *nombre_instruccion = instruccion_to_string(nombre);
    log_info(cpu_logger, "PID: %d - Ejecutando: %s - Parametros: %s %s %s %s %s", pcb_actual->pid, nombre_instruccion, param1, param2, param3, param4, param5);
}

void loguear_y_sumar_pc(t_instruccion *instruccion)
{
    log_instruccion_ejecutada(instruccion->nombre, instruccion->parametro1, instruccion->parametro2, instruccion->parametro3, instruccion->parametro4, instruccion->parametro5);
    
    if((instruccion->nombre) =! GOTO) {
    pcb_actual->contexto_ejecucion->registros->program_counter++;
    } else {}
}

void liberar_instruccion(t_instruccion *instruccion)
{
    free(instruccion->parametro1);
    free(instruccion->parametro2);
    free(instruccion->parametro3);
    free(instruccion->parametro4);
    free(instruccion->parametro5);
    free(instruccion);
}

void crear_buffer(t_paquete *paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}


t_paquete *crear_paquete_con_codigo_de_operacion(op_code codigo)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	crear_buffer(paquete);
	return paquete;
}

void pedir_instruccion_memoria(uint32_t pid, uint32_t pc, int socket)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_INSTRUCCION);
    paquete->buffer->size += sizeof(uint32_t) * 2;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, &(pid), sizeof(uint32_t));
    memcpy(paquete->buffer->stream + sizeof(uint32_t), &(pc), sizeof(int));
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}



t_instruccion *deserializar_instruccion(int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));

    void *stream = paquete->buffer->stream;
    int desplazamiento = 0;

    memcpy(&(instruccion->nombre), stream + desplazamiento, sizeof(nombre_instruccion));
    desplazamiento += sizeof(nombre_instruccion);

    uint32_t tamanio_parametro1;
    memcpy(&(tamanio_parametro1), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanio_parametro2;
    memcpy(&(tamanio_parametro2), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanio_parametro3;
    memcpy(&(tamanio_parametro3), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanio_parametro4;
    memcpy(&(tamanio_parametro4), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanio_parametro5;
    memcpy(&(tamanio_parametro5), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    instruccion->parametro1 = malloc(tamanio_parametro1);
    memcpy(instruccion->parametro1, stream + desplazamiento, tamanio_parametro1);
    desplazamiento += tamanio_parametro1;

    instruccion->parametro2 = malloc(tamanio_parametro2);
    memcpy(instruccion->parametro2, stream + desplazamiento, tamanio_parametro2);
    desplazamiento += tamanio_parametro2;

    instruccion->parametro3 = malloc(tamanio_parametro3);
    memcpy(instruccion->parametro3, stream + desplazamiento, tamanio_parametro3);
    desplazamiento += tamanio_parametro3;

    instruccion->parametro4 = malloc(tamanio_parametro4);
    memcpy(instruccion->parametro4, stream + desplazamiento, tamanio_parametro4);
    desplazamiento += tamanio_parametro4;

    instruccion->parametro5 = malloc(tamanio_parametro5);
    memcpy(instruccion->parametro5, stream + desplazamiento, tamanio_parametro5);

    eliminar_paquete(paquete);

    return instruccion;
}

void iniciar_semaforos_etc()
{
    pthread_mutex_init(&mutex_interrupt, NULL);
    pthread_mutex_init(&mutex_pcb_actual, NULL);
}

bool hayInterrupciones(void)
{
	for (int i = 0; i < 5; i++)
	{
		if (interrupciones[i])
		{
			return true;
		}
	}
	return false;
}

void limpiar_interrupciones(void)
{
	for (int i = 0; i < 5; i++)
	{
		interrupciones[i] = false;
	}
}

char *motivo_desalojo_to_string(t_motivo_desalojo motivo)
{
	switch (motivo)
	{
	case SIN_MOTIVO:
		return "SIN MOTIVO";
	case INTERRUPCION_FIN_QUANTUM:
		return "INTERRUPCION FIN QUANTUM";
	case INTERRUPCION_BLOQUEO:
		return "INTERRUPCION BLOQUEO";
	case INTERRUPCION_FINALIZACION:
		return "INTERRUPCION FINALIZACION";
	case FINALIZACION:
		return "FINALIZACION";
	case INTERRUPCION_ERROR:
		return "INTERRUPCION ERROR";
	case INTERRUPCION_SYSCALL:
		return "INTERRUPCION SYSCALL";
    case INTERRUPCION_OUT_OF_MEMORY:
		return "INTERRUPCION_OUT_OF_MEMORY";
	default:
		return "ERROR";
	}
}

char *instruccion_to_string(nombre_instruccion nombre)
{
    switch (nombre)
    {
    case NOOP:
        return "NOOP";
    case WRITE:
        return "WRITE";
    case READ:
        return "READ";
    case GOTO:
        return "GOTO";
    case IO:
        return "IO";
    case INIT_PROC:
        return "INIT_PROC";
    case DUMP_MEMORY:
        return "DUMP_MEMORY";
    case EXIT:
        return "EXIT";
    default:
        return "DESCONOCIDA";
    }
}*/