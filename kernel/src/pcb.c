#include "pcb.h"
#include <kernel_cpu.h> 

sem_t sem_rpta_dump_memory;

t_pcb *crear_pcb(char *ruta, int tam_proceso)
{
    t_pcb *nueva_pcb = malloc(sizeof(t_pcb));

    static int pid_counter = 0;
    nueva_pcb->pid = pid_counter++;
    // nueva_pcb->nombre_archivo = ruta; strdup(ruta); 
    nueva_pcb->pc = 0;
    nueva_pcb->tamanio_proceso = tam_proceso;
    nueva_pcb->estado = NEW_PROCCES;
    nueva_pcb->tiempo_inicio_estado = time(NULL);
    nueva_pcb->tiempo_estimacion = ESTIMACION_INICIAL;
    nueva_pcb->tiempo_exec_ultima_actualizacion = 0;

    // Inicializar métricas, son 7 estados y 7 tiempos

    for (int i = 0; i < CANT_ESTADOS; i++)
    {
        nueva_pcb->metricas_estado[i] = 0;
        nueva_pcb->metricas_tiempo[i] = 0.0;
    }
    nueva_pcb->metricas_estado[NEW_PROCCES] = 1;

    return nueva_pcb;
}

void agregar_pcb_lista(t_pcb *pcb, t_list *lista_estado, pthread_mutex_t *mutex_lista)
{
    pthread_mutex_lock(mutex_lista);
    list_add(lista_estado, pcb);
    pthread_mutex_unlock(mutex_lista);
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

int  pid_a_buscar;

bool buscar_pcb(void *elemento) {
    t_pcb *pcb = (t_pcb *)elemento;
    return pcb->pid == pid_a_buscar;
}

t_pcb *buscar_y_remover_en_lista(t_list *lista, pthread_mutex_t *mutex, char *nombre_lista, int pid) {
    pthread_mutex_lock(mutex);
    
    pid_a_buscar = pid;  
    
    if (list_any_satisfy(lista, buscar_pcb)) {
        t_pcb *pcb_encontrado = list_find(lista, buscar_pcb);
        list_remove_element(lista, pcb_encontrado);
        pthread_mutex_unlock(mutex);
        
        log_info(kernel_logger, "PCB con PID %d encontrado y removido de lista %s", pid, nombre_lista);
        return pcb_encontrado;
    }
    
    pthread_mutex_unlock(mutex);
    return NULL;
}

t_pcb *buscar_y_remover_pcb_por_pid(int un_pid) {
    t_pcb *un_pcb = NULL;
    log_info(kernel_logger, "DEBUG: Iniciando búsqueda y remoción de PID %d", un_pid);
    
    // Buscar en orden de probabilidad (ajusta según tu lógica)
    // 1. BLOCKED (más probable para I/O)
    un_pcb = buscar_y_remover_en_lista(lista_blocked, &mutex_lista_blocked, "BLOCKED", un_pid);
    if (un_pcb) {
        log_info(kernel_logger, "Saliendo de 'buscar_y_remover_pcb_por_pid' con la PCB: %d", un_pcb->pid);
        return un_pcb;
    }

    // 2. READY 
    un_pcb = buscar_y_remover_en_lista(lista_ready, &mutex_lista_ready, "READY", un_pid);
    if (un_pcb) {
        log_info(kernel_logger, "Saliendo de 'buscar_y_remover_pcb_por_pid' con la PCB: %d", un_pcb->pid);
        return un_pcb;
    }
    
    // 3. NEW
    un_pcb = buscar_y_remover_en_lista(lista_new, &mutex_lista_new, "NEW", un_pid);
    if (un_pcb) {
        log_info(kernel_logger, "Saliendo de 'buscar_y_remover_pcb_por_pid' con la PCB: %d", un_pcb->pid);
        return un_pcb;
    }
    
    // 4. SUSP_READY
    un_pcb = buscar_y_remover_en_lista(lista_susp_ready, &mutex_lista_susp_ready, "SUSP_READY", un_pid);
    if (un_pcb) {
        log_info(kernel_logger, "Saliendo de 'buscar_y_remover_pcb_por_pid' con la PCB: %d", un_pcb->pid);
        return un_pcb;
    }
    
    // 5. SUSP_BLOCKED
    un_pcb = buscar_y_remover_en_lista(lista_susp_blocked, &mutex_lista_susp_blocked, "SUSP_BLOCKED", un_pid);
    if (un_pcb) {
        log_info(kernel_logger, "Saliendo de 'buscar_y_remover_pcb_por_pid' con la PCB: %d", un_pcb->pid);
        return un_pcb;
    }
    
    // 6. EXIT (última opción)
    un_pcb = buscar_y_remover_en_lista(lista_exit, &mutex_lista_exit, "EXIT", un_pid);
    if (un_pcb) {
        log_info(kernel_logger, "Saliendo de 'buscar_y_remover_pcb_por_pid' con la PCB: %d", un_pcb->pid);
        return un_pcb;
    }
    
    // Si no se encontró en ninguna lista
    log_error(kernel_logger, "[PID:%d] no encontrado en ninguna lista", un_pid);
    return NULL;
}

bool cpu_esta_libre(void *cpu_void)
{
    t_modulo_cpu *cpu = (t_modulo_cpu *)cpu_void;
    
    return cpu->libre;
}

t_modulo_cpu* enviar_pcb_a_cpu(t_pcb *un_pcb)
{
    pthread_mutex_lock(&mutex_lista_modulos_cpu_conectadas);

    t_modulo_cpu* un_cpu = list_find(lista_modulos_cpu_conectadas, cpu_esta_libre);

    if (un_cpu)
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
        un_cpu->libre = false;
        un_cpu->proceso_en_ejecucion = un_pcb;

        eliminar_paquete(un_paquete);
    }else
    {
        pthread_mutex_unlock(&mutex_lista_modulos_cpu_conectadas);
        log_error(kernel_logger, "No hay CPU's libres para enviar el PCB con PID %d", un_pcb->pid);
    }
    return un_cpu;
}

void bloquear_proceso_syscall(int pid)
{

    t_pcb *un_pcb = buscar_y_remover_pcb_por_pid(pid);
    cambiar_estado(un_pcb, BLOCKED_PROCCES);
    agregar_pcb_lista(un_pcb, lista_blocked, &mutex_lista_blocked);

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
    log_info(kernel_logger, "Paquete creado con REALIZAR_IO para PID %d", pcb->pid);
    enviar_paquete(paquete, modulo->socket_fd);
    log_info(kernel_logger,"Enviee el paquete a io");
    eliminar_paquete(paquete);
    log_info(kernel_logger, "Paquete enviado exitosamente al módulo IO");
    
}

t_pcb* buscar_en_lista(t_list *lista, pthread_mutex_t *mutex, const char *nombre_lista, int pid_buscado) {
    t_pcb *pcb_encontrado = NULL;
    
    log_info(kernel_logger, "DEBUG: Buscando en lista %s", nombre_lista);
    pthread_mutex_lock(mutex);
    log_info(kernel_logger, "DEBUG: Mutex %s obtenido", nombre_lista);
    
    for (int i = 0; i < list_size(lista); i++) {
        t_pcb *pcb = list_get(lista, i);
        if (pcb && pcb->pid == pid_buscado) {
            pcb_encontrado = pcb;
            log_info(kernel_logger, "DEBUG: PID %d encontrado en lista %s", pid_buscado, nombre_lista);
            break;
        }
    }
    
    pthread_mutex_unlock(mutex);
    
    if (!pcb_encontrado) {
        log_info(kernel_logger, "DEBUG: PID %d no encontrado en lista %s", pid_buscado, nombre_lista);
    }
    
    return pcb_encontrado;
}

t_pcb *buscar_pcb_por_pid(int un_pid) {
    t_pcb *un_pcb = NULL;
    
    log_info(kernel_logger, "DEBUG: Iniciando búsqueda de PID %d", un_pid);
    
    // Buscar en orden de probabilidad
    // 1. EXECUTE (más probable para procesos que vienen de CPU)
    un_pcb = buscar_en_lista(lista_execute, &mutex_lista_execute, "EXECUTE", un_pid);
    if (un_pcb) return un_pcb;
    
    // 2. BLOCKED (segunda opción más probable)
    un_pcb = buscar_en_lista(lista_blocked, &mutex_lista_blocked, "BLOCKED", un_pid);
    if (un_pcb) return un_pcb;
    
    // 3. READY
    un_pcb = buscar_en_lista(lista_ready, &mutex_lista_ready, "READY", un_pid);
    if (un_pcb) return un_pcb;
    
    // 4. NEW (menos probable)
    un_pcb = buscar_en_lista(lista_new, &mutex_lista_new, "NEW", un_pid);
    if (un_pcb) return un_pcb;
    
    // 5. EXIT (última opción)
    un_pcb = buscar_en_lista(lista_exit, &mutex_lista_exit, "EXIT", un_pid);
    if (un_pcb) return un_pcb;
    
    // Si no se encontró en ninguna lista
    log_error(kernel_logger, "[PID:%d] no encontrado en ninguna lista", un_pid);
    return NULL;
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
