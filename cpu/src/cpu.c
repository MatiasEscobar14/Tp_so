#include <utils/utils.h>
#include <utils/protocolo.h>
#include "gestorCPU.h"
#include "cpu.h"

/*pthread_mutex_t mutex_pcb_actual;
pthread_mutex_t mutex_interrupt;*/

int main(int argc, char *argv[])
{

    int identificador = atoi(argv[1]);
    
    printf("CPU %d iniciada\n", identificador);

    char nombre_log[64];
    sprintf(nombre_log, "cpu_%d.log", identificador);
   
    iniciar_config_cpu("cpu.config");
    iniciar_logger_cpu(nombre_log);

    log_info(cpu_logger, "Logger e config iniciados");

    conectar_con_kernel(identificador);

    // cliente_de_memoria = crear_conexion(logger, "Server Memoria", ip_memoria, puerto_memoria);

    return 0;
}

void conectar_con_kernel(int identificador)
{
    cliente_de_kernel_interrupt = crear_conexion(cpu_logger, "Server kernel interrupt", IP_KERNEL, PUERTO_KERNEL_INTERRUPT);
    cliente_de_kernel_dispatch = crear_conexion(cpu_logger, "Server kernel dispatch", IP_KERNEL, PUERTO_KERNEL_DISPATCH);
    t_buffer* un_buffer = new_buffer();
    add_int_to_buffer(un_buffer, identificador);
    log_info(cpu_logger, "llegue hasta el add buffer");
    t_paquete* un_paquete = crear_paquete(HANDSHAKE, un_buffer);
    log_info(cpu_logger, "cree el paquete");
    enviar_paquete(un_paquete, cliente_de_kernel_dispatch);
    log_info(cpu_logger, "envie el paquete");
     while (1)
    {
        int cod_op = recibir_operacion(cliente_de_kernel_dispatch);
        t_paquete *paquete_recibido = recibir_paquete(cliente_de_kernel_dispatch);

        switch (cod_op)
        {
        case MENSAJE:
            log_info(cpu_logger, "Recibida solicitud de CPU del Kernel.");
            // Aquí podrías procesar la solicitud de IO
            break;

        case -1:
            log_error(cpu_logger, "Se desconectó el Kernel.");
            close(cliente_de_kernel_dispatch);
            return;

        default:
        }
    }
    return;
}   
