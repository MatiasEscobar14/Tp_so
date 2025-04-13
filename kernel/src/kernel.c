#include <utils/utils.h>
#include "kernel.h"


int main(int argc, char* argv[]) {

    char* ip;
	char* puerto;
    t_log* logger = iniciar_logger("kernel.log");
    log_info(logger, "Hola mundo");

    t_config* config = iniciar_config("kernel.config"); 
    ip = config_get_string_value(config, "IP_MEMORIA");
	puerto = config_get_string_value (config,"PUERTO_MEMORIA");
    
	int conexionMemoria = crear_conexion(logger, "memoriaSV", ip, puerto);
	if (conexionMemoria == -1) {
		log_error(logger, "No se pudo establecer conexión con el servidor");
		terminar_programa(conexionMemoria, logger, config);
		return EXIT_FAILURE;
	}
	enviar_mensaje("Hola memoria desde KERNEL", conexionMemoria,logger);

   paquete(conexionMemoria);

   terminar_programa(conexionMemoria, logger, config);
    

return 0;
}