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

    pthread_mutex_lock(&mutex_lista_execute);
    if (list_is_empty(lista_execute)) {
        pthread_mutex_lock(&mutex_lista_ready);
        if(!lista_is_empty(lista_ready)){
            t_pcb *un_pcb = list_remove(lista_ready, 0); // FIFO: primer proceso en READY
        }
        pthread_mutex_unlock(&mutex_lista_ready);
            if(un_pcb != NULL){
                agregar_pcb_lista(un_pcb, lista_execute, mutex_lista_exec);
                cambiar_estado(un_pcb, EXEC_PROCCES);
                enviar_pcb_a_cpu(un_pcb);
                atender_kernel_cpu_dispatch();   //nos quedamos a la espera de de PID y motivo (?) sera con el "atender"?
            }else{log_info(kernel_logger, "Lista READY vacia");}
    }else{log_info(kernel_logger, "Lista EXECUTE no esta vacia");}
    pthread_mutex_unlock(&mutex_lista_exec); //va aca o va arriba?
}

