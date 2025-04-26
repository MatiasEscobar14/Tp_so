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

//=======LISTAS======//

extern t_list* lista_new;
extern t_list* lista_ready;
extern t_list* lista_execute;
extern t_list* lista_blocked;	
extern t_list* lista_exit;
extern t_list* lista_susp_blocked;
extern t_list* lista_susp_ready;

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

#endif