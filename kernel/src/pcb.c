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
    t_cpu *cpu = (t_cpu *)cpu_void;
    return cpu->libre;
}

void enviar_pcb_a_cpu(t_pcb *un_pcb)
{

    t_modulo_cpu* un_cpu = list_find(lista_cpu_conectadas, (void *)cpu_esta_libre);

    if (un_cpu != NULL)
    {
        t_buffer *un_buffer = new_buffer();
        add_int_to_buffer(un_buffer, un_pcb->pid);
        add_int_to_buffer(un_buffer, un_pcb->pc);
        t_paquete *un_paquete = crear_paquete(EJECUTAR_PROCESO_KC, un_buffer);
        enviar_paquete(un_paquete, un_cpu->socket_d);
        eliminar_paquete(un_paquete);
    }
    else
    {
        log_error(kernel_logger, "No hay CPU's libres actualmente");

    }
}

void bloquear_proceso_syscall(int pid)
{

    t_pcb *un_pcb = buscar_y_remover_pcb_por_pid(pid);
    cambiar_estado(un_pcb, BLOCKED_PROCCES);
    agregar_pcb_lista(un_pcb, lista_blocked, mutex_lista_blocked);

    // TODO:MOTIVO?
}
