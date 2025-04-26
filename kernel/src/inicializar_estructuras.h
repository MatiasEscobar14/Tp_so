#ifndef ESTRUCTURAS_KERNEL_H
#define ESTRUCTURAS_KERNEL_H

#include <utils/utils.h>
#include "gestorKernel.h"

//t_list* metricas_estado;
//t_list* metricas_tiempo;

void iniciar_lista();
void iniciar_logger_kernel();
void iniciar_config_kernel(char* ruta);
void iniciar_kernel(char* ruta_config);

#endif