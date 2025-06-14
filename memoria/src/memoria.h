#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "gestorMemoria.h"
#include "iniciarMemoria.h"
#include "memoria-kernel.h"
#include "/home/utnso/tp-2025-1c-Linux-Learners/memoria/include/gestor.h"
#include "memoria-cpu.h"

extern t_list* lista_procesos;

extern pthread_mutex_t mutex_lista_procesos;

#endif