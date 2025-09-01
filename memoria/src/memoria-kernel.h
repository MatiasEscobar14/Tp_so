#ifndef MEMORIA_KERNEL_H
#define MEMORIA_KERNEL_H

#include "gestorMemoria.h"
#include "../include/gestor.h"


void attender_memoria_kernel(int socket_kernel);
void inicializar_estructuras(t_buffer* buffer, int socket_kernel);
bool hay_espacio_disponible(int size_proceso);
void asignar_memoria_al_proceso(t_proceso* proceso, int tamanio); 
void suspender_proceso(t_buffer*buffer,int socket_kernel);
void liberar_espacio_memoria(t_proceso* proceso);
void liberar_tabla_recursiva(tabla_pagina* tabla, int pid); 
void finalizar_estructuras(t_buffer* buffer, int socket_kernel);
void esperar_kernel();
void dumpear_memoria(t_buffer* un_buffer,int socket);

t_proceso* crear_proceso(int pid, int tamanio_proceso);
bool buscar_pid(t_proceso* un_proceso);
//t_proceso* buscar_proceso_pid(int pid);
t_proceso* buscar_proceso_por_pid(int pid);

tabla_pagina* crear_tabla_jerarquica(int nro_pagina);
tabla_pagina* crear_tabla_recursiva(int nivel_actual, int* nro_pagina);


#endif 