#ifndef PROTCOLO_H_
#define PROTCOLO_H_

#include "utils.h"

typedef enum
{
	HANDSHAKE,
	MENSAJE,
	PAQUETE,
	//CPU
	INSTRUCCION,
	PEDIDO_INSTRUCCION,

	//====KERNEL - MEMORIA====//
	INICIALIZAR_ESTRUCTURAS_KM,
	FINALIZAR_ESTRUCTURAS_KM,
	//ESTRUCTURA_LIBERADA_OK,
	//SIN_ESPACIO_EN_MEMORIA,
	RTA_INICIALIZAR_ESTRUCTURAS_MK,
	RTA_ESTRUCTURA_LIBERADA_KM,
	ESTRUCTURA_LIBERADA_OK,
	DUMP_MEMORY_KM,

	//====KERNEL - CPU====//
	PCB,
	INIT_PROC,
	IO,
	DUMP_MEMORY,
	EXIT,


	//RTA_DUMP_MEMORY_MK,


	//===== KERNEl- CPU ======//

	EJECUTAR_PROCESO_KC
	
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
void recibir_mensaje(t_log* logger,int socket_cliente);

int recibir_operacion(int socket_cliente);

t_buffer* new_buffer();
void* recibir_buffer(int* size, int socket_cliente);
void add_to_buffer(t_buffer* unBuffer, void* new_stream, int new_size);
void add_int_to_buffer(t_buffer* unBuffer, int int_value);
void* extraer_buffer(t_buffer* buffer);
int extraer_int_buffer(t_buffer* unBuffer);
char* extraer_string_buffer(t_buffer* unBuffer);

t_paquete* crear_paquete(op_code code, t_buffer* unBuffer);
t_list* recibir_paquete(int socket_cliente);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);
void eliminar_paquete(t_paquete* paquete);
void paquete(int conexion);



t_buffer* recv_buffer(int socket_cliente);



#endif