#include "pcb.h"

sem_t sem_rpta_dump_memory;

t_pcb *crear_pcb(char *ruta, int tam_proceso)
{
    t_pcb *nueva_pcb = malloc(sizeof(t_pcb));

    static int pid_counter = 0;
    nueva_pcb->pid = pid_counter++;
    // nueva_pcb->nombre_archivo = ruta;
    nueva_pcb->pc = 0;
    nueva_pcb->tamanio_proceso = tam_proceso;
    nueva_pcb->estado = NEW_PROCCES;
    nueva_pcb->tiempo_inicio_estado = time(NULL);

    // Inicializar métricas, son 7 estados y 7 tiempos

    for (int i = 0; i < CANT_ESTADOS; i++)
    {
        nueva_pcb->metricas_estado[i] = 0;
        nueva_pcb->metricas_tiempo[i] = 0.0;
    }
    nueva_pcb->metricas_estado[NEW_PROCCES] = 1;

    return nueva_pcb;
}

void agregar_pcb_lista(t_pcb *pcb, t_list *lista_estado, pthread_mutex_t mutex_lista)
{
    pthread_mutex_lock(&mutex_lista);
    list_add(lista_estado, pcb);
    pthread_mutex_unlock(&mutex_lista);
}

void cambiar_estado(t_pcb *un_pcb, estado_pcb proximo_estado)
{
    time_t ahora = time(NULL);
    double tiempo_en_estado = difftime(ahora, un_pcb->tiempo_inicio_estado);
    un_pcb->metricas_tiempo[un_pcb->estado] += tiempo_en_estado;
    log_info(kernel_logger, "## (%d) Pasa del estado %s al estado %s", un_pcb->pid, estado_a_string(un_pcb->estado), estado_a_string(proximo_estado));
    un_pcb->estado = proximo_estado;
    un_pcb->tiempo_inicio_estado = ahora;
    un_pcb->metricas_estado[proximo_estado]++;
}

const char *estado_a_string(estado_pcb estado)
{
    switch (estado)
    {
    case NEW_PROCCES:
        return "NEW_PROCCES";
    case READY_PROCCES:
        return "READY_PROCCES";
    case EXEC_PROCCES:
        return "EXEC_PROCCES";
    case BLOCKED_PROCCES:
        return "BLOCKED_PROCCES";
    case EXIT_PROCCES:
        return "EXIT_PROCCES";
    case SUSP_BLOCKED_PROCESS:
        return "SUSP_BLOCKED_PROCESS";
    case SUSP_READY_PROCESS:
        return "SUSP_READY_PROCESS";
    default:
        return "ESTADO_DESCONOCIDO";
    }
}

int pid_a_buscar;

bool buscar_pcb(void *elemento)
{
    t_pcb *pcb = (t_pcb *)elemento;  // Hacemos el cast de void* a t_pcb*
    return pcb->pid == pid_a_buscar; // Comparamos el PID
}

t_pcb *buscar_y_remover_pcb_por_pid(int un_pid)
{
    t_pcb *un_pcb;
    int elemento_encontrado = 0;

    pid_a_buscar = un_pid;
    // NEW
    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_new);
        if (list_any_satisfy(lista_new, buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_new, buscar_pcb);
            list_remove_element(lista_new, un_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_new);
    }
    // READY
    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_ready);
        if (list_any_satisfy(lista_ready, buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_ready, buscar_pcb);
            list_remove_element(lista_ready, un_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_ready);
    }

    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_exit);
        if (list_any_satisfy(lista_exit, buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_exit, buscar_pcb);
            list_remove_element(lista_exit, un_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_exit);
    }
    // BLOCKED
    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_blocked);
        if (list_any_satisfy(lista_blocked, buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_blocked, buscar_pcb);
            list_remove_element(lista_blocked, un_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_blocked);
    }
    // SUSP_READY
    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_susp_ready);
        if (list_any_satisfy(lista_susp_ready, buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_susp_ready, buscar_pcb);
            list_remove_element(lista_susp_ready, un_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_susp_ready);
    }
    // SUSP_BLOCKED
    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_susp_blocked);
        if (list_any_satisfy(lista_susp_blocked, buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_susp_blocked, buscar_pcb);
            list_remove_element(lista_susp_blocked, un_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_susp_blocked);
    }
    if (elemento_encontrado == 0)
    {
        // Si es que no se encontro en ninguna lista
        un_pcb = NULL;
        log_error(kernel_logger, "PID no encontrada en ninguna lista");
    }

    return un_pcb;
}
bool cpu_esta_libre(void *cpu_void)
{
    t_modulo_cpu *cpu = (t_modulo_cpu *)cpu_void;
    
    return cpu->libre;
}

void enviar_pcb_a_cpu(t_pcb *un_pcb)
{

    if (un_pcb == NULL)
    {
        log_error(kernel_logger, "PCB es NULL, no se puede enviar a CPU");
        return;
    }
    pthread_mutex_lock(&mutex_lista_modulos_cpu_conectadas);

    t_modulo_cpu* un_cpu = list_find(lista_modulos_cpu_conectadas, cpu_esta_libre);

    
    if (un_cpu != NULL)
    {

        un_cpu->libre = false;
        un_cpu->proceso_en_ejecucion = un_pcb;
        pthread_mutex_unlock(&mutex_lista_modulos_cpu_conectadas);

        // Crear y enviar el paquete
        t_buffer *un_buffer = new_buffer();
        add_int_to_buffer(un_buffer, un_pcb->pid);
        add_int_to_buffer(un_buffer, un_pcb->pc);

        t_paquete *un_paquete = crear_paquete(EJECUTAR_PROCESO_KC, un_buffer);

        enviar_paquete(un_paquete, un_cpu->socket_fd_dispatch);
        eliminar_paquete(un_paquete);
    }
    else
    {
        pthread_mutex_unlock(&mutex_lista_modulos_cpu_conectadas);
        log_warning(kernel_logger, "No hay CPUs libres. Proceso %d queda en cola READY", un_pcb->pid);

        // El proceso debería volver a la cola READY o mantenerse ahí
        // Esto depende de tu implementación del planificador
    }
}

void bloquear_proceso_syscall(int pid)
{

    t_pcb *un_pcb = buscar_y_remover_pcb_por_pid(pid);
    cambiar_estado(un_pcb, BLOCKED_PROCCES);
    agregar_pcb_lista(un_pcb, lista_blocked, mutex_lista_blocked);

    // TODO:MOTIVO?
}

t_pcb *buscar_pcb_por_socket(int socket_cpu)
{
    for (int i = 0; i < list_size(lista_modulos_cpu_conectadas); i++)
    {
        t_modulo_cpu *cpu = list_get(lista_modulos_cpu_conectadas, i);
        if (cpu->socket_fd_dispatch == socket_cpu)
        {
            return cpu->proceso_en_ejecucion;
        }
    }
    return NULL;
}

void enviar_pcb_a_modulo_io(t_modulo_io *modulo, t_pcb *pcb, int tiempo_ms)
{
    t_buffer *buffer = new_buffer();
    add_int_to_buffer(buffer, pcb->pid);
    add_int_to_buffer(buffer, tiempo_ms);
    t_paquete *paquete = crear_paquete(REALIZAR_IO, buffer);
    log_info(kernel_logger, "Socket de IO teclado %d", modulo->socket_fd);
    enviar_paquete(paquete, modulo->socket_fd);
    eliminar_paquete(paquete);
    
}

int un_pid_a_buscar;

bool __buscar_pcb(void *void_pcb)
{   
    t_pcb *pcb = (t_pcb *)void_pcb;
    if (pcb->pid == un_pid_a_buscar)
    {
        return true;
    }
    else
    {
        return false;
    }
}

t_pcb *buscar_pcb_por_pid(int un_pid)
{
    t_pcb *un_pcb = NULL;
    int elemento_encontrado = 0;

    un_pid_a_buscar = un_pid;

    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_new);
        if (list_any_satisfy(lista_new, __buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_new, __buscar_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_new);
    }
    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_ready);
        //	int cantidad = list_size(lista_ready_thread);
        //	log_info(kernel_logger, "CANTIDAD DE ENTRADAS EN LA LISTA READY: %d", cantidad);
        if (list_any_satisfy(lista_ready, __buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_ready, __buscar_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_ready);
    }
    if (elemento_encontrado == 0)
    {
        log_info(kernel_logger, "Buscando PID %d en lista EXECUTE", un_pid);
        pthread_mutex_lock(&mutex_lista_execute);
        //	int cantidad = list_size(lista_execute_thread);
        //	t_tcb* pepe = list_get(lista_execute_thread, 0);
        //	log_info(kernel_logger, "CANTIDAD DE ENTRADAS EN LA LISTA EXECUTE: %d", cantidad);
        //	log_info(kernel_logger, "TID DE LA LISTA: %d", pepe->tid);
        if (list_any_satisfy(lista_execute, __buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_execute, __buscar_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_execute);
    }
    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_exit);
        if (list_any_satisfy(lista_exit, __buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_exit, __buscar_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_exit);
    }
    if (elemento_encontrado == 0)
    {
        pthread_mutex_lock(&mutex_lista_blocked);
        if (list_any_satisfy(lista_blocked, __buscar_pcb))
        {
            elemento_encontrado = 1;
            un_pcb = list_find(lista_blocked, __buscar_pcb);
        }
        pthread_mutex_unlock(&mutex_lista_blocked);
    }
    if (elemento_encontrado == 0)
    {
        // Si es que no se encontro en ninguna lista
        log_error(kernel_logger, "[PID:%d] no encontrada en ninguna lista", un_pid);
        un_pcb = NULL;
    }

    return un_pcb;
}

void remover_pcb_lista(t_pcb *pcb, t_list *lista, pthread_mutex_t *mutex)
{
    if (pcb == NULL || lista == NULL)
    {
        log_error(kernel_logger, "PCB o lista es NULL en remover_pcb_lista");
        return;
    }

    pthread_mutex_lock(mutex);

    // Buscar y remover el PCB de la lista
    bool encontrado = false;
    for (int i = 0; i < list_size(lista); i++)
    {
        t_pcb *pcb_actual = list_get(lista, i);
        if (pcb_actual->pid == pcb->pid)
        {
            list_remove(lista, i);
            encontrado = true;
            log_debug(kernel_logger, "PCB con PID %d removido de la lista", pcb->pid);
            break;
        }
    }

    if (!encontrado)
    {
        log_warning(kernel_logger, "PCB con PID %d no encontrado en la lista para remover", pcb->pid);
    }

    pthread_mutex_unlock(mutex);
}