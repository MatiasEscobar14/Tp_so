#ifndef PROTCOLO_H_
#define PROTCOLO_H_

#include "utils.h"

typedef enum
{
	HANDSHAKE,
	MENSAJE,
	PAQUETE,

	//====KERNEL - MEMORIA====//
	INICIALIZAR_ESTRUCTURAS_KM,
	ESTRUCTURA_INICIALIZADA_OK,
	SIN_ESPACIO_EN_MEMORIA,
	RTA_INICIALIZAR_ESTRUCTURAS_MK,
	RTA_CREAR_HILO_SYS_KM,
	RTA_LIBERAR_ESTRUCTURA_KM,
	RTA_DUMP_MEMORY_MK,
	FINALIZACION_HILO_KM
	
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
//void crear_buffer(t_paquete* paquete);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void paquete(int conexion);
void recibir_mensaje(t_log* logger,int socket_cliente);
int recibir_operacion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
t_list* recibir_paquete(int socket_cliente);
t_buffer* new_buffer();
void add_to_buffer(t_buffer* unBuffer, void* new_stream, int new_size);
void add_int_to_buffer(t_buffer* unBuffer, int int_value);
t_paquete* create_super_pck(op_code code, t_buffer* unBuffer);

#endif