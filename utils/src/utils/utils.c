#include <utils/utils.h>

extern t_log* logger;

void iterator(char* value) {
	t_log* logger = iniciar_logger("loggerMemoria.log", "Memoria");
    log_info(logger, "%s", value);  // Usamos el logger que se pasa como argumento
}

t_log *logger_recibido;

int iniciar_servidor(t_log *logger, const char *name, char *puerto)
{
	logger_recibido = logger;
	int socket_servidor;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	bool conecto = false;

	for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
	{
		socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (socket_servidor == -1)
			continue;

		int enable = 1;
		if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		{
			close(socket_servidor);
			log_error(logger, "setsockopt(SO_REUSEADDR) failed");
			continue;
		}

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(socket_servidor);
			continue;
		}
		conecto = true;
		break;
	}
	if (!conecto)
	{
		free(servinfo);
		return 0;
	}
	listen(socket_servidor, SOMAXCONN);
	log_info(logger, "Servidor escuchando en %s:%s", name, puerto);

	freeaddrinfo(servinfo);
	return socket_servidor;
}

// ESPERAR CONEXION DE CLIENTE EN UN SERVER ABIERTO
int esperar_cliente(t_log* logger, const char* name, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

    log_info(logger, "Cliente conectado (a %s)\n", name);

    return socket_cliente;
}

// CLIENTE SE INTENTA CONECTAR A SERVER ESCUCHANDO EN IP:PUERTO
int crear_conexion(t_log* logger, const char* server_name, char* ip, char* puerto) {
    struct addrinfo hints, *servinfo;

    // Init de hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    // Crea un socket con la informacion recibida (del primero, suficiente)
    int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    // Fallo en crear el socket
    if(socket_cliente == -1) {
        log_error(logger, "Error creando el socket para %s:%s", ip, puerto);
        return 0;
    }

    // Error conectando
    if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        log_error(logger, "Error al conectar (a %s)\n", server_name);
        freeaddrinfo(servinfo);
        return 0;
    } else
        log_info(logger, "Cliente conectado en %s:%s (a %s)\n", ip, puerto, server_name);

    freeaddrinfo(servinfo);

    return socket_cliente;
}

// CERRAR CONEXION
void liberar_conexion(int* socket_cliente) {
    close(*socket_cliente);
    *socket_cliente = -1;
}

t_log* iniciar_logger(char* nombreArchivo, char* modulo){
	t_log* nuevo_logger = log_create(nombreArchivo, modulo, 1, LOG_LEVEL_INFO);


	return nuevo_logger;
}

t_config* iniciar_config(char* nombreArchivo){
	t_config* nuevo_config = config_create(nombreArchivo);

	if(nuevo_config==NULL)
      {
    
        exit(EXIT_FAILURE);
      }

	return nuevo_config;
}


void terminar_programa(int conexion, t_log* logger, t_config* config){
	 // liberar_conexion(conexion);
	  log_destroy(logger);
	  config_destroy(config);

}


int server_escuchar(t_log* logger, char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->log = logger;
        args->fd = cliente_socket;
        args->server_name = server_name;

       pthread_create(&hilo, NULL, (void*)procesar_conexion, (void*) args);
		

        pthread_detach(hilo);
		log_info(logger, "CHECKPOINT 1"); 
	
        return 1;
    }
    return 0;
}

void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* logger = args->log;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    free(args);
	log_info(logger, "CHECKPOINT 2"); 
	op_code cop;

    while (cliente_socket != -1) {
  
		if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code))
		{
			log_debug(logger, "Cliente desconectado.\n");
			break;
		}

        switch (cop) {
            case HANDSHAKE:
                uint32_t tamanio;
                recv(cliente_socket, &tamanio, sizeof(uint32_t), MSG_WAITALL);

                char* nombre_cliente = malloc(tamanio);
                recv(cliente_socket, nombre_cliente, tamanio, MSG_WAITALL);

                log_info(logger, "Handshake recibido de cliente: %s", nombre_cliente);
                free(nombre_cliente);

                //enviar_mensaje("Hola, handshake recibido",cliente_socket, logger);
        
                break;
            case MENSAJE:
                recibir_mensaje(logger, cliente_socket);
                break;

            case PAQUETE:
            
                t_list* lista = recibir_paquete(cliente_socket);
				log_info(logger, "Me llegaron los siguientes valores:\n");
				list_iterate(lista, (void*) iterator);
	
                break;
            default:
                log_error(logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(logger, "Cop: %d", cop);
                break;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}






