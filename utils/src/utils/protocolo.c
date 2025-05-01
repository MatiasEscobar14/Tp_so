#include "protocolo.h"

void enviar_mensaje(char* mensaje, int socket_cliente, t_log* logger){
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);
	log_info(logger, "Mensaje enviado: %s", mensaje);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void* serializar_paquete(t_paquete* paquete, int bytes){
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}
void eliminar_paquete(t_paquete* paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
/*
void crear_buffer(t_paquete* paquete){
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}*/
t_buffer* new_buffer(){
	t_buffer* unBuffer = malloc(sizeof(t_buffer));
	unBuffer->size = 0;
	unBuffer->stream = NULL;
	return unBuffer;
}



void add_int_to_buffer(t_buffer* unBuffer, int int_value){
	add_to_buffer(unBuffer, &int_value, sizeof(int));
}
void add_to_buffer(t_buffer* unBuffer, void* new_stream, int new_size)
{
	if(unBuffer->size == 0){
		unBuffer->stream = malloc(sizeof(int) + new_size);
		memcpy(unBuffer->stream, &new_size, sizeof(int));
		memcpy(unBuffer->stream + sizeof(int), new_stream, new_size);
	}else{
		unBuffer->stream = realloc(unBuffer->stream, unBuffer->size + sizeof(int) + new_size);
		memcpy(unBuffer->stream + unBuffer->size, &new_size, sizeof(int));
		memcpy(unBuffer->stream + unBuffer->size + sizeof(int), new_stream, new_size);
	}
	
	unBuffer->size += sizeof(int);
	unBuffer->size += new_size;
}


void* extraer_buffer(t_buffer* buffer){

	int tamanio_datos;									         //leo el tamanio de datos a extraer
	memcpy(&tamanio_datos, buffer->stream, sizeof(int));
	void* stream = malloc(tamanio_datos);					     //reservo memoria
	memcpy(stream, buffer->stream + sizeof(int), tamanio_datos); //copio los dtos extraidos
	int nuevo_tamanio = buffer->size - sizeof(int) - tamanio_datos;     //calculo el nuevo tamanio

	if(nuevo_tamanio == 0){
		buffer->size = 0;
		free(buffer->stream);
		buffer->stream = NULL;
		return stream;
	}

	if(nuevo_tamanio < 0){
		perror("\n[ERROR] buffer con tamaño negativo");
		exit(EXIT_FAILURE);
	}

	//si quedan datos en el buffer

	void* nuevo_stream = malloc(nuevo_tamanio); //reservo memoria para nuevo stream
	memcpy(nuevo_stream, buffer->stream + sizeof(int) + tamanio_datos, nuevo_tamanio);
	free(buffer->stream);
	buffer->size = nuevo_tamanio;
	buffer->stream = nuevo_stream;

	return stream;
}

int extraer_int_buffer(t_buffer* buffer){
	int* int_valor = extraer_buffer(buffer);
	int valor = *int_valor;
	free(int_valor);
	return valor;
	
}


t_paquete* crear_paquete(op_code code, t_buffer* unBuffer){

	t_paquete* unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigo_operacion = code;
	unPaquete->buffer = unBuffer;
	return unPaquete;
}


void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}
void enviar_paquete(t_paquete* paquete, int socket_cliente){
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

int recibir_operacion(int socket_cliente){
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else {
		close(socket_cliente);
		return -1;
	}
}
void* recibir_buffer(int* size, int socket_cliente){
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(t_log* logger,int socket_cliente) {
    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);
    log_info(logger, "Me llego el mensaje: %s", buffer);
    free(buffer);
}

t_list* recibir_paquete(int socket_cliente){

	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}