/*

bool esSyscall = false;
bool envioPcb = false;

bool interrupciones[5] = {0, 0, 0, 0, 0};

static void procesar_conexion_cpu(void *void_args)
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
			log_debug(logger, "Cliente desconectado.\n");
			return;
		}

		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(cliente_socket, logger);
			break;

        case PCB:
			pthread_mutex_lock(&mutex_pcb_actual); // PEPI, NO SE SI ACTUALMENTE YA EMPEZARON CON SEMAFOROS, PREGUNTA
			recibir_pcb(pcb_actual, cliente_socket); //(2)
			pthread_mutex_unlock(&mutex_pcb_actual); // PEPI, NO SE SI ACTUALMENTE YA EMPEZARON CON SEMAFOROS, PREGUNTA

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

			pthread_mutex_lock(&mutex_interrupt); // PEPI, NO SE SI ACTUALMENTE YA EMPEZARON CON SEMAFOROS, PREGUNTA
			limpiar_interrupciones(); //(5)
			pthread_mutex_unlock(&mutex_interrupt); // PEPI, NO SE SI ACTUALMENTE YA EMPEZARON CON SEMAFOROS, PREGUNTA
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
    pedir_instruccion_memoria(pid, pc, cliente_de_memoria); //(12)

    op_code codigo_op = recibir_operacion(cliente_de_memoria); //(13)

    t_instruccion *instruccion;

    if (codigo_op == INSTRUCCION)
    {
        instruccion = deserializar_instruccion(cliente_de_memoria); //(14)
    }
    else
    {
        log_warning(logger, "Operación desconocida. No se pudo recibir la instruccion de memoria.");
        exit(EXIT_FAILURE);
    }

    log_info(logger, "PID: %d - FETCH - Program Counter: %d", pid, pc);

    return instruccion;
}

void execute(t_instruccion *instruccion, int socket)
{
    switch (instruccion->nombre)
    {
    case SET:
        loguear_y_sumar_pc(instruccion);
        _set(instruccion->parametro1, instruccion->parametro2); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case SUM:
        loguear_y_sumar_pc(instruccion);
        _sum(instruccion->parametro1, instruccion->parametro2); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case SUB:
        loguear_y_sumar_pc(instruccion);
        _sub(instruccion->parametro1, instruccion->parametro2); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case JNZ:
        loguear_y_sumar_pc(instruccion);
        _jnz(instruccion->parametro1, instruccion->parametro2); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case WAIT:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL; //FUNCION DISTINTA POR CADA OPERACION
        esSyscall = true;
        envioPcb = true;
        _wait(instruccion->parametro1, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case SIGNAL:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
        esSyscall = true;
        envioPcb = true;
        _signal(instruccion->parametro1, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case RESIZE:
        loguear_y_sumar_pc(instruccion);
        _resize(instruccion->parametro1); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case MOV_IN:
        loguear_y_sumar_pc(instruccion);
        _mov_in(instruccion->parametro1, instruccion->parametro2, cliente_de_memoria); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case MOV_OUT:
        loguear_y_sumar_pc(instruccion);
        _mov_out(instruccion->parametro1, instruccion->parametro2, cliente_de_memoria); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case COPY_STRING:
        loguear_y_sumar_pc(instruccion);
        _copy_string(instruccion->parametro1, cliente_de_memoria); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case IO_GEN_SLEEP:
        loguear_y_sumar_pc(instruccion); 
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        envioPcb = true;
        _io_gen_sleep(instruccion->parametro1, instruccion->parametro2, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case IO_STDIN_READ:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        envioPcb = true;
        _io_stdin_read(instruccion->parametro1, instruccion->parametro2, instruccion->parametro3, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case IO_STDOUT_WRITE:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        envioPcb = true;
        _io_stdout_write(instruccion->parametro1, instruccion->parametro2, instruccion->parametro3, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case IO_FS_CREATE:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        envioPcb = true;
        _io_fs_create(instruccion->parametro1, instruccion->parametro2, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case IO_FS_DELETE:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        envioPcb = true;
        _io_fs_delete(instruccion->parametro1, instruccion->parametro2, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case IO_FS_TRUNCATE:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        envioPcb = true;
        _io_fs_truncate(instruccion->parametro1, instruccion->parametro2, instruccion->parametro3, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case IO_FS_WRITE:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        envioPcb = true;
        _io_fs_write(instruccion->parametro1, instruccion->parametro2, instruccion->parametro3, instruccion->parametro4, instruccion->parametro5, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case IO_FS_READ:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        envioPcb = true;
        _io_fs_read(instruccion->parametro1, instruccion->parametro2, instruccion->parametro3, instruccion->parametro4, instruccion->parametro5, socket); //FUNCION DISTINTA POR CADA OPERACION
        break;
    case EXIT:
        log_info(logger, "PID: %d - Ejecutando: %s", pcb_actual->pid, instruccion_to_string(instruccion->nombre));
        esSyscall = true;
        pcb_actual->contexto_ejecucion->motivo_desalojo = FINALIZACION;
        pcb_actual->contexto_ejecucion->motivo_finalizacion = SUCCESS;
        break;
    default:
        break;
    }
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
*/