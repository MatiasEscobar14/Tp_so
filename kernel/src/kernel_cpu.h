#ifndef KERNEL_CPU_H_
#define KERNEL_CPU_H_

#include "gestorKernel.h"
#include "pcb.h"
#include "syscalls.h"
#include "planificadorLargoPlazo.h"


void atender_kernel_cpu_dispatch(int *socket_cliente);
void imprimir_modulos_cpu();
void server_escuchar_cpu_dispatch();
#endif