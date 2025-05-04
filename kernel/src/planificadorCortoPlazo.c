/*#include <planificadorCortoPlazo.h>

void planificadorCortoPlazo(){
while (1){
   //sem_wait(&sem_cpu_disponible);
    int flag_lista_ready_vacia = 0;
    pthread_mutex_lock(&mutex_lista_ready_thread);
    if (list_is_empty(lista_ready_thread)){
        flag_lista_ready_vacia= 1;
    }
    pthread_mutex_unlock(&mutex_lista_ready_thread);
    
    if(flag_lista_ready_vacia == 0){
        switch (ALGORITMO_CORTO_PLAZO){
        case FIFO:
            atender_FIFO();
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

    pthread_mutex_lock(&mutex_lista_exec);
    if (list_is_empty(lista_execute)) {
        t_pcb* un_pcb = NULL;
        log_info(kernel_logger, "FIFO - Lista EXEC vacía, buscando en READY");

        pthread_mutex_lock(&mutex_lista_ready);
        if (!list_is_empty(lista_ready)) {
            un_pcb = list_remove(lista_ready, 0);  // FIFO: primer proceso en READY
        }
        pthread_mutex_unlock(&mutex_lista_ready);

        if (un_pcb != NULL) {
            list_add(lista_execute, un_pcb);

            log_info(kernel_logger, "PID: %d - Estado: READY → EXEC", un_pcb->pid);
            log_info(kernel_logger, "PID: %d - Estado: READY → EXEC", un_pcb->pid);

            cambiar_estado(un_pcb, EXEC_PROCCES);
            //enviar_pcb_a_cpu(un_pcb);  // Función que realiza el envío al CPU
        } else {
            log_warning(kernel_logger, "La lista READY está vacía");
        }
    } else {
        log_info(kernel_logger, "La lista EXEC no está vacía");
    }
    pthread_mutex_unlock(&mutex_lista_exec);
}
*/