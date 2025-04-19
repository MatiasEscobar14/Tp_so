#include "conexiones_io.h"

int main(int argc, char* argv[]) {

    //===========================================CONEXION IO (CLIENTE) CON KERNEL (SERVER)========================================================//
    
    t_log* logger = iniciar_logger("loggerIo.log", "Io");
    t_config* config = iniciar_config("io.config"); 

    log_info(logger, "Logger e config iniciados");

    int socket_cliente = crear_conexiones_io(logger, config);

    enviar_mensaje("Hola kernel desde io",socket_cliente, logger);


    
    return 0;
}
