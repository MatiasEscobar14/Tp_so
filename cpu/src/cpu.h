#ifndef CPU_H_
#define CPU_H_

#include "gestorCPU.h"
#include "inicializar_estructuras.h"

void conectar_con_kernel(int identificador);
void conectar_con_dispatch(int socket_cliente);
void conectar_con_memoria();
void atender_cpu_kernel_dispatch_single(int socket_kernel);
void atender_peticion_kernel(int cliente_de_kernel_dispatch);

#endif