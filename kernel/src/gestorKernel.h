#ifndef GESTOR_KERNEL_H
#define GESTOR_KERNEL_H
#define CANT_ESTADOS 7
typedef struct{
char* nombre_io;
int pid;
int miliseg;
}t_syscall_io;


#include <utils/utils.h>
#include <pthread.h>
#include <semaphore.h>


extern t_log* kernel_logger;
extern t_config* kernel_config;

typedef enum{
    FIFO, 
    SJF,
    SJF_SD, 
    PMCP,
}t_algoritmo;


//=======CONFIG========//
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* PUERTO_ESCUCHA_DISPATCH;
extern char* PUERTO_ESCUCHA_INTERRUPT;
extern char* PUERTO_ESCUCHA_IO;
extern char* PUERTO_CPU_DISPATCH;
extern char* PUERTO_CPU_INTERRUPT;
extern t_algoritmo ALGORITMO_CORTO_PLAZO;
extern t_algoritmo ALGORITMO_INGRESO_A_READY;
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
    int pid;
    uint32_t pc;
    char* nombre_archivo;
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
extern t_list* lista_modulos_io_conectadas;
extern t_list* lista_modulos_cpu_conectadas;
//======MUTEX=======//

extern pthread_mutex_t mutex_lista_new;
extern pthread_mutex_t mutex_lista_ready;
extern pthread_mutex_t mutex_lista_blocked;
extern pthread_mutex_t mutex_lista_susp_ready;
extern pthread_mutex_t mutex_lista_susp_blocked;
extern pthread_mutex_t mutex_lista_exit;
extern pthread_mutex_t mutex_lista_modulos_cpu_conectadas;
extern pthread_mutex_t mutex_lista_execute;
extern pthread_mutex_t mutex_lista_modulos_io_conectadas;

extern sem_t sem_rpta_estructura_inicializada;
extern sem_t semaforo_largo_plazo;
extern sem_t sem_estructura_liberada;
extern sem_t sem_rpta_dump_memory;
extern sem_t sem_cpu_disponible;

extern pthread_mutex_t mutex_flag_pedido_memoria;

//=======CONEXIONES======//
extern int socket_io;
extern int socket_cpu_dispatch;             //TODO valen,  es necesario esto?
extern int socket_cpu_interrupt;            //TODO valen,  es necesario esto?  
extern int socket_memoria;

//extern bool flag_respuesta_dump;
//extern sem_t sem_estructura_liberada;

//======CPU's======//

typedef struct {
    int identificador;
    int socket_fd_dispatch;
    //int socket_interrupt;
    t_pcb* proceso_en_ejecucion;
    bool libre;
}t_modulo_cpu;

//=====IO=====/
typedef struct {                //?
    t_pcb* pcb;
    int milisegundos;
} t_io_esperando;

typedef struct {
    char* nombre;
    int socket_fd;  
    t_pcb* pcb_ejecutando;
    //t_queue* cola_espera;
    bool libre;
    pthread_mutex_t mutex;
} t_modulo_io;

typedef struct {
    t_queue* cola_espera;
    char* nombre;                  
    pthread_mutex_t mutex;
} t_io_espera_por_nombre;

//Valen prueba

extern t_list* lista_io_esperas;
extern pthread_mutex_t mutex_lista_io_esperas;


#endif