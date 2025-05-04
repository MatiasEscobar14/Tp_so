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
        if (pthread_create(&thread, NULL, (void *)attender_memoria_kernel, (void *)(intptr_t)fd_conexion))
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
        /*case FINALIZAR_ESTRUCTURAS_KM:
            un_buffer = recibir_buffer(&size, socket_kernel);
          //  finalizar_estructuras(un_buffer, socket_kernel);
            free(un_buffer);
        break;*/
    }   
   // free(un_buffer);
}

void inicializar_estructuras(t_buffer *buffer, int socket_kernel)
{
    log_info(logger_memoria, "HOLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    int pid = extract_int_buffer(buffer);
    int tamanio = extract_int_buffer(buffer);

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

int memoria_ocupada = 0;

bool hay_espacio_disponible(int tamanio_proceso)
{

    return (memoria_ocupada + tamanio_proceso <= TAM_MEMORIA);
}

t_proceso* crear_proceso(int pid, int tamanio_proceso)
{
    log_info(logger_memoria, "Entre a Crear Proceso");
    t_proceso* nuevo_proceso = malloc(sizeof(t_proceso));
    nuevo_proceso->pid = pid;
    nuevo_proceso->tamanio = tamanio_proceso;
    log_info(logger_memoria, "Cree el Proceso");

    pthread_mutex_lock(&mutex_lista_procesos);
    list_add(lista_procesos, nuevo_proceso);
    pthread_mutex_unlock(&mutex_lista_procesos);
    
    log_info(logger_memoria, "Return Proceso");
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
    t_paquete* respuesta = crear_paquete(ESTRUCTURA_LIBERADA_OK, NULL);
    enviar_paquete(respuesta, socket_kernel);
    eliminar_paquete(respuesta);
}
*/