#include "kernel.h"

void crear_proceso_inicial(int tamanio_proceso, char* ruta){
	t_pcb* pcb = NULL;
	log_info(kernel_logger, "El nombre del proceso es: [%s] y el tamaño del mismo es: [%d] \n", ruta, tamanio_proceso);

	if(list_is_empty(lista_new)){
		pcb = crear_pcb(ruta, tamanio_proceso);
		pthread_mutex_lock(&mutex_lista_new);
		list_add(lista_new, pcb);
		log_info(kernel_logger,"El PID es %d", pcb->pid);
		pthread_mutex_unlock(&mutex_lista_new);
		log_info(kernel_logger, "Creación de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", pcb->pid);
		iniciar_plp();	//	PLANIFICADOR A LARGO PLAZO
	}else{
		log_info(kernel_logger, "No hay suficiente espacio para crear el proceso");
	}
	
}

int main(int argc, char* argv[]) {
	

	char* archivo_pseudocodigo = argv[1];
    int tamanio_proceso = atoi(argv[2]);

	iniciar_kernel("kernel.config");

	pthread_t thread_conexiones;
    pthread_create(&thread_conexiones, NULL, crear_conexiones, NULL);

	crear_proceso_inicial(tamanio_proceso, archivo_pseudocodigo);
	pthread_join(thread_conexiones, NULL);

	return 0;

}


