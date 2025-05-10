#include <planificadorCortoPlazo.h>

void planificadorCortoPlazo()
{
    while (1)
    {
        // sem_wait(&sem_cpu_disponible);
        int flag_lista_ready_vacia = 0;

        if (flag_lista_ready_vacia == 0)
        {
            switch (ALGORITMO_CORTO_PLAZO)
            {
            case FIFO:
                atender_FIFO();
                break;

            case SJF:

                break;
            case SJF_SD:

                break;

            default:
                log_error(kernel_logger, "ALGORITMO DE CORTO PLAZO DESCONOCIDO");
                exit(EXIT_FAILURE);
                break;
            }
        }
    }
}

void atender_FIFO() {
    log_info(kernel_logger, "Comienzo a ejecutar FIFO");

    pthread_mutex_lock(&mutex_lista_ready);

    if (!list_is_empty(lista_ready)) {
        t_pcb *un_pcb = list_remove(lista_ready, 0); // FIFO: primer proceso en READY

        pthread_mutex_unlock(&mutex_lista_ready);

        cambiar_estado(un_pcb, EXEC_PROCCES);

        agregar_pcb_lista(un_pcb, lista_execute, mutex_lista_exec); // suponiendo que la función maneja su mutex o no lo necesita

        log_info(kernel_logger, "PID: %d - Estado: READY → EXEC", un_pcb->pid);

        enviar_pcb_a_cpu(un_pcb);

        //atender_kernel_cpu_dispatch();
        //TDOO: Nos quedamos esperanod el PID y el motivo !!



        // sem_wait(&?);
    } else {
        log_warning(kernel_logger, "La lista READY está vacía");
        
    }
    pthread_mutex_unlock(&mutex_lista_ready);
}

