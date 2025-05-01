#ifndef MEMORIA_KERNEL_H
#define MEMORIA_KERNEL_H

#include <utils/utils.h>
#include "gestorMemoria.h"

void attender_memoria_kernel(int socket_kernel);
void inicializar_estructuras(t_buffer* buffer, int socket_kernel);
bool hay_espacio_disponible(int size_proceso);

#endif 