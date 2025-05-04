#include "planificadorLargoPlazo.h"

// Variable global para gestionar la lista de procesos y la sincronizacion
t_list *lista_new;                      // Lista de procesos nuevos
t_list *lista_ready;                    // Lista de procesos listos para ejecutarse
t_list *lista_ready_thread;             // Lista de hilos listos
sem_t sem_rpta_estructura_inicializada; // Semaforo para sincronizar la respuesta de memoria
sem_t sem_estructura_liberada;
pthread_mutex_t mutex_lista_new = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_ready = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_ready_thread = PTHREAD_MUTEX_INITIALIZER;
bool flag_pedido_de_memoria;
pthread_mutex_t mutex_flag_pedido_memoria;

// Definir el semáforo global
sem_t semaforo_largo_plazo; // Semáforo que controla la planificación

// Función para esperar a que el usuario presione Enter
void *esperar_enter(void *arg)
{
    printf("Presione Enter para iniciar el Planificador de Largo Plazo...\n");
    getchar();
    printf("Planificador de Largo Plazo iniciado.\n");

    sem_post(&semaforo_largo_plazo);
    return NULL;
}

void iniciar_plp()
{

    if (sem_init(&semaforo_largo_plazo, 0, 0) != 0)
    {
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

void planificadorLargoPlazo()
{
    t_pcb *pcb = NULL;

    pthread_mutex_lock(&mutex_lista_new); // Bloqueamos el acceso a la lista de NEW

    if (!list_is_empty(lista_new))
    {

        pcb = list_get(lista_new, 0); // sale por fifo

        if (pcb != NULL){
            int hay_pcb = 1;
            while (hay_pcb){
                // Enviamos un mensaje a memoria para que inicialice las estructuras del proceso
                t_buffer *a_enviar = new_buffer();
                add_int_to_buffer(a_enviar, pcb->pid);
                // add_string_to_buffer(a_enviar, pcb->path);
                add_int_to_buffer(a_enviar, pcb->tamanio_proceso);
                t_paquete *un_paquete = crear_paquete(INICIALIZAR_ESTRUCTURAS_KM, a_enviar);
                enviar_paquete(un_paquete, socket_memoria);
                atender_kernel_memoria();
                eliminar_paquete(un_paquete);
                liberar_conexion(socket_memoria);
                log_info(kernel_logger, "Se avisó a Memoria del nuevo proceso");

                // Esperamos hasta que la estructura sea creada
                sem_wait(&sem_rpta_estructura_inicializada);

                pthread_mutex_lock(&mutex_flag_pedido_memoria);

                if (flag_pedido_de_memoria){
                    // Si la memoria respondió correctamente, removemos el PCB de la lista NEW
                
                   printf("Primer elemento de la lista_new: %d\n", ((t_pcb *)list_get(lista_new, 0))->pid);
                    list_remove_element(lista_new, pcb);
                    log_info(kernel_logger, "Creación de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", pcb->pid);
                    agregar_pcb_lista(pcb, lista_ready, mutex_lista_ready);
                    cambiar_estado(pcb, READY_PROCCES);
                    if(list_is_empty(lista_new)){
							hay_pcb = 0;
						}

                }else{
                // Si la memoria no respondió correctamente, terminamos el ciclo
                log_info(kernel_logger, "Memoria no tiene espacio para PID %d, se mantiene en NEW", pcb->pid);
                hay_pcb = 0;
                }

                pthread_mutex_unlock(&mutex_flag_pedido_memoria);
                
            // Reintentar procesar el siguiente PCB si no hubo espacio
            // log_info(kernel_logger, "Esperando espacio en memoria para el siguiente proceso...");
            }
        }
    }
    pthread_mutex_unlock(&mutex_lista_new);
}

/*
int comparar_pcb_por_tamanio(const void* a, const void* b) {
    t_pcb* pcb_a = (t_pcb*)a;
    t_pcb* pcb_b = (t_pcb*)b;
    return pcb_a->tamanio_proceso - pcb_b->tamanio_proceso;
}
*/