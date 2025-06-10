#include <planificadorCortoPlazo.h>

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

void atender_FIFO() {
    log_info(kernel_logger, "Comienzo a ejecutar FIFO");

    // Verificar si EXECUTE está vacío
    pthread_mutex_lock(&mutex_lista_execute);
    bool execute_esta_vacia = list_is_empty(lista_execute);
    pthread_mutex_unlock(&mutex_lista_execute);

    if (!execute_esta_vacia) {
        log_info(kernel_logger, "Lista EXECUTE no está vacía");
        return;
    }

    // Intentar obtener un proceso de la lista READY
    pthread_mutex_lock(&mutex_lista_ready);
    t_pcb* un_pcb = NULL;

    if (!list_is_empty(lista_ready)) {
        un_pcb = list_remove(lista_ready, 0);
    }
    pthread_mutex_unlock(&mutex_lista_ready);

    if (un_pcb != NULL) {
        enviar_pcb_a_cpu(un_pcb);
        cambiar_estado(un_pcb, EXEC_PROCCES);
        agregar_pcb_lista(un_pcb, lista_execute, mutex_lista_execute);
        
        //atender_kernel_cpu_dispatch(socket_fd_dispatch);  // Espera PID + motivo de finalizacion/interrupcion
    } else {
        log_info(kernel_logger, "Lista READY esta vacia");
    }
}


