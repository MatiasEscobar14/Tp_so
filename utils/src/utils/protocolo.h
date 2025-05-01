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
	//ESTRUCTURA_INICIALIZADA_OK,
	//SIN_ESPACIO_EN_MEMORIA,
	RTA_INICIALIZAR_ESTRUCTURAS_MK,
<<<<<<< HEAD
	RTA_CREAR_HILO_SYS_KM,
	RTA_LIBERAR_ESTRUCTURA_KM,
	RTA_DUMP_MEMORY_MK,
	FINALIZACION_HILO_KM
	
=======
>>>>>>> c047ab4210c130883fab0dfce669ec9e94bab8c5
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
int extraer_int_buffer(t_buffer* buffer);

t_paquete* crear_paquete(op_code code, t_buffer* unBuffer);
t_list* recibir_paquete(int socket_cliente);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);
void eliminar_paquete(t_paquete* paquete);
void paquete(int conexion);





#endif