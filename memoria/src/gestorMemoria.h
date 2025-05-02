#ifndef GESTOR_MEMORIA_H
#define GESTOR_MEMORIA_H

#include <utils/utils.h>

extern t_log* logger_memoria;
extern t_config* memoria_config;

extern char* PUERTO_ESCUCHA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern int ENTRADAS_POR_TABLA;
extern int CANTIDAD_NIVELES;
extern int RETARDO_MEMORIA;
extern int RETARDO_SWAP;

extern int socket_memoria;

typedef struct {
    int nivel;
    int entradas;
    void* tabla[];               //void porque puede apuntar a otra tabla o a un marco de memoria fisica
} tabla_pagina_t;

typedef struct {
    void* espacio_usuario;      // Memoria principal (4096B)
    size_t tam_pagina;          // 64B (para cálculos de desplazamiento)
    int entradas_por_nivel;     // 4 (para navegación en tablas)
    FILE* swapfile;             // Puntero al archivo swap
    t_list* procesos_activos;   // Lista de procesos en memoria
} memoria_t;


typedef struct 
{
    uint32_t pid;
    uint32_t tamanio;
    
}t_proceso;

#endif 

