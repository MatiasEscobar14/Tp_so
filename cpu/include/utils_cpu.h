#ifndef UTILS_CPU_H_
#define UTILS_CPU_H_

#include <stdint.h>
#include <commons/collections/list.h>
#include </home/utnso/tp-2025-1c-Linux-Learners/cpu/include/gestor.h>


static void procesar_conexion_cpu(void *void_args);
void ejecutar_ciclo_instruccion(int socket);
t_instruccion *fetch(uint32_t pid, uint32_t pc);
void execute(t_instruccion *instruccion, int socket);
void liberar_instruccion(t_instruccion *instruccion);
void pedir_instruccion_memoria(uint32_t pid, uint32_t pc, int socket);
t_instruccion *deserializar_instruccion(int socket);
void iniciar_semaforos_etc();
bool hayInterrupciones(void);

typedef enum
{
    NUEVO,
    LISTO,
    EJECUTANDO,
    BLOQUEADO,
    FINALIZADO
} t_estado_proceso;

typedef struct
{
    uint32_t program_counter;
    uint8_t ax, bx, cx, dx;
    uint32_t eax, ebx, ecx, edx, si, di;
} t_registros;

typedef enum
{
    SIN_MOTIVO,
    INTERRUPCION_FIN_QUANTUM,
    INTERRUPCION_BLOQUEO,
    INTERRUPCION_FINALIZACION,
    FINALIZACION,
    INTERRUPCION_ERROR,
    INTERRUPCION_OUT_OF_MEMORY,
    INTERRUPCION_SYSCALL
} t_motivo_desalojo;

typedef enum
{
    FINALIZACION_SIN_MOTIVO,
    SUCCESS,
    INVALID_RESOURCE,
    INVALID_INTERFACE,
    OUT_OF_MEMORY,
    INTERRUPTED_BY_USER
} t_motivo_finalizacion;

typedef struct
{
    t_registros *registros;
    t_motivo_desalojo motivo_desalojo;
    t_motivo_finalizacion motivo_finalizacion;
} t_contexto_ejecucion;

typedef struct
{
    uint32_t pid;
    t_estado_proceso estado;
    uint32_t quantum;
    uint64_t tiempo_q;
    t_list *recursos_asignados;
    t_contexto_ejecucion *contexto_ejecucion;
} t_pcb;

extern t_pcb *pcb_actual;
extern bool esSyscall;
extern bool envioPcb;
extern bool interrupciones[5];


#endif