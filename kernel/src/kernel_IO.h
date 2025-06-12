#ifndef KERNEL_IO_H_
#define KERNEL_IO_H_

#include "gestorKernel.h"

void atender_kernel_io(int* socket_cliente);
void server_escuchar_io();
void imprimir_modulos_io();
t_modulo_io* obtener_modulo_io_por_socket(int socket);
t_io_espera_por_nombre* buscar_o_crear_cola_io(char* nombre);
bool misma_io(void* elemento);
#endif
