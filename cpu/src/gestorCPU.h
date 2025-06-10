#ifndef GESTOR_CPU_H_
#define GESTOR_CPU_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <signal.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/memory.h>
#include <readline/readline.h>

#include <utils/utils.h>
//#include "inicializar_estructuras.h"
//#include "contexto.h"
//#include "utils_pcb.h"
//#include "instrucciones.h"



extern t_log* cpu_logger;
extern t_config* cpu_config;

extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* IP_KERNEL;
extern char* PUERTO_KERNEL_DISPATCH;
extern char* PUERTO_KERNEL_INTERRUPT;

extern int cliente_de_kernel_interrupt,cliente_de_kernel_dispatch, socket_memoria;


extern pthread_mutex_t mutex_pcb_actual;
extern pthread_mutex_t mutex_interrupt;

//extern t_pcb* pcb_actual;

#endif