#include "memoria.h"

pthread_mutex_t mutex_lista_procesos;

t_proceso_memoria* proceso_memoria;

pthread_mutex_t mutex_procesos;

int main(int argc, char *argv[])
{

    logger_memoria = iniciar_logger("memoria.log", "Memoria");
    iniciar_config_memoria("memoria.config");

    socket_memoria = iniciar_servidor(logger_memoria, "Memoria Server", PUERTO_ESCUCHA);
    log_info(logger_memoria, "Esperando cliente en el puerto %s", PUERTO_ESCUCHA);


    //pthread_t hilo_memoria_cpu, 
    pthread_t hilo_memoria_kernel;
    //pthread_create(&hilo_memoria_cpu, NULL, (void *)attend_memoria_cpu, NULL);
    //pthread_detach(hilo_memoria_cpu);

    pthread_create(&hilo_memoria_kernel, NULL, (void *)esperar_kernel, NULL);
    pthread_join(hilo_memoria_kernel, NULL);

    return 0;
}


