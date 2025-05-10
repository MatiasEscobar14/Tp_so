#include "conexiones_io.h"

int main(int argc, char *argv[])
{

    // if(argc < 2) {
    //     printf("Uso: %s [NOMBRE_DISPOSITIVO]\n", argv[0]);
    //     return EXIT_FAILURE;
    // }

    char *nombre_dispositivo = argv[1];

    printf("[INFO] Dispositivo configurado: %s\n", nombre_dispositivo);

    char *mensaje_presentacion = string_from_format(nombre_dispositivo);

    //===========================================CONEXION IO (CLIENTE) CON KERNEL (SERVER)========================================================//

    t_log *logger = iniciar_logger("loggerIo.log", "Io");
    t_config *config = iniciar_config("io.config");

    log_info(logger, "Logger e config iniciados");

    int socket_cliente = crear_conexiones_io(logger, config);
    t_buffer *buffer = new_buffer();
    add_string_to_buffer(buffer, mensaje_presentacion);
    add_int_to_buffer(buffer, socket_cliente);
    t_paquete *paquete = crear_paquete(HANDSHAKE, buffer);
    enviar_paquete(paquete, socket_cliente);

    // Bucle infinito para mantener el módulo IO activo
    while (1)
    {
        int cod_op = recibir_operacion(socket_cliente);
        t_paquete *paquete_recibido = recibir_paquete(socket_cliente);

        switch (cod_op)
        {
        case MENSAJE:
            log_info(logger, "Recibida solicitud de IO del Kernel.");
            // Aquí podrías procesar la solicitud de IO
            break;

        case -1:
            log_error(logger, "Se desconectó el Kernel.");
            close(socket_cliente);
            return;

        default:
        }
    }
    // le enviará su nombre para que él lo pueda identificar y quedará esperando las peticiones del mismo.

    /*Al momento de recibir una petición del Kernel, el módulo deberá iniciar un usleep por el tiempo indicado en la request.*/

    return 0;
}
