#ifndef CONTEXTO_H_
#define CONTEXTO_H_

#include <stdint.h>
#include <commons/collections/list.h>
#include <utils/utils.h>
#include <utils/protocolo.h>

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
    t_motivo_desalojo motivo_desalojo;
    t_motivo_finalizacion motivo_finalizacion;
} t_contexto_ejecucion;

typedef struct
{
    int pid;
    int pc;
    t_estado_proceso estado;
    //t_list *recursos_asignados;
    t_contexto_ejecucion *contexto_ejecucion;
} t_pcb;


#endif