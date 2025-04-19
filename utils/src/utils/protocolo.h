#ifndef PROTCOLO_H_
#define PROTCOLO_H_

#include "utils.h"

typedef enum
{
	HANDSHAKE,
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void enviar_mensaje(char* mensaje, int socket_cliente,t_log* logger);
void* serializar_paquete(t_paquete* paquete, int bytes);
void eliminar_paquete(t_paquete* paquete);
void crear_buffer(t_paquete* paquete);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void paquete(int conexion);
void recibir_mensaje(t_log* logger,int socket_cliente);
int recibir_operacion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
t_list* recibir_paquete(int socket_cliente);

#endif