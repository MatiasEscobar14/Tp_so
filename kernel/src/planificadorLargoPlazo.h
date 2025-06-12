#ifndef PLANIFICADOR_LARGO_PLAZO_H_
#define PLANIFICADOR_LARGO_PLAZO_H_

#include <utils/utils.h>
#include "gestorKernel.h"
#include "pcb.h"
#include "kernel_memoria.h"

void iniciar_plp();
void planificadorLargoPlazo();
void planificadorLargoPlazoPMCP();
void planificadorLargoPlazoFifo();
void* esperar_enter(void* arg);
int comparar_pcb_por_tamanio(const void* a, const void* b);
void finalizar_proceso(int pid);



#endif