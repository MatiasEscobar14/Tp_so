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

//FUNCIONES PARA RECIBIR DEL BUFFER
void* extraer_buffer(t_buffer* unBuffer){
	if(unBuffer->size == 0){
		perror("\n[ERROR] Al intentar extraer un contenido de un buffer vacio\n");
		exit(EXIT_FAILURE);
	}

	if(unBuffer->size < 0){
		perror("\n[ERROR] size negativo\n");
		exit(EXIT_FAILURE);
	}

	int ext_size;
	memcpy(&ext_size, unBuffer->stream, sizeof(int));
	void* ext_stream = malloc(ext_size);
	memcpy(ext_stream, unBuffer->stream + sizeof(int), ext_size);

	int new_size = unBuffer->size - sizeof(int) - ext_size;
	if(new_size == 0){
		unBuffer->size = 0;
		free(unBuffer->stream);
		unBuffer->stream = NULL;
		return ext_stream;
	}

	if(new_size < 0){
		perror("\n[ERROR] buffer con tamaño negativo");
		exit(EXIT_FAILURE);
	}

	void* new_stream = malloc(new_size);
	memcpy(new_stream, unBuffer->stream + sizeof(int) + ext_size, new_size);
	free(unBuffer->stream);
	unBuffer->size = new_size;
	unBuffer->stream = new_stream;

	return ext_stream;
}

int extraer_int_buffer(t_buffer* unBuffer){
	int* ext_int = extraer_buffer(unBuffer);
	int value = *ext_int;
	free(ext_int);
	return value;
}

char* extraer_string_buffer(t_buffer* unBuffer){
	char* ext_string = extraer_buffer(unBuffer);
	return ext_string;
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


t_buffer* recv_buffer(int socket_cliente)
{
	t_buffer* unBuffer = malloc(sizeof(t_buffer));

	if(recv(socket_cliente, &(unBuffer->size), sizeof(int), MSG_WAITALL) > 0){
		unBuffer->stream = malloc(unBuffer->size);

		if(recv(socket_cliente, unBuffer->stream, unBuffer->size, MSG_WAITALL) > 0){
			return unBuffer;
		}else{
			perror("Error al recibir el void* del buffer de la conexion");
			exit(EXIT_FAILURE);
		}
	}else{
		perror("Error al recibir el tamaño del buffer de la conexion");
		exit(EXIT_FAILURE);
	}
	

	return unBuffer;
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