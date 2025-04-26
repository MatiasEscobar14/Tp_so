#ifndef CONEXIONES_KERNEL_H
#define CONEXIONES_KERNEL_H

#include <utils/protocolo.h>
#include "gestorKernel.h"

typedef struct {
    t_log* logger;
    char* nombre_servidor;
    char* puerto;
}datos_servidor_t, datos_servidor_dispatch_t,datos_servidor_interrupt_t;

typedef struct {
    t_log* logger;
    char* nombre_cliente;
    char* ip;
    char* puerto;
}datos_conexion_t;

void* hilo_servidor(void* args);
void* hilo_cliente(void* args);
void* hilo_servidor_dispatch(void* args);
void* hilo_servidor_interrupt(void* args);
void crear_conexiones(t_log* logger, t_config* config);



#endif