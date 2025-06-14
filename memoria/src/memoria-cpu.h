#ifndef MEMORIA_CPU_H_
#define MEMORIA_CPU_H_

#include "utils/utils.h"
#include "gestorMemoria.h"

void procesar_conexion_memoria(void *void_args);
void recibir_pedido_instruccion(int *pid, int *pc, int socket);
void deserializar_pedido_instruccion(int *pid, int *pc, t_buffer *buffer);
bool id_process(void *elemento);
t_proceso_memoria *obtener_proceso_pid(int pid_pedido);
void enviar_instruccion(int socket, t_instruccion *instruccion);
void serializar_instruccion(t_paquete *paquete, t_instruccion *instruccion);

void liberar_instruccion(t_instruccion *instruccion);
bool _buscar_proceso(void *element);
void recibir_finalizar_proceso(int *pid, int socket);
t_instruccion *armar_estructura_instruccion(nombre_instruccion instruccion, char *parametro1, char *parametro2, char *parametro3, char *parametro4, char *parametro5);
void iniciar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria);
t_list *parsear_instrucciones(char *path);
char *leer_archivo(char *path);
t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo);
t_proceso_memoria *deserializar_proceso(t_buffer *buffer);
t_proceso_memoria *recibir_proceso_memoria(int socket_cliente);
t_instruccion *obtener_instruccion_del_proceso_pc(t_proceso_memoria *proceso, uint32_t pc);



#endif