#ifndef GESTOR_MEMORIA_H
#define GESTOR_MEMORIA_H

#include <utils/utils.h>
#include <utils/protocolo.h>

extern t_log* logger_memoria;
extern t_config* memoria_config;

extern char* PUERTO_ESCUCHA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern int ENTRADAS_POR_TABLA;
extern int CANTIDAD_NIVELES;
extern int RETARDO_MEMORIA;
extern int RETARDO_SWAP;
extern char* PATH_INSTRUCCIONES;

extern int socket_memoria;

typedef struct 
{
    uint32_t pid;
    int tamanio;
    
}t_proceso;
// Listas
extern t_list* lista_procesos;
//MUTEX

extern pthread_mutex_t mutex_lista_procesos;


#endif 

