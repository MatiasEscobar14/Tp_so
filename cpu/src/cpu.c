#include <utils/utils.h>
#include <utils/protocolo.h>

int cliente_de_memoria;
int cliente_de_kernel_interrupt;
int cliente_de_kernel_dispatch;

t_log* logger;
t_config* config;

pthread_mutex_t mutex_pcb_actual;
pthread_mutex_t mutex_interrupt;

int main(int argc, char* argv[]) {

    char* ip_memoria;
    char* puerto_memoria;
    char* ip_kernel;
    char* puerto_kernel_dispatch;
    char* puerto_kernel_interrupt;
    
    logger = iniciar_logger("loggerCPU.log", "Cpu");

    config = iniciar_config("cpu.config"); 
    
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");

    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel_dispatch = config_get_string_value(config,"PUERTO_KERNEL_DISPATCH");
    puerto_kernel_interrupt= config_get_string_value(config,"PUERTO_KERNEL_INTERRUPT");

    cliente_de_memoria = crear_conexion(logger, "Server Memoria", ip_memoria, puerto_memoria);
    cliente_de_kernel_interrupt = crear_conexion(logger, "Server kernel interrupt", ip_kernel,puerto_kernel_interrupt);
    cliente_de_kernel_dispatch = crear_conexion(logger,"Server kernel dispatch",ip_kernel,puerto_kernel_dispatch);

    return 0;
}

