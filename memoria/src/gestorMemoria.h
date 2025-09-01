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
extern char* PATH_SWAPFILE;
extern int RETARDO_MEMORIA;
extern int RETARDO_SWAP;
extern char* PATH_INSTRUCCIONES;

extern int socket_memoria;

extern pthread_mutex_t mutex_procesos;
extern pthread_mutex_t mutex_lista_marcos;

extern void* espacio_usuario; 
extern t_list* lista_metricas;
extern bool* bitmap_marcos;
extern uint32_t cantidad_marcos;

typedef struct{
    int pid;
    int base;
    bool libre;
    int tamanio;
    int marco_id;
}t_marco;

typedef struct {
    int nro_pagina;
    bool presente;
    bool uso;
    bool modificado;
    int numero_marco;
    struct tabla_pagina* siguiente_nivel;
} entrada_tabla_pagina;

typedef struct {
    int id_tabla;
    t_list* entradas;
} tabla_pagina;

typedef struct {
    uint32_t pid;
    int accesos_tablas_paginas;
    int instrucciones_solicitadas;
    int bajadas_swap;
    int subidas_memoria;
    int lecturas_memoria;
    int escrituras_memoria;
} t_metricas_proceso;


typedef struct 
{
    uint32_t pid;
    int tamanio;
   // lista_marcos;
    tabla_pagina* tabla_1er_nivel;
}t_proceso;

typedef struct
{
    uint32_t pid;
    char *path;
    int tamanio;
    t_list *instrucciones;
    t_list *tabla_paginas;
    //t_tabla_paginas *tabla_paginas;
} t_proceso_memoria;

typedef struct{
    int pid;
    int nro_pagina;
    int offset_en_swap;
}t_pagina_swap;
// Listas
extern t_list* lista_procesos;
extern t_list* procesos_en_swap;
extern t_list* lista_marcos;
//MUTEX

extern pthread_mutex_t mutex_lista_procesos;



void iniciar_swap();
void escribir_pagina_en_swap(int nro_pagina, void* datos);
void leer_pagina_de_swap(int nro_pagina, void* destino);
void loguear_metricas_proceso(uint32_t pid);
t_metricas_proceso* obtener_metricas_por_pid(uint32_t pid);
void crear_metricas_para_proceso(uint32_t pid);


#endif 

