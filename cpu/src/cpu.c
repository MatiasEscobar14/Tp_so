#include "cpu.h"
#include "inicializar_estructuras.h"
/*pthread_mutex_t mutex_pcb_actual;
pthread_mutex_t mutex_interrupt;*/

int main(int argc, char *argv[])
{

    int identificador = 5; //atoi(argv[1]);
    char* nombre_log = string_from_format("cpu_%d.log", identificador);

    iniciar_config_cpu("cpu.config");
    iniciar_logger_cpu(nombre_log);

    log_info(cpu_logger, "Logger e config iniciados");

    conectar_con_kernel(identificador);
    while(1) {
        sleep(1); // Pausa 1 segundo para no consumir CPU innecesariamente
    }
    //socket_memoria = crear_conexion(cpu_logger, "Server Memoria", IP_MEMORIA, PUERTO_MEMORIA);
    
    /*log_info(cpu_logger, "===================================================");
    t_buffer* buffer = new_buffer();
            add_string_to_buffer(buffer, "proceso2");
            add_int_to_buffer(buffer, 30);
            t_paquete* nuevo_paquete = crear_paquete(INIT_PROC, buffer);
            enviar_paquete(nuevo_paquete, cliente_de_kernel_dispatch);
            log_info(cpu_logger, "Envie SYSCALL-INITI al kernel");
            atender_peticion_kernel(cliente_de_kernel_dispatch);
            log_info(cpu_logger, "El proceso se creo pelucheee.");
    log_info(cpu_logger, "===================================================");*/


    return 0;
}


void conectar_con_kernel(int identificador)
{
    // Para que es esta validacion? TODO valen
    if (cliente_de_kernel_interrupt == -1 || cliente_de_kernel_dispatch == -1) {
    log_error(cpu_logger, "No se pudo conectar con el Kernel");
    exit(EXIT_FAILURE);
    }

    cliente_de_kernel_interrupt = crear_conexion(cpu_logger, "Server kernel interrupt", IP_KERNEL, PUERTO_KERNEL_INTERRUPT);
    cliente_de_kernel_dispatch = crear_conexion(cpu_logger, "Server kernel dispatch", IP_KERNEL, PUERTO_KERNEL_DISPATCH);
    t_buffer* un_buffer = new_buffer();
    add_int_to_buffer(un_buffer, identificador);
    t_paquete* un_paquete = crear_paquete(HANDSHAKE, un_buffer);
    log_info(cpu_logger, "HANDSHAKE de la CPU: %d enviado.", identificador);
    enviar_paquete(un_paquete, cliente_de_kernel_dispatch);
    //TODO valen, el socket interrupt deberiaa pasarlo?

    
    return;
} 

void atender_peticion_kernel(int cliente_de_kernel_dispatch) {
    
        int cod_op = recibir_operacion(cliente_de_kernel_dispatch);
        //t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
        //int cliente_socket = args->fd;
        switch (cod_op) {
            case MENSAJE:
                log_info(cpu_logger, "Recibida solicitud de CPU del Kernel.");
                // Aquí podrías procesar la solicitud de IO
                /*
                t_buffer* buffer = new_buffer();
                add_string_to_buffer(buffer, "proceso2");
                add_int_to_buffer(buffer, 30);
                t_paquete* nuevo_paquete = crear_paquete(INIT_PROC, buffer);
                enviar_paquete(nuevo_paquete, cliente_de_kernel_dispatch);
                log_info(cpu_logger, "Envie SYSCALL-IO al kernel");
                */
                break;
            case EJECUTAR_PROCESO_KC:
    
                t_buffer* recibir_kernel = recv_buffer(cliente_de_kernel_dispatch);
                int pid = extraer_int_buffer(recibir_kernel);
                int pc = extraer_int_buffer(recibir_kernel);
    
                pcb_actual-> pid = pid;
                pcb_actual-> pc = pc;
                pcb_actual-> estado = EJECUTANDO;
                pcb_actual-> contexto_ejecucion = NULL;
                
                
                t_buffer* solicitud_instruccion = new_buffer();
                add_int_to_buffer(solicitud_instruccion, pid);
                t_paquete* un_paquete = crear_paquete(PEDIDO_INSTRUCCION, solicitud_instruccion);
                enviar_paquete(un_paquete, cliente_de_kernel_dispatch);
    
                //atender_peticion_memoria();
                sem_wait(&sem_rta_instruccion);
                
                log_info(cpu_logger, "Sali del semafoto SEM_RTA_INSTRUCCION");
    
                /*while (!hayInterrupciones() && pcb_actual != NULL && !esSyscall)
                {
                    ejecutar_ciclo_instruccion(cliente_de_kernel_dispatch); //(3)
                }
    
                log_debug(logger, "PID: %d, Contexto: %s\n", pcb_actual->pid, motivo_desalojo_to_string(pcb_actual->contexto_ejecucion->motivo_desalojo));
    
                // Envia el PCB actualizado si no ejecuto una syscall de pedido de recurso o IO
                if (!envioPcb)
                {
                    enviar_pcb(pcb_actual, cliente_de_kernel_dispatch); //(4)
                }
    
                envioPcb = false;
                esSyscall = false;
    
                pthread_mutex_lock(&mutex_interrupt); 
                limpiar_interrupciones(); //(5)
                pthread_mutex_unlock(&mutex_interrupt); 
                break;
    
    
                //devolver a kernel un motivo por el cual salio.*/
                break;
            case -1:
                log_error(cpu_logger, "Se desconectó el Kernel.");
                close(cliente_de_kernel_dispatch);
                break;
                
            default:
                break;
        }
    return;
}


void atender_peticion_memoria() {
    int cod_op = recibir_operacion(socket_memoria);

    switch (cod_op) {
        case MENSAJE:
        
            break;
        case RTA_INSTRUCCION:
            //t_instruccion* instruccion = deserializar_instruccion(socket_memoria);
            sem_post(&sem_rta_instruccion);
            break;
        case -1:
            log_error(cpu_logger, "Se desconectó el Kernel.");
            close(cliente_de_kernel_dispatch);
            break;
            
        default:
            break;
    }
    return;
}
