#ifndef PCB_H_
#define PCB_H_

#include "gestorKernel.h"

t_pcb* crear_pcb(char* ruta, int tam_proceso);
void agregar_pcb_lista(t_pcb* pcb, t_list* lista_estado, pthread_mutex_t mutex_lista);

void cambiar_estado(t_pcb* un_pcb, estado_pcb proximo_estado);
const char* estado_a_string(estado_pcb estado);

//bool buscar_pcb(void *elemento);
bool __buscar_pcb(void* void_pcb);
//bool __buscar_pcb(t_pcb* void_pcb);
t_pcb* buscar_y_remover_pcb_por_pid(int un_pid);
t_pcb* buscar_pcb_por_pid(int un_pid);
t_pcb* buscar_pcb_por_socket(int socket_cpu);

void enviar_pcb_a_cpu(t_pcb* un_pcb);
void bloquear_proceso_syscall(int pid);
void enviar_pcb_a_modulo_io(t_modulo_io* modulo, t_pcb* pcb, int tiempo_ms);

#endif 
