#include "gestorKernel.h"

t_log* kernel_logger;
t_config* kernel_config;


char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;
char* PUERTO_ESCUCHA_IO;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;

int socket_io;
int socket_cpu_dispatch;
int socket_cpu_interrupt;
int socket_memoria;