#include "cpu.h"
#include "inicializar_estructuras.h"
/*pthread_mutex_t mutex_pcb_actual;
pthread_mutex_t mutex_interrupt;*/

int main(int argc, char *argv[])
{

    int identificador = 5; //atoi(argv[1]);
    char* nombre_log = string_from_format("cpu_%d.log", identificador);

    iniciar_config_cpu("cpu.config");
    iniciar_logger_cpu(nombre_log);

    log_info(cpu_logger, "Logger e config iniciados");

    conectar_con_kernel(identificador);

    log_info(cpu_logger, "===================================================");
    t_buffer* buffer = new_buffer();
            add_string_to_buffer(buffer, "proceso2");
            add_int_to_buffer(buffer, 30);
            t_paquete* nuevo_paquete = crear_paquete(INIT_PROC, buffer);
            enviar_paquete(nuevo_paquete, cliente_de_kernel_dispatch);
            log_info(cpu_logger, "Envie SYSCALL-INITI al kernel");
            atender_peticion_kernel(cliente_de_kernel_dispatch);
            log_info(cpu_logger, "El proceso se creo pelucheee.");
    log_info(cpu_logger, "===================================================");
    //cliente_de_memoria = crear_conexion(logger, "Server Memoria", ip_memoria, puerto_memoria);

    return 0;
}

void conectar_con_kernel(int identificador)
{
    // Para que es esta validacion? TODO valen
    if (cliente_de_kernel_interrupt == -1 || cliente_de_kernel_dispatch == -1) {
    log_error(cpu_logger, "No se pudo conectar con el Kernel");
    exit(EXIT_FAILURE);
    }

    cliente_de_kernel_interrupt = crear_conexion(cpu_logger, "Server kernel interrupt", IP_KERNEL, PUERTO_KERNEL_INTERRUPT);
    cliente_de_kernel_dispatch = crear_conexion(cpu_logger, "Server kernel dispatch", IP_KERNEL, PUERTO_KERNEL_DISPATCH);
    t_buffer* un_buffer = new_buffer();
    add_int_to_buffer(un_buffer, identificador);
    t_paquete* un_paquete = crear_paquete(HANDSHAKE, un_buffer);
    log_info(cpu_logger, "HANDSHAKE de la CPU: %d enviado.", identificador);
    enviar_paquete(un_paquete, cliente_de_kernel_dispatch);
    //TODO valen, el socket interrupt deberiaa pasarlo?

    
    return;
}  

void atender_peticion_kernel(int cliente_de_kernel_dispatch) {
    int cod_op = recibir_operacion(cliente_de_kernel_dispatch);
    
    switch (cod_op) {
        case MENSAJE:
            log_info(cpu_logger, "Recibida solicitud de CPU del Kernel.");
            // Aquí podrías procesar la solicitud de IO
            /*
            t_buffer* buffer = new_buffer();
            add_string_to_buffer(buffer, "proceso2");
            add_int_to_buffer(buffer, 30);
            t_paquete* nuevo_paquete = crear_paquete(INIT_PROC, buffer);
            enviar_paquete(nuevo_paquete, cliente_de_kernel_dispatch);
            log_info(cpu_logger, "Envie SYSCALL-IO al kernel");
            */
            break;
            
        case -1:
            log_error(cpu_logger, "Se desconectó el Kernel.");
            close(cliente_de_kernel_dispatch);
            break;
            
        default:
            break;
    }
    return;
}
