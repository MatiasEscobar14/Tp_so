#include <conexionMemoria.h>
// FUNCION NUEVA - Escuchar respuesta de Memoria
/*void* escuchar_memoria(void* arg) {
    while(1) {
        op_code cop;
        int res = recv(socket_memoria, &cop, sizeof(op_code), 0);
        if (res <= 0) {
            log_error(kernel_logger, "Error al recibir de Memoria o conexión cerrada");
            break;
        }

        switch(cop) {
            case ESTRUCTURA_INICIALIZADA_OK: 
                pthread_mutex_lock(&mutex_flag_pedido_memoria);
                flag_pedido_de_memoria = true;
                pthread_mutex_unlock(&mutex_flag_pedido_memoria);
                sem_post(&sem_rpta_estructura_inicializada);
                break;
            case SIN_ESPACIO_EN_MEMORIA:
                pthread_mutex_lock(&mutex_flag_pedido_memoria);
                flag_pedido_de_memoria = false;
                pthread_mutex_unlock(&mutex_flag_pedido_memoria);
                sem_post(&sem_rpta_estructura_inicializada);
                break;
            default:
                log_warning(kernel_logger, "Código inesperado recibido de Memoria");
                break;
        }
    }
    return NULL;
}
*/
/*
bool flag_respuesta_dump;

void attend_kernel_memoria(int socket_memoria){
    bool control_key = 1;
	t_buffer* un_buffer;
	int respuesta; // 1 o 0          
		int cod_op = recv_op(socket_memoria);
		switch (cod_op) {
		    case MENSAJE:
			    //
			    break;
		    case PAQUETE:
			    //
			    break;
			case RTA_INICIALIZAR_ESTRUCTURAS_MK:
				un_buffer = recv_buffer(socket_memoria);
                respuesta = extract_int_from_buffer(un_buffer);
				
				log_info(kernel_logger, "se recibio respuesta init memo: %d", respuesta);
				if(respuesta == 1){
					pthread_mutex_lock(&mutex_flag_pedido_memoria);
					flag_pedido_de_memoria = true;
					log_info(kernel_logger, "entramos a la respuesta correcta, flag: %d", flag_pedido_de_memoria);
					pthread_mutex_unlock(&mutex_flag_pedido_memoria);
				} else {
					pthread_mutex_lock(&mutex_flag_pedido_memoria);
					flag_pedido_de_memoria = false;
					pthread_mutex_unlock(&mutex_flag_pedido_memoria);
				}
				sem_post(&sem_rpta_estructura_inicializada);
				free(un_buffer);
				//free(un_buffer); // hay que liberar el buffer cada vez?
				break;
		    case -1:

			    log_error(kernel_logger, "DESCONEXION DE KERNEL - MEMORIA");
			    control_key = 0;
				//free(un_buffer);
                break;
				
		    default:
			    log_warning(kernel_logger,"OPERACION DESCONOCIDA - KERNEL - MEMORIA");
				//free(un_buffer);
			    break;
	}
}
*/