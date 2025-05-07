#include "inicializar_estructuras.h"

	t_list* lista_execute;
	t_list* lista_blocked;
	t_list* lista_exit;
	t_list* lista_susp_blocked;
	t_list* lista_susp_ready;



void iniciar_logger_kernel(){
	kernel_logger = log_create("kernel.log","Kernel", 0, LOG_LEVEL_INFO);
	log_info(kernel_logger, "Logger de Kernel iniciado.");
}

void iniciar_config_kernel(char* ruta){
	kernel_config = config_create(ruta);
	
	if(kernel_config ==  NULL){
		log_info(kernel_logger,  "No se pudo cargar la config.");
		return;
	}

	IP_MEMORIA = config_get_string_value(kernel_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_DISPATCH = config_get_string_value(kernel_config, "PUERTO_ESCUCHA_DISPATCH");
    PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(kernel_config, "PUERTO_ESCUCHA_INTERRUPT");
    PUERTO_ESCUCHA_IO = config_get_string_value(kernel_config, "PUERTO_ESCUCHA_IO");
	PUERTO_CPU_DISPATCH = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	PUERTO_CPU_INTERRUPT = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
	char* algoritmo_corto_plazo = config_get_string_value(kernel_config, "ALGORITMO_CORTO_PLAZO");
	ALGORITMO_INGRESO_A_READY = config_get_string_value(kernel_config, "ALGORITMO_INGRESO_A_READY");
	ESTIMACION_INICIAL = config_get_string_value(kernel_config,"ESTIMACION_INICIAL" );

	log_info(kernel_logger, "Config de Kernel iniciado.");

	if(strcmp(algoritmo_corto_plazo, "FIFO") == 0){
		ALGORITMO_CORTO_PLAZO = FIFO;
		log_info(kernel_logger,"[Algoritmo de planificación: FIFO]");
	}else if(strcmp(algoritmo_corto_plazo, "SJF") == 0){
		//ALGORITMO_CORTO_PLAZO = SJF;
		log_info(kernel_logger,"[Algoritmo de planificación: SJF]");	
	}else if(strcmp(algoritmo_corto_plazo, "SJF_SD") == 0){
		//ALGORITMO_CORTO_PLAZO = SJF_SD;
		log_info(kernel_logger,"[Algoritmo de planificación: SJF_SD]");
	}else{
		log_error(kernel_logger,"[Algoritmo de planificación no válido]");
		exit(EXIT_FAILURE);
	}

}
	
void iniciar_lista(){

	lista_new = list_create();
	lista_ready = list_create();
	lista_execute = list_create();
	lista_blocked = list_create();
	lista_exit = list_create();
    lista_susp_blocked = list_create();
    lista_susp_ready = list_create();
	log_info(kernel_logger, "Listas inicializadas.");
}

void iniciar_kernel(char* ruta_config){
	iniciar_logger_kernel();
	iniciar_config_kernel(ruta_config);
	iniciar_lista();
	initialize_mutex();
	initialize_semaphores();
}
void initialize_mutex(){
	pthread_mutex_init(&mutex_lista_new, NULL);
	pthread_mutex_init(&mutex_lista_ready, NULL);
	pthread_mutex_init(&mutex_lista_exec, NULL);
	pthread_mutex_init(&mutex_lista_blocked, NULL);
	pthread_mutex_init(&mutex_lista_exit, NULL);
	pthread_mutex_init(&mutex_lista_susp_blocked, NULL);
	pthread_mutex_init(&mutex_lista_susp_ready, NULL);
	log_info(kernel_logger, "Mutex inicializados.");

}
void initialize_semaphores(){
		sem_init(&semaforo_largo_plazo,0,0);
		sem_init(&sem_rpta_estructura_inicializada,0,0);
		sem_init(&sem_estructura_liberada,0,0);
		log_info(kernel_logger, "Sempaphores inicializados.");

}