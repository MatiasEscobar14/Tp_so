#include "instrucciones.h"

//Representa la instrucción No Operation, es decir, va a consumir solamente el tiempo del ciclo de instrucción.

void _noop() {
    printf("entre a la instruccion noop");
}

//WRITE (Dirección, Datos): Escribe los datos del parámetro datos en la dirección física obtenida 
//a partir de la dirección lógica que se encuentra en el parámetro Dirección, 
//el campo datos será siempre una cadena de caracteres sin espacios.


void _write(char* dir_logica, char* datos, int socket) {
    printf("entre a la instruccion write");
}

//(Dirección, Tamaño): Lee el valor de memoria correspondiente a la dirección física obtenida a partir de la dirección lógica 
//que se encuentra en el parámetro Dirección, de un tamaño determinado por el parámetro Tamaño 
//y lo imprime por pantalla y en el Log Obligatorio correspondiente.


void _read(char* dir_logica, int tamanio, int socket){
    printf("entre a la instruccion read");
}

//(Valor): Actualiza el PC o Program Counter del proceso al valor pasado por parámetro.

void _goto(int valor){
   
    if (pcb_actual == NULL || pcb_actual->contexto_ejecucion == NULL) {
    log_error(cpu_logger, "_goto falló: pcb_actual o contexto_ejecucion no inicializado");
    return;
    }

    log_info(cpu_logger, "_goto: PC previo = %d", pcb_actual->contexto_ejecucion->registros->program_counter);
    pcb_actual->contexto_ejecucion->registros->program_counter = valor;
    log_info(cpu_logger, "_goto ejecutado. PC actualizado a %d", valor);
   
}

//syscalls

//(Dispositivo, Tiempo) 

//no estoy segura si es correcto este desarrollo
void _IO (char* dispositivo, int tiempo){
    
    if (!pcb_actual || !pcb_actual->contexto_ejecucion) {
        log_error(cpu_logger, "SYSCALL_IO falló: PCB no inicializado");
        return;
    }

    log_info(cpu_logger, "SYSCALL_IO: pid %d solicita IO \"%s\" (%d ms)",
             pcb_actual->pid, dispositivo, tiempo);

    t_buffer* buffer = new_buffer();
    add_int_to_buffer(buffer, pcb_actual->pid);
    add_string_to_buffer(buffer, dispositivo);
    add_int_to_buffer(buffer, tiempo);

    t_paquete* paquete = crear_paquete(SYSCALL_IO, buffer);
    enviar_paquete(paquete, cliente_de_kernel_dispatch);
    eliminar_paquete(paquete);
}

//(Archivo de instrucciones, Tamaño)
void _init_proc(char* path, int tamanio){
   
    log_info(cpu_logger, "SYSCALL INIT_PROC - archivo: %s, tamaño: %d", path, tamanio);

    t_buffer* buffer = new_buffer();
    add_string_to_buffer(buffer, path);
    add_int_to_buffer(buffer, tamanio);

    t_paquete* paquete = crear_paquete(SYSCALL_INIT_PROC, buffer);
    enviar_paquete(paquete, cliente_de_kernel_dispatch);
    eliminar_paquete(paquete);

}

void _dump_memory(){
    
    if (!pcb_actual) {
    log_error(cpu_logger, "DUMP_MEMORY: PCB no inicializado");
    return;
    }

    uint32_t pid = pcb_actual->pid;

    log_info(cpu_logger, "DUMP_MEMORY solicitado por PID: %d", pid);

    t_buffer* buffer = new_buffer();
    add_int_to_buffer(buffer, pid);

    t_paquete* paquete = crear_paquete(SYSCALL_DUMP_MEMORY, buffer);
    enviar_paquete(paquete, socket_memoria);
    eliminar_paquete(paquete);

    // Esperar respuesta de Memoria (bloqueante)
    int respuesta = recibir_operacion(socket_memoria);

    if (respuesta == RESPUESTA_DUMP_MEMORY) {
        log_error(cpu_logger, "DUMP_MEMORY falló en Memoria para PID: %d", pid);
        pcb_actual->contexto_ejecucion->motivo_desalojo = FINALIZACION;
        pcb_actual->contexto_ejecucion->motivo_finalizacion = OUT_OF_MEMORY;
    } else {
        log_info(cpu_logger, "DUMP_MEMORY finalizado correctamente para PID: %d", pid);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
    }

}

/*void _exit(){
    printf("entre a exit");
}*/