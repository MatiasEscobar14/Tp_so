#include "planificadorLargoPlazo.h"
#include "planificadorCortoPlazo.h"

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
pthread_mutex_t mutex_lista_execute = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_modulos_cpu_conectadas= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lista_modulos_io_conectadas = PTHREAD_MUTEX_INITIALIZER;
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

    if (!list_is_empty(lista_new))
    {
        if(ALGORITMO_INGRESO_A_READY == FIFO){
            planificadorLargoPlazoFifo();
        }else{ //Proceso mas chico primero
            planificadorLargoPlazoPMCP();
        }
    }
    
    
}


void planificadorLargoPlazoFifo()
{
    t_pcb *pcb = list_get(lista_new, 0); 
    
    if (pcb != NULL)
    {
        int hay_pcb = 1;
        while (hay_pcb)
        {

            t_buffer *a_enviar = new_buffer();
            add_int_to_buffer(a_enviar, pcb->pid);
            add_int_to_buffer(a_enviar, pcb->tamanio_proceso);
            t_paquete *un_paquete = crear_paquete(INICIALIZAR_ESTRUCTURAS_KM, a_enviar);

            socket_memoria = crear_conexion(kernel_logger, "Memoria Server", IP_MEMORIA, PUERTO_MEMORIA);
            enviar_paquete(un_paquete, socket_memoria);
            atender_kernel_memoria();
            eliminar_paquete(un_paquete);
            liberar_conexion(socket_memoria);

            log_info(kernel_logger, "Se avisó a Memoria del nuevo proceso");
            

            sem_wait(&sem_rpta_estructura_inicializada);
            //pthread_mutex_lock(&mutex_flag_pedido_memoria);
            
            if (flag_pedido_de_memoria)
            {
                remover_pcb_lista(pcb, lista_new, &mutex_lista_new);
                agregar_pcb_lista(pcb, lista_ready, &mutex_lista_ready);
                cambiar_estado(pcb, READY_PROCCES);
                planificadorCortoPlazo();
                
                if (list_is_empty(lista_new))
                {
                    hay_pcb = 0;
                } else {
                    pcb = list_get(lista_new, 0); 
                }
            }
            else
            {

                log_info(kernel_logger, "Memoria no tiene espacio para PID %d, se mantiene en NEW", pcb->pid);
                // Se debe esperar la finalizacion de un proceso para volver a intentar? 
                hay_pcb = 0;
            }
            //pthread_mutex_unlock(&mutex_flag_pedido_memoria);
        }
    }
}

void planificadorLargoPlazoPMCP()
{

    list_sort(lista_new, (void*)comparar_pcb_por_tamanio);
    
    /* Intentamos inicializar procesos en orden de tamaño hasta que no se pueda más */
    bool se_inicio_alguno = true;
    
    while (se_inicio_alguno && !list_is_empty(lista_new))
    {
        se_inicio_alguno = false;
        
        /* Reordenar la lista después de cada cambio para mantener el orden */
        list_sort(lista_new, (void*)comparar_pcb_por_tamanio);
        
        /* Intentar con el proceso más pequeño actual */
        t_pcb *pcb = list_get(lista_new, 0); /* sale el de menor tamaño */
        
        if (pcb != NULL)
        {
            /* Enviamos un mensaje a memoria para que inicialice las estructuras del proceso */
            t_buffer *a_enviar = new_buffer();
            add_int_to_buffer(a_enviar, pcb->pid);
            add_int_to_buffer(a_enviar, pcb->tamanio_proceso);
            t_paquete *un_paquete = crear_paquete(INICIALIZAR_ESTRUCTURAS_KM, a_enviar);
            
            socket_memoria = crear_conexion(kernel_logger, "Memoria Server", IP_MEMORIA, PUERTO_MEMORIA);
            enviar_paquete(un_paquete, socket_memoria);
            atender_kernel_memoria();
            eliminar_paquete(un_paquete);
            liberar_conexion(socket_memoria);
            log_info(kernel_logger, "PMCP: Se avisó a Memoria del proceso PID %d (tamaño: %d)", pcb->pid, pcb->tamanio_proceso);
            
            /* Esperamos hasta que la estructura sea creada */
            sem_wait(&sem_rpta_estructura_inicializada);
            //pthread_mutex_lock(&mutex_flag_pedido_memoria);
            log_info(kernel_logger, "Flag pedido de memoria antes de IF: %d", flag_pedido_de_memoria);
            if (flag_pedido_de_memoria)
            {
                /* Si la memoria respondió correctamente, removemos el PCB de la lista NEW */
                remover_pcb_lista(pcb, lista_new, &mutex_lista_new);
                log_info(kernel_logger, "Removi correctamente la pcb: %d de la lista NEW", pcb->pid);
                agregar_pcb_lista(pcb, lista_ready, &mutex_lista_ready);
                log_info(kernel_logger, "Agregue correctamente la pcb: %d de la lista READY", pcb->pid);
                cambiar_estado(pcb, READY_PROCCES);
                log_info(kernel_logger, "PMCP: Proceso PID %d iniciado exitosamente (tamaño: %d)", pcb->pid, pcb->tamanio_proceso);
                planificadorCortoPlazo();
                se_inicio_alguno = true; /* Seguimos intentando con otros procesos */
            }
            else
            {
                /* Si la memoria no respondió correctamente, terminamos el ciclo */
                log_info(kernel_logger, "PMCP: Memoria no tiene espacio para PID %d (tamaño: %d), terminando intentos", pcb->pid, pcb->tamanio_proceso);
                //TODO se debera esperar a que se libere espacio en memoria para continuar?
                se_inicio_alguno = false;
            }
            //pthread_mutex_unlock(&mutex_flag_pedido_memoria);
        }
    }
    
    log_info(kernel_logger, "TERMINO EL PLANIFICADOR PMCP");
}


void finalizar_proceso(int pid)
{
    t_pcb *un_pcb = buscar_y_remover_pcb_por_pid(pid); //TODO esta funcion funciona?
    if (un_pcb)
    {
        t_buffer *a_enviar = new_buffer();
        add_int_to_buffer(a_enviar, un_pcb->pid);
        t_paquete *un_paquete = crear_paquete(FINALIZAR_ESTRUCTURAS_KM, a_enviar);
        //TODO habria que proteger "socket_memoria" con un mutex?
        socket_memoria = crear_conexion(kernel_logger, "Memoria Server", IP_MEMORIA, PUERTO_MEMORIA);
        enviar_paquete(un_paquete, socket_memoria);
        atender_kernel_memoria();
        eliminar_paquete(un_paquete);
        liberar_conexion(socket_memoria);

        log_info(kernel_logger, "Se aviso a Memoria para que libere las estructuras del proceso");
        sem_wait(&sem_estructura_liberada);

        agregar_pcb_lista(un_pcb, lista_exit, &mutex_lista_exit);

        log_info(kernel_logger, "Fin de Proceso: ## Finaliza el proceso %d", un_pcb->pid);

        // Verificamos si hay procesos en SUSP_READY
        pthread_mutex_lock(&mutex_lista_susp_ready);

        bool hay_espacio_disponible = true; //TODO: esto es un mock deberiamos cambiarlo cuando la memoria este completa

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
                
                remover_pcb_lista(pcb_a_reactivar, lista_susp_ready, &mutex_lista_susp_ready);
                agregar_pcb_lista(pcb_a_reactivar, lista_ready, &mutex_lista_ready);
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
