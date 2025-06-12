#include "conexiones_io.h"

int main(int argc, char *argv[])
{

    // if(argc < 2) {
    //     printf("Uso: %s [NOMBRE_DISPOSITIVO]\n", argv[0]);
    //     return EXIT_FAILURE;
    // }

    char *nombre_dispositivo = argv[1];
    char *mensaje_presentacion = string_from_format(nombre_dispositivo);

    //===========================================CONEXION IO (CLIENTE) CON KERNEL (SERVER)========================================================//

    t_log *logger = iniciar_logger("loggerIo.log", "Io");
    t_config *config = iniciar_config("io.config");

    log_info(logger, "Logger e config iniciados");

    int socket_cliente = crear_conexiones_io(logger, config);
    
    t_buffer *buffer = new_buffer();
    add_string_to_buffer(buffer, mensaje_presentacion);
    //add_int_to_buffer(buffer, socket_cliente);
    t_paquete *paquete = crear_paquete(HANDSHAKE, buffer);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
    // Bucle infinito para mantener el módulo IO activo
    while (1)
    {
        int cod_op = recibir_operacion(socket_cliente);
        switch (cod_op)
        {
        case MENSAJE:
            log_info(logger, "Recibida solicitud de IO del Kernel.");
            // Aquí podrías procesar la solicitud de IO
            break;
        case REALIZAR_IO:
            t_buffer* un_buffer = recv_buffer(socket_cliente);
            int pid = extraer_int_buffer(un_buffer);
            int tiempo_ms = extraer_int_buffer(un_buffer);
            log_info(logger, "Inicio de IO: ## PID: <%d> - Inicio de IO - Tiempo: <%d>.", pid, tiempo_ms);

            usleep(tiempo_ms * 1000);

            log_info(logger,"Finalizacion de IO: ## PID: <%d> - Fin de IO.", pid);
            // TODO informar a kernal que finalizo el IO
            t_buffer *buffer_respuesta = new_buffer();
            add_int_to_buffer(buffer_respuesta, pid);
            t_paquete *paquete_respuesta = crear_paquete(FIN_IO, buffer_respuesta);
            enviar_paquete(paquete_respuesta, socket_cliente);
            eliminar_paquete(paquete_respuesta);
            free(un_buffer);
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
