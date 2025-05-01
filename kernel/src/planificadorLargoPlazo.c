#include "planificadorLargoPlazo.h"

// Variable global para gestionar la lista de procesos y la sincronizacion
t_list* lista_new;              // Lista de procesos nuevos
t_list* lista_ready;            // Lista de procesos listos para ejecutarse
t_list* lista_ready_thread;     // Lista de hilos listos
sem_t sem_rpta_estructura_inicializada; // Semaforo para sincronizar la respuesta de memoria
pthread_mutex_t mutex_lista_new = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_ready = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_ready_thread = PTHREAD_MUTEX_INITIALIZER;
bool flag_pedido_de_memoria;
pthread_mutex_t mutex_flag_pedido_memoria;


// Definir el semáforo global
sem_t semaforo_largo_plazo;  // Semáforo que controla la planificación

// Función para esperar a que el usuario presione Enter
void* esperar_enter(void* arg) {
    printf("Presione Enter para iniciar el Planificador de Largo Plazo...\n");
    getchar();  // Espera que el usuario presione Enter
    printf("Planificador de Largo Plazo iniciado.\n");

    // Liberar el semáforo, permitiendo que el algoritmo de largo plazo continúe
    sem_post(&semaforo_largo_plazo);  
    return NULL;
}

// Función que inicia el Planificador de Largo Plazo (bloqueado por el semáforo)
void iniciar_plp() {
    // Inicializar el semáforo en valor 0 (bloqueado)
    if (sem_init(&semaforo_largo_plazo, 0, 0) != 0) {
        perror("Error al inicializar el semáforo");
        return;
    }

    // Crear un hilo para esperar el Enter del usuario
    pthread_t hilo_entrada;
    pthread_create(&hilo_entrada, NULL, esperar_enter, NULL);

    // Bloqueamos el semáforo, esperando a que el usuario presione Enter
    sem_wait(&semaforo_largo_plazo);

    // Aquí comienza el Planificador de Largo Plazo
    planificadorLargoPlazo();  // Llamada al planificador de largo plazo

    // Esperar que el hilo de entrada termine
    pthread_join(hilo_entrada, NULL);
    sem_destroy(&semaforo_largo_plazo);
}


// Funcion que implementa el algoritmo del Planificador de Largo Plazo
void planificadorLargoPlazo() {
    t_pcb* pcb = NULL;
    
    pthread_mutex_lock(&mutex_lista_new);  // Bloqueamos la lista de procesos en NEW

    // Si la lista de NEW no esta vacia, comenzamos a procesar
    if (!list_is_empty(lista_new)) {
        // Obtiene el primer proceso en la lista de NEW
        pcb = list_get(lista_new, 0);  
        if (pcb != NULL) {
            // Enviamos a memoria para inicializar las estructuras del proceso
            t_buffer* buffer = new_buffer();  
            add_int_to_buffer(buffer, pcb->pid);
            //add_string_to_buffer(buffer, pcb->path);   //TODO preguntar si es necesario agregar el path como atributa  de la pcb
            add_int_to_buffer(buffer, pcb->tamanio_proceso);
            t_paquete* un_paquete = create_super_pck(INICIALIZAR_ESTRUCTURAS_KM, buffer);

            enviar_paquete(un_paquete, socket_memoria);

            log_info(kernel_logger, "Se aviso a Memoria para la creacion del proceso");

            // Espera hasta que la memoria haya inicializado las estructuras
            sem_wait(&sem_rpta_estructura_inicializada);

            pthread_mutex_lock(&mutex_flag_pedido_memoria);  // Bloqueamos la estructura de la memoria
            if (flag_pedido_de_memoria) {
                // Si la memoria respondio correctamente, removemos el PCB de la lista NEW
                list_remove_element(lista_new, pcb);

                log_info(kernel_logger, "Creacion de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", pcb->pid);
                agregar_pcb_lista(pcb, lista_ready, mutex_lista_ready);
                cambiar_estado(pcb, READY_PROCCES);  // Cambiamos su estado a READY
                log_info(kernel_logger, "Creacion de Proceso: ## (%d:0) Se cambia el estado a READY", pcb->pid);
            }
            pthread_mutex_unlock(&mutex_flag_pedido_memoria);
        }
    }
    pthread_mutex_unlock(&mutex_lista_new);
}
