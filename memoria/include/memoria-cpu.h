#ifndef UTILS_MEMORIA_H_
#define UTILS_MEMORIA_H_

#include "/home/utnso/tp-2025-1c-Linux-Learners/memoria/include/gestor.h"
#include "/home/utnso/tp-2025-1c-Linux-Learners/memoria/src/gestorMemoria.h"

typedef struct {
    uint32_t pid_buscado;
} t_pid_contexto;

static void procesar_conexion_memoria(void *void_args);
void recibir_pedido_instruccion(uint32_t *pid, uint32_t *pc, int socket);
void deserializar_pedido_instruccion(uint32_t *pid, uint32_t *pc, t_buffer *buffer);
t_proceso_memoria *obtener_proceso_pid(uint32_t pid_pedido);
bool id_process(void *elemento);
void enviar_instruccion(int socket, t_instruccion *instruccion);
void serializar_instruccion(t_paquete *paquete, t_instruccion *instruccion);
t_instruccion *obtener_instruccion_del_proceso_pc(t_proceso_memoria *proceso, uint32_t pc);
t_proceso_memoria *recibir_proceso_memoria(int socket_cliente);
t_proceso_memoria *deserializar_proceso(t_buffer *buffer);
t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo);
char *leer_archivo(char *path);
t_list *parsear_instrucciones(char *path);
void iniciar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria);
t_instruccion *armar_estructura_instruccion(nombre_instruccion instruccion, char *parametro1, char *parametro2, char *parametro3, char *parametro4, char *parametro5);
void recibir_finalizar_proceso(uint32_t *pid, int socket);
bool _buscar_proceso(void *element);
void liberar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria);
void liberar_instruccion(t_instruccion *instruccion);


#endif