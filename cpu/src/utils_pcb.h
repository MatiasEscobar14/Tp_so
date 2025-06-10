/*#ifndef UTILS_PCB_H_
#define UTILS_PCB_H_

//#include "/home/utnso/tp-2025-1c-Linux-Learners/utils/src/utils/protocolo.h"
#include "gestorCPU.h"

void recibir_pcb(t_pcb* pcb, int socket_cliente);
void enviar_pcb(t_pcb* pcb, int socket_cliente);
void deserializar_pcb(t_buffer *buffer, t_pcb* pcb);
t_paquete *crear_paquete_PCB(t_pcb* pcb);
t_buffer *crear_buffer_pcb(t_pcb* pcb);

#endif*/