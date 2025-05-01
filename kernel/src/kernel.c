#include "kernel.h"


void crear_proceso_inicial(int tamanio_proceso, char* ruta){
	t_pcb* pcb = NULL;
	log_info(kernel_logger, "El nombre del proceso es: [%s] y el tamaño del mismo es: [%d] \n", ruta, tamanio_proceso);

	if(list_is_empty(lista_new)){
		pcb = crear_pcb(/*ruta*/, tamanio_proceso);
		//pthread_mutex_lock(&mutex_lista_new);
		list_add(lista_new, pcb);
		log_info(kernel_logger,"El PID es %d", pcb->pid);
		//pthread_mutex_unlock(&mutex_lista_new);
		log_info(kernel_logger, "Creación de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", pcb->pid);
		//sem_post(&sem_cpu_disponible); //lo pongo aca porque es el inicio y la cpu esta disponible
		//planificadorLargoPlazo();		PLANIFICADOR A LARGO PLAZO
	}
	
}

int main(int argc, char* argv[]) {
	

	char* archivo_pseudocodigo = argv[1];
    int tamanio_proceso = atoi(argv[2]);

	iniciar_kernel("kernel.config");

	//crear_proceso_inicial(tamanio_proceso, archivo_pseudocodigo);

	//iniciar_plp();

	primeraPCB = crear_pcb(16);
	printf("El PID es %d", primeraPCB->pid);
	printf("El PC es %d", primeraPCB->pc);
	printf("El tamanio del proceso es %d", primeraPCB->tamanio_proceso);
	printf("El estado del proceso es %d", primeraPCB->estado);
	printf("El tiempo de inicio del estado es %d", primeraPCB->tiempo_inicio_estado);
	printf("El estado del proceso es %s", estado_a_string(primeraPCB->estado));
	printf("Las veces que estuvo en este estado son: %d", primeraPCB->metricas_estado[0]);
	printf("El tiempo que estuvo en este estado es: %f", primeraPCB->metricas_tiempo[0]);

	printf("================CAMBIAMOS ESTADO A READY=====================");
	cambiar_estado(primeraPCB, READY_PROCCES);
	printf("El estado del proceso es %s", estado_a_string(primeraPCB->estado));
	printf("Las veces que estuvo en este estado son: %d", primeraPCB->metricas_estado[1]);
	printf("=====================================");
	printf("El tiempo que estuvo en NEW es: %f", primeraPCB->metricas_tiempo[0]);




	return 0;

}


	