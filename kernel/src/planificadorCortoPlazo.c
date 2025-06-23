#include <planificadorCortoPlazo.h>

sem_t sem_cpu_disponible;

void planificadorCortoPlazo()
{
     while (1)
    {   
        log_info(kernel_logger, "Esperando a que se conecte una CPU...");
        sem_wait(&sem_cpu_disponible); // Espera a que haya una CPU disponible
        log_info(kernel_logger, "CPU conectada, enviando pcb a CPU.");  
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

void atender_FIFO() {
    log_info(kernel_logger, "Comienzo a ejecutar FIFO (planificador corto plazo)");
    // Intentar obtener un proceso de la lista READY
    pthread_mutex_lock(&mutex_lista_ready);
    t_pcb* un_pcb = NULL;

    if (!list_is_empty(lista_ready)) {
        un_pcb = list_get(lista_ready, 0);
    }
    pthread_mutex_unlock(&mutex_lista_ready);

    if (un_pcb != NULL) {
        
        cambiar_estado(un_pcb, EXEC_PROCCES);
        remover_pcb_lista(un_pcb, lista_ready, &mutex_lista_ready);
        agregar_pcb_lista(un_pcb, lista_execute, &mutex_lista_execute);
    
        t_modulo_cpu* modulo_cpu = enviar_pcb_a_cpu(un_pcb); 

        //atender_kernel_cpu_dispatch(&(modulo_cpu->socket_fd_dispatch));  // Espera PID + motivo de finalizacion/interrupcion
        //falta agregar como protocolo los motivos de finalizacion/interrupcion
    }
}


double calcular_estimacion(t_pcb* un_pcb){
    double tiempo_total_actual = un_pcb->metricas_tiempo[EXEC_PROCESS];
    double tiempo_rafaga_anterior = tiempo_total_actual - un_pcb->tiempo_exec_ultima_actualizacion;
    double estimacion_anterior = un_pcb->tiempo_estimacion;
    
    return (tiempo_rafaga_anterior * ALPHA) + estimacion_anterior * (1 - ALPHA);
}

void actualizar_estimacion_sjf(t_pcb* pcb) {
    if (ALGORITMO_CORTO_PLAZO == SJF) { 
        double nueva_estimacion = calcular_estimacion(pcb);
        pcb->tiempo_estimacion = nueva_estimacion;
        pcb->tiempo_exec_ultima_actualizacion = pcb->metricas_tiempo[EXEC_PROCESS];
        
        log_info("PCB %d - Nueva estimación: %lf", pcb->pid, nueva_estimacion);
    }
}


