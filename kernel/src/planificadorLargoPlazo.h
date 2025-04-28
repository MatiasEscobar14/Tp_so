#ifndef PLANIFICADOR_LARGO_PLAZO_H_
#define PLANIFICADOR_LARGO_PLAZO_H_

#include <utils/utils.h>
#include "gestorKernel.h"
#include "pcb.h"

void iniciar_plp();
void planificadorLargoPlazo();
void* esperar_enter(void* arg);



#endif