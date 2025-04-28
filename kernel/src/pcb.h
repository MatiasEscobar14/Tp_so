#ifndef PCB_H_
#define PCB_H_

#include "gestorKernel.h"

pcb_t* crear_pcb(char* ruta, int tam_proceso, int prioridad_hilo_main);
void agregar_pcb_lista(pcb_t* pcb, t_list* lista_estado, pthread_mutex_t mutex_lista);
void cambiar_estado(pcb_t* un_pcb, estado_pcb proximo_estado);
#endif
