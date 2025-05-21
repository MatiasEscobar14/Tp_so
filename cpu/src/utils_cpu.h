#ifndef UTILS_CPU_H_
#define UTILS_CPU_H_

#include "gestorCPU.h"
#include <stdint.h>
#include <commons/collections/list.h>

void procesar_conexion_cpu(void *void_args);
void ejecutar_ciclo_instruccion(int socket);
t_instruccion *fetch(uint32_t pid, uint32_t pc);
void execute(t_instruccion *instruccion, int socket);
void log_instruccion_ejecutada(nombre_instruccion nombre, char *param1, char *param2, char *param3, char *param4, char *param5);
void loguear_y_sumar_pc(t_instruccion *instruccion);
void liberar_instruccion(t_instruccion *instruccion);
void crear_buffer(t_paquete *paquete);
t_paquete *crear_paquete_con_codigo_de_operacion(op_code codigo);
void pedir_instruccion_memoria(uint32_t pid, uint32_t pc, int socket);
t_instruccion *deserializar_instruccion(int socket);
void iniciar_semaforos_etc();
bool hayInterrupciones(void);
void limpiar_interrupciones(void);
char *motivo_desalojo_to_string(t_motivo_desalojo motivo);
char *instruccion_to_string(nombre_instruccion nombre);

extern bool esSyscall;
extern bool envioPcb;
extern bool interrupciones[5];


#endif