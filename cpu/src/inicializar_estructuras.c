#include "inicializar_estructuras.h"

void iniciar_config_cpu(char* ruta){
    cpu_config = config_create(ruta);
    
    if(cpu_config == NULL){
        log_error(cpu_logger, "No se pudo leer el archivo de configuracion");
        exit(EXIT_FAILURE);
    }
    IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
    IP_KERNEL = config_get_string_value(cpu_config, "IP_KERNEL");
    PUERTO_KERNEL_DISPATCH = config_get_string_value(cpu_config, "PUERTO_KERNEL_DISPATCH");
    PUERTO_KERNEL_INTERRUPT = config_get_string_value(cpu_config, "PUERTO_KERNEL_INTERRUPT");
}

void iniciar_logger_cpu(char* nombre){
    cpu_logger = log_create(nombre, "CPU", 1, LOG_LEVEL_INFO);

     if(cpu_logger == NULL){
        log_error(cpu_logger, "No se pudo leer el logger");
        exit(EXIT_FAILURE);
    }
}