#include "planificadorLargoPlazo.h"

t_list *lista_new;
t_list *lista_ready;

sem_t sem_rpta_estructura_inicializada;
sem_t semaforo_largo_plazo;
sem_t sem_estructura_liberada;

pthread_mutex_t mutex_lista_new = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_ready = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_exit = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_blocked = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_susp_blocked = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_susp_ready = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_exec = PTHREAD_MUTEX_INITIALIZER;

bool flag_pedido_de_memoria;
pthread_mutex_t mutex_flag_pedido_memoria;

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
        if(ALGORITMO_INGRESO_A_READY == FIFO){
            pcb = list_get(lista_new, 0); // sale por fifo
        }else{ //Proceso mas chico primero
            list_sort(lista_new, (void*)comparar_pcb_por_tamanio);
            pcb = list_get(lista_new, 0); // sale el de menor tamaño
        }
       
        if (pcb != NULL)
        {
            int hay_pcb = 1;
            while (hay_pcb)
            {
                // Enviamos un mensaje a memoria para que inicialice las estructuras del proceso
                t_buffer *a_enviar = new_buffer();
                add_int_to_buffer(a_enviar, pcb->pid);
                // add_string_to_buffer(a_enviar, pcb->path);
                add_int_to_buffer(a_enviar, pcb->tamanio_proceso);
                t_paquete *un_paquete = crear_paquete(INICIALIZAR_ESTRUCTURAS_KM, a_enviar);
                socket_memoria = crear_conexion(kernel_logger, "Memoria Server", IP_MEMORIA, PUERTO_MEMORIA);
                enviar_paquete(un_paquete, socket_memoria);
                atender_kernel_memoria();
                eliminar_paquete(un_paquete);
                liberar_conexion(socket_memoria);
                log_info(kernel_logger, "Se avisó a Memoria del nuevo proceso");

                // Esperamos hasta que la estructura sea creada
                sem_wait(&sem_rpta_estructura_inicializada);

                pthread_mutex_lock(&mutex_flag_pedido_memoria);

                if (flag_pedido_de_memoria)
                {
                    // Si la memoria respondió correctamente, removemos el PCB de la lista NEW
                    list_remove_element(lista_new, pcb);
                    agregar_pcb_lista(pcb, lista_ready, mutex_lista_ready);
                    cambiar_estado(pcb, READY_PROCCES);
                    // planificadorCortoPlazo();
                    if (list_is_empty(lista_new))
                    {
                        hay_pcb = 0;
                    }
                }
                else
                {
                    // Si la memoria no respondió correctamente, terminamos el ciclo
                    log_info(kernel_logger, "Memoria no tiene espacio para PID %d, se mantiene en NEW", pcb->pid);
                    // Se debe esperar la finalizacion de un proceso para volver a intentar?

                    hay_pcb = 0;
                }

                pthread_mutex_unlock(&mutex_flag_pedido_memoria);
            }
        }
    }
    pthread_mutex_unlock(&mutex_lista_new);

  
   //deberia volver a iniciar el plp?para probar con el siguiente proceso?

}

void finalizar_proceso(int pid)
{
    t_pcb *un_pcb = buscar_y_remover_pcb_por_pid(pid);
    if (un_pcb != NULL)
    {
                                  
        t_buffer *a_enviar = new_buffer();
        add_int_to_buffer(a_enviar, un_pcb->pid);
        log_info(kernel_logger, "AGREGO AL BUFFER EL PID A ELIMINAR: %d", un_pcb->pid);
        t_paquete *un_paquete = crear_paquete(FINALIZAR_ESTRUCTURAS_KM, a_enviar);

        enviar_paquete(un_paquete, socket_memoria);
        atender_kernel_memoria();
        eliminar_paquete(un_paquete);
        liberar_conexion(socket_memoria);

        log_info(kernel_logger, "Se aviso a Memoria para que libere las estructuras del proceso");
        sem_wait(&sem_estructura_liberada);

        agregar_pcb_lista(un_pcb, lista_exit, mutex_lista_exit);

        log_info(kernel_logger, "Fin de Proceso: ## Finaliza el proceso %d", un_pcb->pid);

        // Verificamos si hay procesos en SUSP_READY
        pthread_mutex_lock(&mutex_lista_susp_ready);

        bool hay_espacio_disponible = true;

        while (!list_is_empty(lista_susp_ready) && hay_espacio_disponible)
        {
            // Reactivar proceso de SUSP_READY -> READY
    
            t_pcb *pcb_a_reactivar = list_get(lista_susp_ready, 0);
            t_buffer *a_enviar = new_buffer();
            t_paquete *un_paquete = crear_paquete(INICIALIZAR_ESTRUCTURAS_KM, a_enviar);
            add_int_to_buffer(a_enviar, pcb_a_reactivar->pid);
            add_int_to_buffer(a_enviar, pcb_a_reactivar->tamanio_proceso);

            socket_memoria = crear_conexion(kernel_logger, "Memoria Server", IP_MEMORIA, PUERTO_MEMORIA);
            enviar_paquete(un_paquete, socket_memoria);
            atender_kernel_memoria();
            eliminar_paquete(un_paquete);
            liberar_conexion(socket_memoria);

            sem_wait(&sem_rpta_estructura_inicializada);

            pthread_mutex_lock(&mutex_flag_pedido_memoria);

            if (flag_pedido_de_memoria)
            {
                // Si la memoria respondió correctamente, removemos el PCB de la lista NEW
                list_remove_element(lista_susp_ready, pcb_a_reactivar);
                agregar_pcb_lista(pcb_a_reactivar, lista_ready, mutex_lista_ready);
                cambiar_estado(pcb_a_reactivar, READY_PROCCES);
            }
            else
            {
                // Si la memoria no respondió correctamente, terminamos el ciclo
                log_info(kernel_logger, "Memoria no tiene espacio para PID %d, se mantiene en SUSP_READY", pcb_a_reactivar->pid);
                hay_espacio_disponible = false;
            }

            pthread_mutex_unlock(&mutex_flag_pedido_memoria);
        }
        pthread_mutex_unlock(&mutex_lista_susp_ready);
    }

    log_info(kernel_logger, "Métricas de Estado: ## (%d) - Métricas de estado: NEW (%d) (%f), READY (%d) (%f),EXEC (%d) (%f), BLOCKED (%d) (%f), SUSP_READY (%d) (%f), SUSP_BLOCKED (%d) (%f),EXIT (%d) (%f),",
         un_pcb->pid, un_pcb->metricas_estado[NEW_PROCCES],un_pcb->metricas_tiempo[NEW_PROCCES],
         un_pcb->metricas_estado[READY_PROCCES], un_pcb->metricas_tiempo[READY_PROCCES],
         un_pcb->metricas_estado[EXEC_PROCCES], un_pcb->metricas_tiempo[EXEC_PROCCES],
         un_pcb->metricas_estado[BLOCKED_PROCCES], un_pcb->metricas_tiempo[BLOCKED_PROCCES],
         un_pcb->metricas_estado[SUSP_READY_PROCESS], un_pcb->metricas_tiempo[SUSP_READY_PROCESS],
         un_pcb->metricas_estado[SUSP_BLOCKED_PROCESS], un_pcb->metricas_tiempo[SUSP_BLOCKED_PROCESS],
         un_pcb->metricas_estado[EXIT_PROCCES], un_pcb->metricas_tiempo[EXIT_PROCCES]);
    // Si no hay procesos en SUSP_READY, se intenta NEW
    planificadorLargoPlazo();

}

int comparar_pcb_por_tamanio(const void* a, const void* b) {
    t_pcb* pcb_a = (t_pcb*)a;
    t_pcb* pcb_b = (t_pcb*)b;
    return pcb_a->tamanio_proceso - pcb_b->tamanio_proceso;
}
