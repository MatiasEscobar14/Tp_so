#include <utils/utils.h>
#include <utils/protocolo.h>
#include "gestorCPU.h"


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
    t_paquete* un_paquete = crear_paquete(HANDSHAKE, un_buffer);
    enviar_paquete(un_paquete, cliente_de_kernel_dispatch);
    //atender_kernel_cpu_dispatch(cliente_de_kernel_dispatch);
    return;
}   

