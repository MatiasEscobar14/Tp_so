<<<<<<< HEAD
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
=======
#include "memoria.h"

int main(int argc, char* argv[]){
	
	/*logger_memoria = iniciar_logger("memoria.log", "Memoria");
    iniciar_config_memoria("memoria.config");
    
    int socket_memoria =  iniciar_servidor(logger_memoria, "Memoria Server",PUERTO_ESCUCHA);
    log_info(logger_memoria, "Esperando cliente en el puerto %s", PUERTO_ESCUCHA);
    
	while(server_escuchar(logger_memoria, "Memoria Server", socket_memoria));*/

>>>>>>> c047ab4210c130883fab0dfce669ec9e94bab8c5
    


    return 0;

}
