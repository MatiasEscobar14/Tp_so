#ifndef TLB_H
#define TLB_H

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>
#include </home/utnso/tp-2025-1c-Linux-Learners/utils/src/utils/utils.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef enum {
    FIFO_TLB,
    LRU_TLB
} t_tlb_algoritmo;

typedef struct {
    t_list *entradas;
    uint32_t monto_entradas;
    t_tlb_algoritmo algoritmo;
} t_tlb;

typedef struct {
    uint32_t pid;
    uint32_t pagina;
    uint32_t frame;
    long long last_use;
} t_tlb_fila;

#endif