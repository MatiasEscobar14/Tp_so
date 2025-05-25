#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_

#define _GNU_SOURCE

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <string.h>
#include <stdlib.h>
#include<readline/readline.h>
#include <pthread.h>
#include "protocolo.h"
#include <commons/string.h>
#include <commons/collections/list.h>
#include<commons/collections/queue.h>
#include <stdint.h>

typedef struct {
    t_log* log;
    int fd;
    char* server_name;
}t_procesar_conexion_args;

typedef enum
{
    NOOP,
    WRITE,
    READ, 
    GOTO,
    //IO,
    INIT_PROC,
    DUMP_MEMORY,
    EXIT
} nombre_instruccion;


typedef struct
{
    nombre_instruccion nombre;
    char *parametro1;
    char *parametro2;
    char *parametro3;
    char *parametro4;
    char *parametro5;
    uint32_t longitud_parametro1;
    uint32_t longitud_parametro2;
    uint32_t longitud_parametro4;
    uint32_t longitud_parametro3;
    uint32_t longitud_parametro5;
} t_instruccion;


int iniciar_servidor(t_log* logger, const char* name, char* puerto);
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);
int crear_conexion(t_log* logger, const char* server_name, char* ip, char* puerto);
void liberar_conexion(int socket_cliente); 

t_config* iniciar_config(char* nombreArchivoConfig);
t_log* iniciar_logger(char* nombreArchivoLog, char* nombreModulo);
void terminar_programa(int conexion, t_log* logger, t_config* config);

int server_escuchar(t_log* logger, char* server_name, int server_socket);
void procesar_conexion(void* void_args);
void iterator(char* value);


#endif