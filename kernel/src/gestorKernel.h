#ifndef GESTOR_KERNEL_H
#define GESTOR_KERNEL_H

#include <utils/utils.h>


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

//=======PCB========//

typedef struct {
    uint32_t pid;
    uint32_t pc;
    uint32_t tamanio_proceso;
    t_list* metricas_estado;
    t_list* metricas_tiempo;
}pcb_t;

extern bool flag_pedido_de_memoria;

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


//======ESTADOS======//

typedef enum{
	NEW_PROCCES,
	READY_PROCCES,
	EXEC_PROCCES,
	BLOCKED_PROCCES,
	EXIT_PROCCES,
    SUSP_BLOCKED_PROCESS,
    SUSP_READY_PROCESS
}estado_pcb;

//=======CONEXIONES======//
extern int socket_io;
extern int socket_cpu_dispatch;
extern int socket_cpu_interrupt;
extern int socket_memoria;


#endif