#include "kernel.h"

/*
int memoria_ocupada = 0;  // Inicialmente 0
int TAM_MEMORIA;          // Cargado desde el config
pthread_mutex_t mutex_memoria_ocupada = PTHREAD_MUTEX_INITIALIZER;
*/
void crear_proceso_inicial(int tamanio_proceso, char* ruta){
	t_pcb* pcb = NULL;
	log_info(kernel_logger, "El nombre del proceso es: [%s] y el tamaño del mismo es: [%d] \n", ruta, tamanio_proceso);

	if(list_is_empty(lista_new)){
		pcb = crear_pcb(/*ruta,*/ tamanio_proceso);
		pthread_mutex_lock(&mutex_lista_new);
		list_add(lista_new, pcb);
		log_info(kernel_logger,"El PID es %d", pcb->pid);
		pthread_mutex_unlock(&mutex_lista_new);
		log_info(kernel_logger, "Creación de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", pcb->pid);
		//sem_post(&sem_cpu_disponible); //lo pongo aca porque es el inicio y la cpu esta disponible
		// Se actualiza la memoria ocupada cuando el proceso es creado
          //  pthread_mutex_lock(&mutex_memoria_ocupada);
            //memoria_ocupada += tamanio_proceso;
            //pthread_mutex_unlock(&mutex_memoria_ocupada);

		iniciar_plp();	//	PLANIFICADOR A LARGO PLAZO
	}else{
		log_info(kernel_logger, "No hay suficiente espacio para crear el proceso");
	}
	
}

int main(int argc, char* argv[]) {
	

	char* archivo_pseudocodigo = argv[1];
    int tamanio_proceso = atoi(argv[2]);

	iniciar_kernel("kernel.config");
	crear_conexiones();
	crear_proceso_inicial(tamanio_proceso, archivo_pseudocodigo);
	
	
	return 0;

}


	