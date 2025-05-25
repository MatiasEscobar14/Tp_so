#ifndef KERNEL_CPU_H_
#define KERNEL_CPU_H_

#include "gestorKernel.h"
#include "pcb.h"
#include "syscalls.h"
#include "planificadorLargoPlazo.h"


void atender_kernel_cpu_dispatch(int *socket_cliente);
void imprimir_modulos_cpu();
void server_escuchar_cpu_dispatch();
t_modulo_cpu* buscar_modulo_cpu_por_identificador(int identificador);
t_modulo_io* buscar_modulo_io_por_nombre(char* nombre_io);
char* recibir_string(int socket);
#endif