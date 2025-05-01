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
    log_info(logger_memoria, "HOLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    int pid = extraer_int_buffer(buffer);
    int pc = extraer_int_buffer(buffer);
    int tamanio = extraer_int_buffer(buffer);
    
   
    log_info(logger_memoria,"Solicitud de inicialización PID: %d, tamanio: %d", pid,tamanio);

    if (hay_espacio_disponible(tamanio)){
       
          
    }else{
     
       
    }

    free(path);
    
    
    return;
}
bool hay_espacio_disponible(int size_proceso) {
    return (memoria_usada + size_proceso) <= TAM_MEMORIA;
}
