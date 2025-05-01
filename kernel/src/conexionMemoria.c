#include <conexionMemoria.h>

void attender_kernel_memoria(int socket_memoria){
	t_buffer* un_buffer = malloc(sizeof(t_buffer));
	int respuesta; // 1 o 0          
		int cod_op = recibir_operacion(socket_memoria);
		switch (cod_op) {
		    case MENSAJE:
			    //
			    break;
		    case PAQUETE:
			    //
			    break;
			case RTA_INICIALIZAR_ESTRUCTURAS_MK:
				int tamanio;
				un_buffer = recibir_buffer(&tamanio, socket_memoria);
                respuesta = extraer_int_buffer(un_buffer);
				
				log_info(kernel_logger, "Se recibio respuesta init memo: %d", respuesta);
				if(respuesta == 1){
					pthread_mutex_lock(&mutex_flag_pedido_memoria);
					flag_pedido_de_memoria = true;
					log_info(kernel_logger, "Entramos a la respuesta correcta, flag: %d", flag_pedido_de_memoria);
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
				free(un_buffer);
                break;
				
		    default:
			    log_warning(kernel_logger,"OPERACION DESCONOCIDA - KERNEL - MEMORIA");
				free(un_buffer);
			    break;
	}
}
