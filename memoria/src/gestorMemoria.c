#include "gestorMemoria.h"

t_log* logger_memoria;
t_config* memoria_config;


char* PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
int ENTRADAS_POR_TABA;
int CANTIDAD_NIVELES;
int RETARDO_MEMORIA;
int RETARDO_SWAP;

//SOCKETS

int socket_memoria;

//listas

// Mutex
//pthread_mutex_t mutex_lista_procesos;
