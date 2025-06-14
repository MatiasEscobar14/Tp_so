#include "memoria-kernel.h"

void esperar_kernel()
{
    log_info(logger_memoria, "ESPERANDO COMUNICACION KERNEL");

    while (1)
    {
        int fd_conexion = accept(socket_memoria, NULL, NULL);
        // TODO: Validacion de error?
        log_info(logger_memoria, "## Kernel Conectado - FD: <%d>", fd_conexion);

        pthread_t thread;
        if (pthread_create(&thread, NULL, (void *)attender_memoria_kernel, (void *)(intptr_t) (fd_conexion)))
        {
            log_error(logger_memoria, "Error al crear hilo para Kernel");
            close(fd_conexion); // Cierra el socket si no se pudo crear el hilo.
        }
        else
        {
            pthread_detach(thread);
        }
    }
}

void attender_memoria_kernel(int socket_kernel)
{
    t_buffer *un_buffer;
    log_info(logger_memoria, "Esperando mensaje del kernel");
    int cod_op = recibir_operacion(socket_kernel);

    switch (cod_op)
    {
    case INICIALIZAR_ESTRUCTURAS_KM:
        un_buffer = recv_buffer(socket_kernel);
        
        inicializar_estructuras(un_buffer, socket_kernel);
        free(un_buffer->stream);
        free(un_buffer);
        break;
     case FINALIZAR_ESTRUCTURAS_KM:
            un_buffer = recv_buffer(socket_kernel);
         //   finalizar_estructuras(un_buffer, socket_kernel);
            free(un_buffer);
        break;
    case DUMP_MEMORY_KM:
        un_buffer = recv_buffer(socket_kernel);
        dumpear_memoria(un_buffer, socket_kernel);
        free(un_buffer);
        break;
    }   
   // free(un_buffer);
}

void inicializar_estructuras(t_buffer *buffer, int socket_kernel)
{

    int pid = extraer_int_buffer(buffer);
    int tamanio = extraer_int_buffer(buffer);

    log_info(logger_memoria, "Solicitud de inicialización PID: %d, tamanio: %d", pid, tamanio);

    t_buffer *otro_buffer = new_buffer();
    if (hay_espacio_disponible(tamanio)){
        log_info(logger_memoria, "Entre al IF (true)");
        t_proceso* proceso = crear_proceso(pid, tamanio);
        log_info(logger_memoria, "## Proceso Creado - PID: %d - Tamanio: %d", pid, tamanio);

        add_int_to_buffer(otro_buffer, 1);
    }
    else
    {
        log_info(logger_memoria, "Espacio insuficiente para el proceso - PID: %d", pid);
        add_int_to_buffer(otro_buffer, 0);
    }
    // free(path);
    t_paquete *rta_kernel = crear_paquete(RTA_INICIALIZAR_ESTRUCTURAS_MK, otro_buffer);
    enviar_paquete(rta_kernel, socket_kernel);
    eliminar_paquete(rta_kernel);
    //free(otro_buffer);
    return;
}



bool hay_espacio_disponible(int tamanio_proceso)
{
    return true;
}

t_proceso* crear_proceso(int pid, int tamanio_proceso)
{
    log_info(logger_memoria, "Entre a Crear Proceso");
    t_proceso* nuevo_proceso = malloc(sizeof(t_proceso));
    nuevo_proceso->pid = pid;
    nuevo_proceso->tamanio = tamanio_proceso;
    
    pthread_mutex_lock(&mutex_lista_procesos);
    list_add(lista_procesos, nuevo_proceso);
    pthread_mutex_unlock(&mutex_lista_procesos);
    
    return nuevo_proceso;
}
/*
void finalizar_estructuras(t_buffer* buffer, int socket_kernel) {
    int desplazamiento = 0;
    int pid = leer_int_de_buffer(buffer, &desplazamiento);

    log_info(logger_memoria, "Finalizando estructuras de proceso PID %d", pid);

    //liberar_tabla_de_paginas(pid);
    //liberar_swap(pid);
    //eliminar_metricas(pid);

    // Enviar confirmación al Kernel
    t_buffer* a_enviar = new_buffer();
    add_int_to_buffer(respuesta->buffer, 1); 
    t_paquete* respuesta = crear_paquete(RTA_ESTRUCTURA_LIBERADA_KM, a_enviar);
    enviar_paquete(respuesta, socket_kernel);
    eliminar_paquete(respuesta);
}
*/
int buscar_valor_pid;

bool buscar_pid(t_proceso* un_proceso){
    return un_proceso->pid == buscar_valor_pid;
}

t_proceso* buscar_proceso_pid(int pid){
    buscar_valor_pid = pid;
    pthread_mutex_lock(&mutex_lista_procesos);
    t_proceso* un_proceso = list_find(lista_procesos, (void*)buscar_pid);
    pthread_mutex_unlock(&mutex_lista_procesos);
    if(un_proceso == NULL){
        log_info(logger_memoria, "PID - (%d) no fue encontrado en la lista de procesos", pid);
        exit(EXIT_FAILURE);
    }
    return un_proceso;
}


void dumpear_memoria(t_buffer* un_buffer, int socket){
    int pid = extraer_int_buffer(un_buffer);
    log_info(logger_memoria, "## Memory Dump solicitado - (PID) - (%d)", pid);
    t_proceso* un_proceso = buscar_proceso_pid(pid);
    
}

//Devolver un valor fijo de espacio libre (mock)

int suma_tamanios = 0;

int valor_memoria_disponible(){

    suma_tamanios = 0;
    pthread_mutex_lock(&mutex_lista_procesos);
    list_iterate(lista_procesos, acumular_tamanio);
    pthread_mutex_unlock(&mutex_lista_procesos);
    return suma_tamanios;
}

void acumular_tamanio(void* elemento) {
    t_proceso_memoria* proceso = (t_proceso_memoria*) elemento;
    suma_tamanios += proceso->tamanio;
}
