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


sem_t semaforo_largo_plazo;  // Semáforo que controla la planificación

// Función para esperar a que el usuario presione Enter
void* esperar_enter(void* arg) {
    printf("Presione Enter para iniciar el Planificador de Largo Plazo...\n");
    getchar();  
    printf("Planificador de Largo Plazo iniciado.\n");

    sem_post(&semaforo_largo_plazo);  
    return NULL;
}


void iniciar_plp() {

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
    planificadorLargoPlazo();  

    // Esperar que el hilo de entrada termine
    pthread_join(hilo_entrada, NULL);
    sem_destroy(&semaforo_largo_plazo);
}


/*
void planificadorLargoPlazo() {
    pcb_t* pcb = NULL;
    
    pthread_mutex_lock(&mutex_lista_new);  // Bloqueamos la lista de procesos en NEW
    
    // Si la lista de NEW no esta vacia, comenzamos a procesar
    if (!list_is_empty(lista_new)) {
        // Se procesa dependiendo del algoritmo de planificación (FIFO o SJF)
        if (strcmp(ALOGRITMO_PLANIFICACION, "FIFO") == 0) {
            // FIFO: El primer proceso en la cola se procesa
            pcb = list_get(lista_new, 0);
        } else if (strcmp(ALOGRITMO_PLANIFICACION, "SJF") == 0) {
            // SJF: Ordenamos la lista por el tamaño de memoria solicitado (más chico primero)
            list_sort(lista_new, comparar_tamanio); // comparador por tamaño
            pcb = list_get(lista_new, 0);
        } 
        if (pcb != NULL) {
            int hay_pcb = 1;
            while (hay_pcb){
                // Enviamos a memoria para inicializar las estructuras del proceso
                t_buffer* buffer = new_buffer();  
                add_int_to_buffer(buffer, pcb->pid);
                add_string_to_buffer(buffer, pcb->path);                              //TODO preguntar si es necesario agregar el path como atributa  de la pcb
                add_int_to_buffer(buffer, pcb->tamanio_proceso);
                t_paquete* un_paquete = crear_paquete(INICIALIZAR_ESTRUCTURAS_KM, buffer);

                //conexion_memoria(un_paquete);  // Envia el paquete a la memoria
                enviar_paquete(un_paquete, socket_memoria);

                log_info(kernel_logger, "Se aviso a Memoria para la creacion del proceso");

                // Espera hasta que la memoria haya inicializado las estructuras
                sem_wait(&sem_rpta_estructura_inicializada);

                pthread_mutex_lock(&mutex_flag_pedido_memoria);  // Bloqueamos la estructura de la memoria
                if (flag_pedido_de_memoria) {  // Si la memoria respondió correctamente
                    // Removemos el PCB de la lista NEW y lo agregamos a READY
                    list_remove_element(lista_new, pcb);

                    log_info(kernel_logger, "Creacion de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", pcb->pid);
                    agregar_pcb_lista(pcb, lista_ready, mutex_lista_ready);
                    cambiar_estado(pcb, READY_PROCCES);  // Cambiamos su estado a READY
                    log_info(kernel_logger, "Creacion de Proceso: ## (%d:0) Se cambia el estado a READY", pcb->pid);
                    // Si no hay más procesos en NEW, salimos del ciclo
                        if (list_is_empty(lista_new)) {
                            hay_pcb = 0;
                        }
                        } else {
                            // Si la memoria no respondió correctamente, terminamos el ciclo
                            hay_pcb = 0;
                        }
                         pthread_mutex_unlock(&mutex_flag_pedido_memoria);
            }
        }    
    }    pthread_mutex_unlock(&mutex_lista_new);
}
*/
void planificadorLargoPlazo() {
    pcb_t* pcb = NULL;

    pthread_mutex_lock(&mutex_lista_new);  // Bloqueamos el acceso a la lista de NEW

    log_info(kernel_logger,"Lista de NEW vacía: %d", list_is_empty(lista_new));
    
    if (!list_is_empty(lista_new)) {

        pcb = list_get(lista_new, 0);

        if (pcb != NULL) {
            int hay_pcb = 1;
            while (hay_pcb) {
                // Enviamos un mensaje a memoria para que inicialice las estructuras del proceso
                t_buffer* a_enviar = new_buffer();
                add_int_to_buffer(a_enviar, pcb->pid);
                //add_string_to_buffer(a_enviar, pcb->path);
                add_int_to_buffer(a_enviar, pcb->tamanio_proceso);
                t_paquete* un_paquete = crear_paquete(INICIALIZAR_ESTRUCTURAS_KM, a_enviar);
                enviar_paquete(un_paquete, socket_memoria);
                log_info(kernel_logger, "Se avisó a Memoria del nuevo proceso");

                // Esperamos hasta que la estructura sea creada
                sem_wait(&sem_rpta_estructura_inicializada);

                pthread_mutex_lock(&mutex_flag_pedido_memoria);
                
                if (flag_pedido_de_memoria) {
                    // Si la memoria respondió correctamente, removemos el PCB de la lista NEW
                    list_remove_element(lista_new, pcb);

                    log_info(kernel_logger, "Creación de Proceso: ## (%d) Se crea el proceso - Estado: NEW", pcb->pid);
                    agregar_pcb_lista(pcb, lista_ready, mutex_lista_ready);
                    cambiar_estado(pcb, READY_PROCCES);

                    // Si no hay más procesos en NEW, salimos del ciclo
                    if (list_is_empty(lista_new)) {
                        hay_pcb = 0;
                    }

                } else {
                    // Si la memoria no respondió correctamente, terminamos el ciclo
                    hay_pcb = 0;
                }

                pthread_mutex_unlock(&mutex_flag_pedido_memoria);
            }
        }
    }               //TODO: Hace falta volver a verificar si queda algun proceso en NEW?
}

int comparar_pcb_por_tamanio(const void* a, const void* b) {
    pcb_t* pcb_a = (pcb_t*)a;
    pcb_t* pcb_b = (pcb_t*)b;
    return pcb_a->tamanio_proceso - pcb_b->tamanio_proceso;
}