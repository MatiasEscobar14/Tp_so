#include "conexiones_io.h"

int main(int argc, char* argv[]) {

    if(argc < 2) {
        printf("Uso: %s [NOMBRE_DISPOSITIVO]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* nombre_dispositivo = argv[1]; 

    printf("[INFO] Dispositivo configurado: %s\n", nombre_dispositivo);

    char* mensaje_presentacion = string_from_format("Hola kernel desde: %s", nombre_dispositivo);


    //===========================================CONEXION IO (CLIENTE) CON KERNEL (SERVER)========================================================//
    
    t_log* logger = iniciar_logger("loggerIo.log", "Io");
    t_config* config = iniciar_config("io.config"); 

    log_info(logger, "Logger e config iniciados");
    
    int socket_cliente = crear_conexiones_io(logger, config);
    enviar_mensaje(mensaje_presentacion, socket_cliente, logger);

    //recibir_mensaje(logger, socket_cliente);  ESPERAR PETICIONES DE KERNEL

    /*Al momento de recibir una petición del Kernel, el módulo deberá iniciar un usleep por el tiempo indicado en la request.*/

    return 0;
}
