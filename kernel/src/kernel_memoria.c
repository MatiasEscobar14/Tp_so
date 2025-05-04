#include<kernel_memoria.h>

void atender_kernel_memoria(){
t_buffer* un_buffer;
    int cod_op = recibir_operacion(socket_memoria);   
    int respuesta;  
    switch (cod_op) {
   
        case RTA_INICIALIZAR_ESTRUCTURAS_MK:
            un_buffer = recv_buffer(socket_memoria);
            respuesta = extract_int_buffer(un_buffer);

            log_info(kernel_logger,"se recibio respuesta inicial de memoria: %d", respuesta);

            if(respuesta == 1){
                pthread_mutex_lock(&mutex_flag_pedido_memoria);
                flag_pedido_de_memoria = true;
                log_info(kernel_logger,"entramos a la respuesta correcta, flag: %d", flag_pedido_de_memoria);
                pthread_mutex_unlock(&mutex_flag_pedido_memoria);
            }else{
                pthread_mutex_lock(&mutex_flag_pedido_memoria);
                flag_pedido_de_memoria = false;
                pthread_mutex_unlock(&mutex_flag_pedido_memoria);
            }
            sem_post(&sem_rpta_estructura_inicializada);
            free(un_buffer);
        break;

    }
    
}

