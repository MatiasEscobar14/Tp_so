#include "gestorCPU.h"

t_log* cpu_logger;
t_config* cpu_config;


char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_KERNEL;
char* PUERTO_KERNEL_DISPATCH;
char* PUERTO_KERNEL_INTERRUPT;

int cliente_de_kernel_interrupt,cliente_de_kernel_dispatch, socket_memoria;

pthread_mutex_t mutex_pcb_actual;
pthread_mutex_t mutex_interrupt;


t_pcb *pcb_actual;