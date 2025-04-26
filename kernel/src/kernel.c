#include "kernel.h"


void crear_proceso_inicial(int tamanio_proceso, char* ruta){
	pcb_t* pcb = NULL;
	log_info(kernel_logger, "El nombre del proceso es: [%s] y el tamaño del mismo es: [%d] \n", ruta, tam_proceso);

	if(list_is_empty(lista_new)){
		pcb = crear_pcb(ruta, tam_proceso, 0);
		//pthread_mutex_lock(&mutex_lista_new);
		list_add(lista_new, un_pcb);
		log_info(kernel_logger,"El PID es %d", un_pcb->pid);
		//pthread_mutex_unlock(&mutex_lista_new);
		log_info(kernel_logger, "Creación de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", un_pcb->pid);
		//sem_post(&sem_cpu_disponible); //lo pongo aca porque es el inicio y la cpu esta disponible
		//planificadorLargoPlazo();		PLANIFICADOR A LARGO PLAZO
	}
}

int main(int argc, char* argv[]) {
	

	char* archivo_pseudocodigo = argv[1];
    int tamanio_proceso = atoi(argv[2]);

	iniciar_kernel("kernel.config");

	crear_proceso_inicial(tamanio_proceso, archivo_pseudocodigo);
   
	//===========================================CONEXION KERNEL (CLIENTE) CON MEMORIA (SERVER)========================================================//
	
	
	return 0;

}
	