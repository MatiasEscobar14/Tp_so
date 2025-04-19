#include <utils/utils.h>
#include <utils/protocolo.h>

int main(int argc, char* argv[]){
	
	char* puerto_escucha;
	
    t_log* logger = iniciar_logger("loggerMemoria.log", "Memoria");


    t_config* config = iniciar_config("memoria.config"); 
	
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");   
    
    int server_fd =  iniciar_servidor(logger, "Memoria Server",puerto_escucha);
    log_info(logger, "Esperando cliente en el puerto %s", puerto_escucha);
    
	while(server_escuchar(logger, "Memoria Server", server_fd));


    return 0;

}
