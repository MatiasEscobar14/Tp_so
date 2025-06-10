/*#ifndef GESTOR_H_
#define GESTOR_H_

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/memory.h>
#include <readline/readline.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/memory.h>
#include <readline/readline.h>

#include </home/utnso/tp-2025-1c-Linux-Learners/utils/src/utils/protocolo.h>
#include "/home/utnso/tp-2025-1c-Linux-Learners/memoria/src/memoria.h"


typedef struct
{
    uint32_t pid;
    char *path;
    int tamanio;
    t_list *instrucciones;
    t_list *tabla_paginas;
    //t_tabla_paginas *tabla_paginas;
} t_proceso_memoria;

extern int server_fd_memoria;
extern t_proceso_memoria *proceso_memoria;
extern pthread_mutex_t mutex_comunicacion_procesos;    
extern pthread_mutex_t mutex_procesos;
extern pthread_mutex_t mutex_memoria_usuario;
extern int RETARDO_MEMORIA;
extern char* PATH_INSTRUCCIONES;


#endif */