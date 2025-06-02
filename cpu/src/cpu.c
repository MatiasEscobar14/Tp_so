#include <utils/utils.h>
#include <utils/protocolo.h>
#include "gestorCPU.h"
#include "cpu.h"

/*pthread_mutex_t mutex_pcb_actual;
pthread_mutex_t mutex_interrupt;*/
void conectar_con_memoria();
int main(int argc, char *argv[]){

    int identificador = 5; //atoi(argv[1]);
    char *nombre_log = string_from_format("cpu_%d.log", identificador);

    iniciar_config_cpu("cpu.config");
    iniciar_logger_cpu(nombre_log);

    log_info(cpu_logger, "Logger e config iniciados");

    conectar_con_kernel(identificador);
   // conectar_con_memoria();
   // socket_memoria = crear_conexion(cpu_logger, "Server Memoria", IP_MEMORIA, PUERTO_MEMORIA);

    return 0;
}

void conectar_con_memoria(){
    socket_memoria = crear_conexion(cpu_logger, "Server Memoria", IP_MEMORIA, PUERTO_MEMORIA);
    while (1)
    {
        int cod_op = recibir_operacion(socket_memoria);
        switch (cod_op)
        {
        case  MENSAJE:
            log_info(cpu_logger,"llegue al mensaje");
            break;
        
        default:
            break;
        }
    }
    

}
void conectar_con_kernel(int identificador)
{
    // Para que es esta validacion? TODO valen
    if (cliente_de_kernel_interrupt == -1 || cliente_de_kernel_dispatch == -1)
    {
        log_error(cpu_logger, "No se pudo conectar con el Kernel");
        exit(EXIT_FAILURE);
    }

    cliente_de_kernel_interrupt = crear_conexion(cpu_logger, "Server kernel interrupt", IP_KERNEL, PUERTO_KERNEL_INTERRUPT);
    cliente_de_kernel_dispatch = crear_conexion(cpu_logger, "Server kernel dispatch", IP_KERNEL, PUERTO_KERNEL_DISPATCH);
    t_buffer *un_buffer = new_buffer();
    add_int_to_buffer(un_buffer, identificador);
    t_paquete *un_paquete = crear_paquete(HANDSHAKE, un_buffer);
    
    enviar_paquete(un_paquete, cliente_de_kernel_dispatch);
    log_info(cpu_logger,"Enviado HANDSHAKE a Kernel con ID: %d",identificador);
    // TODO valen, el socket interrupt deberiaa pasarlo?
   
    while (1)
    {
        int cod_op = recibir_operacion(cliente_de_kernel_dispatch);
        if(cod_op == MENSAJE){
            log_info(cpu_logger,"Handshake ACK recibido del Kernel.");
        }else{
            log_warning(cpu_logger,"Codigo de respuesta inesperado del Kernel: %d", cod_op);
        }
        t_paquete *paquete_recibido = recibir_paquete(cliente_de_kernel_dispatch);

        switch (cod_op)
        {
        case MENSAJE:
            log_info(cpu_logger, "Recibida solicitud de CPU del Kernel.");
            // Supongamos que recibiste una instrucción tipo: IO teclado 100
           /* t_paquete *paquete_recibido = recibir_paquete(cliente_de_kernel_dispatch);
            t_buffer *buffer_paquete = paquete_recibido->buffer;
            char* nombre_io = extraer_string_buffer(buffer_paquete);  // "teclado"
            int tiempo = extraer_int_buffer(buffer_paquete);          // 100
            int pid = extraer_int_buffer(buffer_paquete); */            // PID asociado al proceso
            // Aquí podrías procesar la solicitud de IO

            t_buffer *buffer = new_buffer();
            add_string_to_buffer(buffer, "teclado");
            add_int_to_buffer(buffer, 0);
            add_int_to_buffer(buffer, 100);
            t_paquete *nuevo_paquete = crear_paquete(IO, buffer);
            enviar_paquete(nuevo_paquete, cliente_de_kernel_dispatch);
            
            log_info(cpu_logger, "Envie SYSCALL-IO al kernel");

            break;

        case -1:
            log_error(cpu_logger, "Se desconectó el Kernel.");
            close(cliente_de_kernel_dispatch);
            return;

        default:
        }
    }
    return;
}
