#include "syscalls.h"

void crear_proceso_sys(char* nombre_archivo, int tam_proceso){
	t_pcb* un_pcb = NULL;
	log_info(kernel_logger, "El PATH del proceso es: [%s] y el tamaño del mismo es: [%d] \n", nombre_archivo, tam_proceso);
	un_pcb = crear_pcb(nombre_archivo, tam_proceso);
	agregar_pcb_lista(un_pcb, lista_new, mutex_lista_new);
	planificadorLargoPlazo();
}

void dump_memory_sys(int pid){
	t_buffer* un_buffer = new_buffer();
	add_int_to_buffer(un_buffer, pid);
    t_paquete* un_paquete = crear_paquete(DUMP_MEMORY_KM, un_buffer);

	//TODO: crear conexion con memoria
    socket_memoria = crear_conexion(kernel_logger, "Memoria Server", IP_MEMORIA, PUERTO_MEMORIA);
	enviar_paquete(un_paquete, socket_memoria);
    atender_kernel_memoria();
    eliminar_paquete(un_paquete);
    liberar_conexion(socket_memoria);

	log_info(kernel_logger, "Se aviso a Memoria para que haga dump del proceso");
}