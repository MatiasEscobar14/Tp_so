#include <utils/utils.h>
#include <utils/protocolo.h>

#include "memoria-kernel.h"

void attender_memoria_kernel(int socket_kernel){

	t_buffer* un_buffer;
    log_info(logger_memoria,"Esperando mensaje del kernel");
    int cod_op = recibir_operacion(socket_kernel);   
    int size;  
    switch (cod_op) {
   
        case INICIALIZAR_ESTRUCTURAS_KM:
            un_buffer = recibir_buffer(&size,socket_kernel);
            inicializar_estructuras(un_buffer,socket_kernel);
            free(un_buffer);
        break;

    }
    free(un_buffer);

}

void inicializar_estructuras(t_buffer* buffer, int socket_kernel){
    log_info(logger_memoria, "HOLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    int pid = extraer_int_buffer(buffer);
    //int pc = extraer_int_buffer(buffer);
    int tamanio = extraer_int_buffer(buffer);
    //char* path = extraer_string_buffer(buffer);

    log_info(logger_memoria,"Solicitud de inicialización PID: %d, tamanio: %d", pid,tamanio);
    
    t_buffer* otro_buffer = new_buffer();
    if (hay_espacio_disponible(tamanio)){
       // t_proceso* proceso = crear_proceso(pid,tamanio);
        log_info(logger_memoria, "## Proceso Creado - PID: %d - Tamanio: %d",pid,tamanio);

        add_int_to_buffer(otro_buffer,1);
    }else{
        add_int_to_buffer(otro_buffer,0);
    }
    //free(path);
    t_paquete* rta_kernel = crear_paquete(RTA_INICIALIZAR_ESTRUCTURAS_MK,otro_buffer);
    enviar_paquete(rta_kernel,socket_kernel);
    eliminar_paquete(rta_kernel);
    
    return;
}
/*
int memoria_ocupada = 0;  // Inicialmente 0
int TAM_MEMORIA;          // Cargado desde el config
pthread_mutex_t mutex_memoria_ocupada = PTHREAD_MUTEX_INITIALIZER;
*/
bool hay_espacio_disponible(int size_proceso) {
    //pthread_mutex_lock(&mutex_memoria_ocupada);  // Bloquea el acceso a la variable compartida
    //bool resultado = (memoria_ocupada + size_proceso) <= TAM_MEMORIA;  // Verifica si hay espacio
    //pthread_mutex_unlock(&mutex_memoria_ocupada);  // Libera el acceso
    //return resultado;
}

t_proceso* crear_proceso(int pid, int tamanio){
    t_proceso* proceso = malloc(sizeof(t_proceso));
    proceso->pid = pid;
    proceso->tamanio = tamanio;
    //proceso->tabla_paginas = list_create();
    //proceso->path = path;
    return proceso;
}