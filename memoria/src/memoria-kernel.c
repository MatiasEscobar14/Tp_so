#include "memoria-kernel.h"

void attender_memoria_kernel(int socket_kernel){

	t_buffer* un_buffer;
    log_info(logger_memoria,"Esperando mensaje del kernel");
    int cod_op = recibir_operacion(socket_kernel);   
    int size;  
    switch (cod_op) {
   
        case INICIALIZAR_ESTRUCTURAS_KM:
            un_buffer = recibir_buffer(&size,socket_kernel);
            inicializar_estructuras(un_buffer,socket_kernel);
            free(un_buffer);
        break;

    }
    free(un_buffer);

}


void inicializar_estructuras(t_buffer* buffer, int socket_kernel){
    log_info(logger_memoria, "HOLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

    return;
}

