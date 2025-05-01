#include <utils/utils.h>
#include <utils/protocolo.h>
#include "gestorMemoria.h"

int main(int argc, char* argv[]){
	
	char* puerto_escucha;
	
    logger = iniciar_logger("loggerMemoria.log", "Memoria");
    config = iniciar_config("memoria.config"); 
	
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");   
    
    socket_memoria =  iniciar_servidor(logger, "Memoria Server",puerto_escucha);
    log_info(logger, "Esperando cliente en el puerto %s", puerto_escucha);
    
	while(server_escuchar(logger, "Memoria Server", server_fd));


    return 0;

}
