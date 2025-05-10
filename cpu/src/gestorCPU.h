#ifndef GESTOR_CPU_H_
#define GESTOR_CPU_H_

#include <utils/utils.h>
#include "inicializar_estructuras.h"


extern t_log* cpu_logger;
extern t_config* cpu_config;

extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* IP_KERNEL;
extern char* PUERTO_KERNEL_DISPATCH;
extern char* PUERTO_KERNEL_INTERRUPT;

extern int cliente_de_kernel_interrupt,cliente_de_kernel_dispatch, socket_memoria;


#endif