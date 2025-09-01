#include "memoria.h"
t_proceso* proceso_memoria;

int main(int argc, char *argv[]){
    iniciar_memoria("memoria.config");
   pthread_t hilo_memoria_cpu; 
    pthread_t hilo_memoria_kernel;

    pthread_create(&hilo_memoria_cpu, NULL, (void *)esperar_cpu, NULL);
    pthread_detach(hilo_memoria_cpu);

    pthread_create(&hilo_memoria_kernel, NULL, (void *)esperar_kernel, NULL);
    pthread_join(hilo_memoria_kernel, NULL);

    return 0;
}

