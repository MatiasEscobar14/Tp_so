#include "conexionesKernel.h"

void* hilo_servidor_io_f(void* args) {
    datos_servidor_t* datos = (datos_servidor_t*) args;
    socket_io = iniciar_servidor(datos->logger, datos->nombre_servidor, datos->puerto);

    server_escuchar_io();

    free(datos);    
    return 0;
} 

void* hilo_servidor_dispatch(void* args) {
    datos_servidor_dispatch_t* datos_dispatch = (datos_servidor_dispatch_t*) args;
    socket_cpu_dispatch = iniciar_servidor(datos_dispatch->logger, datos_dispatch->nombre_servidor, datos_dispatch->puerto);

    server_escuchar_cpu_dispatch();
    
    free(datos_dispatch);    
    return 0;
} 

void* hilo_servidor_interrupt(void* args) {
    datos_servidor_interrupt_t* datos_interrupt = (datos_servidor_interrupt_t*) args;
    socket_cpu_interrupt = iniciar_servidor(datos_interrupt->logger, datos_interrupt->nombre_servidor, datos_interrupt->puerto);

    free(datos_interrupt);    
    return 0;
} 

void* hilo_cliente_memoria_f(void* args) {
    datos_conexion_t* datos = (datos_conexion_t*) args;
    socket_memoria = crear_conexion(datos->logger, datos->nombre_cliente,datos->ip, datos->puerto);
    free(datos);
    return 0;
}

//=====================================================================

void crear_conexiones(void* arg){
    pthread_t hilo_servidor_io;
    pthread_t hilo_cliente_memoria;
    pthread_t hilo_dispatch;
    pthread_t hilo_interrupt;

   datos_servidor_t* datos = malloc(sizeof(datos_servidor_t));
    datos->logger = kernel_logger;
    datos->nombre_servidor = "Kernel-IO";
    datos->puerto = PUERTO_ESCUCHA_IO;

    if(pthread_create(&hilo_servidor_io, NULL, hilo_servidor_io_f, datos) != 0 ) {
        perror("Error al crear el hilo del servidor");
        return;
    }

    //=====================================================================
   
    datos_servidor_dispatch_t* datos_dispatch = malloc(sizeof(datos_servidor_dispatch_t));
    datos_dispatch->logger = kernel_logger;
    datos_dispatch->nombre_servidor = "Kernel Dispatch";
    datos_dispatch->puerto = PUERTO_ESCUCHA_DISPATCH;

    if(pthread_create(&hilo_dispatch, NULL, hilo_servidor_dispatch, datos_dispatch) != 0 ) {
        perror("Error al crear el hilo del servidor");
        return;
    }

    //=====================================================================
    
    datos_servidor_interrupt_t* datos_interrupt = malloc(sizeof(datos_servidor_interrupt_t));
    datos_interrupt->logger = kernel_logger;
    datos_interrupt->nombre_servidor = "Kernel Interrupt";
    datos_interrupt->puerto = PUERTO_ESCUCHA_INTERRUPT;

    if(pthread_create(&hilo_interrupt, NULL, hilo_servidor_interrupt, datos_interrupt) != 0 ) {
        perror("Error al crear el hilo del servidor");
        return;
    }

     //=====================================================================
        
    datos_conexion_t* datosConexion = malloc(sizeof(datos_conexion_t));
    datosConexion->logger = kernel_logger;
    datosConexion->nombre_cliente = "Memoria";
    datosConexion->ip = config_get_string_value(kernel_config, "IP_MEMORIA");
    datosConexion->puerto = PUERTO_MEMORIA;

    if(pthread_create(&hilo_cliente_memoria, NULL, hilo_cliente_memoria_f, datosConexion) != 0 ) {
        perror("Error al crear el hilo del cliente");
        return;
    }


    pthread_join(hilo_servidor_io, NULL);       //SERVER PARA IO
    pthread_join(hilo_cliente_memoria, NULL);   //CLIENTE DE MEMORIA
    pthread_join(hilo_dispatch, NULL);          //TODO SERVER INTERRUPT PARA CPU
    pthread_join(hilo_interrupt, NULL);         //TODO SERVER DISPATCH PARA CPU
    
}

