#ifndef CPU_H_
#define CPU_H_

#include "gestorCPU.h"
#include "inicializar_estructuras.h"

void conectar_con_kernel(int identificador);
void conectar_con_dispatch(int socket_cliente);

#endif