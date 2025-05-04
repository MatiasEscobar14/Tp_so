#ifndef KERNEL_H_
#define KERNEL_H

#include "conexionesKernel.h"
#include "planificadorLargoPlazo.h"
#include "gestorKernel.h"
#include "pcb.h"
#include "inicializar_estructuras.h"

void crear_proceso_inicial(int tamanio_proceso, char* ruta);

#endif