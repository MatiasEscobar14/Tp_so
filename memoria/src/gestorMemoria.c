#include "gestorMemoria.h"

t_log* logger_memoria;
t_config* memoria_config;


char* PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
int ENTRADAS_POR_TABLA;
int CANTIDAD_NIVELES;
char* PATH_SWAPFILE;
int RETARDO_MEMORIA;
int RETARDO_SWAP;
char* PATH_INSTRUCCIONES;

//SOCKETS

int socket_memoria;

//listas

t_list* lista_procesos;
t_list* procesos_en_swap;
t_list* lista_marcos;

// Mutex
pthread_mutex_t mutex_lista_procesos;
pthread_mutex_t mutex_procesos;
pthread_mutex_t mutex_lista_marcos;
