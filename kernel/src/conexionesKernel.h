#ifndef CONEXIONES_KERNEL_H
#define CONEXIONES_KERNEL_H

#include <utils/protocolo.h>
#include "gestorKernel.h"
#include "kernel_cpu.h"
#include "kernel_IO.h"

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

void* hilo_servidor_io_f(void* args);
void* hilo_cliente_memoria_f(void* args);
void* hilo_servidor_dispatch(void* args);
void* hilo_servidor_interrupt(void* args);
void crear_conexiones();




#endif