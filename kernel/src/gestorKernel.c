#include "gestorKernel.h"

t_log* kernel_logger;
t_config* kernel_config;
t_algoritmo* ALGORITMO_PLANIFICACION;

char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;
char* PUERTO_ESCUCHA_IO;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;
t_algoritmo ALGORITMO_CORTO_PLAZO;
t_algoritmo ALGORITMO_INGRESO_A_READY;
char* ESTIMACION_INICIAL;

int socket_io;
int socket_cpu_dispatch;
int socket_cpu_interrupt;
int socket_memoria;


t_list* lista_io_esperas;
pthread_mutex_t mutex_lista_io_esperas = PTHREAD_MUTEX_INITIALIZER;

