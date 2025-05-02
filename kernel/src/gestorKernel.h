#ifndef GESTOR_KERNEL_H
#define GESTOR_KERNEL_H
#define CANT_ESTADOS 7

#include <utils/utils.h>
#include<pthread.h>
#include <semaphore.h>

extern t_log* kernel_logger;
extern t_config* kernel_config;


//=======CONFIG========//
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* PUERTO_ESCUCHA_DISPATCH;
extern char* PUERTO_ESCUCHA_INTERRUPT;
extern char* PUERTO_ESCUCHA_IO;
extern char* PUERTO_CPU_DISPATCH;
extern char* PUERTO_CPU_INTERRUPT;
extern char* ALOGRITMO_PLANIFICACION;
extern char* ESTIMACION_INICIAL;

//=======PCB========//

typedef enum{
	NEW_PROCCES,
	READY_PROCCES,
	EXEC_PROCCES,
	BLOCKED_PROCCES,
	EXIT_PROCCES,
    SUSP_BLOCKED_PROCESS,
    SUSP_READY_PROCESS
}estado_pcb;

typedef struct {
    uint32_t pid;
    uint32_t pc;
    uint32_t tamanio_proceso;
    int metricas_estado[CANT_ESTADOS];
    double metricas_tiempo[CANT_ESTADOS];    
    estado_pcb estado;
    time_t tiempo_inicio_estado;
}t_pcb;

extern bool flag_pedido_de_memoria;

//========ESTADO PLANIFICACION========//


//=======LISTAS======//

extern t_list* lista_new;
extern t_list* lista_ready;
extern t_list* lista_execute;
extern t_list* lista_blocked;	
extern t_list* lista_exit;
extern t_list* lista_susp_blocked;
extern t_list* lista_susp_ready;

/*extern t_list* lista_new_thread;
extern t_list* lista_ready_thread;
extern t_list* lista_execute_thread;
extern t_list* lista_blocked_thread;
extern t_list* lista_exit_thread;
extern t_list* lista_mutex_thread;
extern t_list* lista_iniciar_estructura;*/



//======MUTEX=======//

extern pthread_mutex_t mutex_lista_new;
extern pthread_mutex_t mutex_lista_ready;
extern pthread_mutex_t mutex_lista_ready_thread;
extern pthread_mutex_t mutex_flag_pedido_memoria;


extern sem_t sem_rpta_estructura_inicializada;
extern sem_t semaforo_largo_plazo;



//=======CONEXIONES======//
extern int socket_io;
extern int socket_cpu_dispatch;
extern int socket_cpu_interrupt;
extern int socket_memoria;

//extern bool flag_respuesta_dump;
//extern sem_t sem_estructura_liberada;
#endif