#include <planificadorCortoPlazo.h>

sem_t sem_cpu_disponible;

void planificadorCortoPlazo()
{
    while (1)
    {
        pthread_mutex_lock(&mutex_lista_ready);
        bool lista_vacia = list_is_empty(lista_ready);
        pthread_mutex_unlock(&mutex_lista_ready);

        if (!lista_vacia)
        {
            switch (ALGORITMO_CORTO_PLAZO)
            {
            case FIFO:
                atender_FIFO();
                break;

            case SJF:
                // atender_SJF(); // falta implementar
                break;

            case SJF_SD:
                // atender_SJF_SD(); // falta implementar
                break;

            default:
                log_error(kernel_logger, "ALGORITMO DE CORTO PLAZO DESCONOCIDO");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void atender_FIFO()
{

    log_info(kernel_logger, "Esperando a que se conecte una CPU...");
    sem_wait(&sem_cpu_disponible); // Espera a que haya una CPU disponible
    log_info(kernel_logger, "CPU conectada, enviando pcb a CPU.");

    log_info(kernel_logger, "Comienzo a ejecutar FIFO (planificador corto plazo)");

    pthread_mutex_lock(&mutex_lista_ready);
    t_pcb *un_pcb = NULL;

    if (!list_is_empty(lista_ready))
    {
        un_pcb = list_get(lista_ready, 0);
    }
    pthread_mutex_unlock(&mutex_lista_ready);

    if (un_pcb != NULL)
    {
        t_modulo_cpu *modulo_cpu_libre = cpu_libre();
        if (modulo_cpu_libre != NULL)
        {
            enviar_pcb_a_cpu(modulo_cpu_libre, un_pcb);
            cambiar_estado(un_pcb, EXEC_PROCCES);
            remover_pcb_lista(un_pcb, lista_ready, &mutex_lista_ready);
            agregar_pcb_lista(un_pcb, lista_execute, &mutex_lista_execute);
            atender_kernel_cpu_dispatch(&(modulo_cpu_libre->socket_fd_dispatch));// Espera PID + motivo de finalizacion/interrupcion
            // falta agregar como protocolo los motivos de finalizacion/interrupcion
        }
        else
        {
            log_warning(kernel_logger, "No hay CPU libre para ejecutar PCB %d, permanece en READY",
                un_pcb->pid);
            // El PCB queda en lista_ready para el próximo ciclo del planificador
        } 
        
    }
}

void atender_SJF()
{

    log_info(kernel_logger, "Esperando a que se conecte una CPU...");
    sem_wait(&sem_cpu_disponible); // Espera a que haya una CPU disponible
    log_info(kernel_logger, "CPU conectada, enviando pcb a CPU.");

    log_info(kernel_logger, "Comienzo a ejecutar SJF sin desalojo (planificador corto plazo)");

    pthread_mutex_lock(&mutex_lista_ready);
    t_pcb *pcb_elegido = NULL;

    if (!list_is_empty(lista_ready))
    {
        // Buscar el proceso con menor estimación de ráfaga
        pcb_elegido = encontrar_proceso_menor_estimacion();
    }

    pthread_mutex_unlock(&mutex_lista_ready);

    if (pcb_elegido != NULL)
    {
        log_info(kernel_logger, "PCB %d seleccionado con estimación: %lf",
                 pcb_elegido->pid, pcb_elegido->tiempo_estimacion);

        t_modulo_cpu *modulo_cpu_libre = cpu_libre();
        if (modulo_cpu_libre != NULL)
        {
            enviar_pcb_a_cpu(modulo_cpu_libre, pcb_elegido);
            cambiar_estado(pcb_elegido, EXEC_PROCCES);
            remover_pcb_lista(pcb_elegido, lista_ready, &mutex_lista_ready);
            agregar_pcb_lista(pcb_elegido, lista_execute, &mutex_lista_execute);
            atender_kernel_cpu_dispatch(&(modulo_cpu_libre->socket_fd_dispatch));
        }
        else
        {
            log_warning(kernel_logger, "No hay CPU libre para ejecutar PCB %d, permanece en READY",
                        pcb_elegido->pid);
            // El PCB queda en lista_ready para el próximo ciclo del planificador
        }
    }
}

double calcular_estimacion(t_pcb *un_pcb)
{
    double tiempo_total_actual = un_pcb->metricas_tiempo[EXEC_PROCCES];
    double tiempo_rafaga_anterior = tiempo_total_actual - un_pcb->tiempo_exec_ultima_actualizacion;
    double estimacion_anterior = un_pcb->tiempo_estimacion;

    return (tiempo_rafaga_anterior * ALPHA) + estimacion_anterior * (1 - ALPHA);
}

void actualizar_estimacion_sjf(t_pcb *pcb)
{
    if (ALGORITMO_CORTO_PLAZO == SJF)
    {
        double nueva_estimacion = calcular_estimacion(pcb);
        pcb->tiempo_estimacion = nueva_estimacion;
        pcb->tiempo_exec_ultima_actualizacion = pcb->metricas_tiempo[EXEC_PROCCES];

        log_info(kernel_logger, "PCB %d - Nueva estimación: %lf", pcb->pid, nueva_estimacion);
    }
}
