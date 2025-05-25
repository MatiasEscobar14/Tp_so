#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include "gestorKernel.h"
//#include "pcb.h"
//#include "planificadorLargoPlazo.h"

void crear_proceso_sys(char* nombre_archivo, int tam_proceso);
void dump_memory_sys(int pid);
void syscall_io(t_syscall_io* param);

#endif