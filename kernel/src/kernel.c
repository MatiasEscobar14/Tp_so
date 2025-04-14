#include <utils/utils.h>
#include "kernel.h"


int main(int argc, char* argv[]) {

	//===========================================CONEXION KERNEL (CLIENTE) CON MEMORIA (SERVER)========================================================//

    char* ip_memoria;
	char* puerto_memoria;

    t_log* logger = iniciar_logger("kernel.log", "Kernel");
   

    t_config* config = iniciar_config("kernel.config"); 

    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value (config,"PUERTO_MEMORIA");
    
	int conexionMemoria = crear_conexion(logger, "memoriaSV", ip_memoria, puerto_memoria);
	if (conexionMemoria == -1) {
		log_error(logger, "No se pudo establecer conexión con el servidor");						//TODO BIDIRECCIONAL VALEN
		terminar_programa(conexionMemoria, logger, config);
		return EXIT_FAILURE;
	}

	enviar_mensaje("Hola memoria desde KERNEL", conexionMemoria,logger);   //prueba

	log_info(logger, "Mensaje enviado, esperando respuesta del memoria...");
    while (1) {
        int cod_op = recibir_operacion(conexionMemoria);  
        switch (cod_op) {
            case MENSAJE:
                recibir_mensaje(logger, conexionMemoria);
                break;
            case PAQUETE:
                t_list* paquete = recibir_paquete(conexionMemoria);

                break;
            case -1:
                log_error(logger, "Se cerró la conexión con el Kernel");
                return EXIT_FAILURE;
            default:
                log_warning(logger, "Operación desconocida recibida");
                break;
        }
    }

   paquete(conexionMemoria);				//preguntar para que sirve y para que esta al final 

   

   
//===========================================CONEXION KERNEL (SERVER) CON IO (CLIENTE)========================================================//
	char* puerto_io;
	
	puerto_io = config_get_string_value (config,"PUERTO_ESCUCHA_IO");
    
	int server_fd =  iniciar_servidor(logger, "kernelServer",ip_memoria, puerto_io);       //ip_memoria?
    log_info(logger, "Esperando cliente en el puerto %s", puerto_io);
    int cliente_fd = esperar_cliente(logger, "kernelServer", server_fd);
    
    t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
				recibir_mensaje(logger, cliente_fd);
				enviar_mensaje("Hola IO, mensaje recibido.", cliente_fd,logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "El cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}return EXIT_SUCCESS;

	terminar_programa(conexionMemoria, logger, config);
	
	
return 0;
}
void iterator(char* value) {
	t_log* logger = iniciar_logger("kernel.log", "Kernel");
    log_info(logger, "%s", value);  // Usamos el logger que se pasa como argumento
}