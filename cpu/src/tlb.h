/*#ifndef TLB_H
#define TLB_H

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>
#include <utils/utils.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef enum {
    FIFO_TLB,
    LRU_TLB
} algoritmo_tlb;

typedef struct {
    entrada_tlb *entradas;
    int monto_entradas;//valor que dice el config
    int monto_actual; //las que se usan 
    algoritmo_tlb algoritmo;
} t_tlb;

typedef struct {
    int pid;
    int pagina;
    int frame;
    int tiempo_lru;
} entrada_tlb;

void inizializar_tlb();
int buscar_en_tlb(int pid, int pagina);
void reemplazo_algoritmo_FIFO(int pid, int pagina, int frame);
void reemplazo_algoritmo_LRU(int pid, int pagina, int frame);
void actualizar_tlb(int pid, int pagina, int frame);


#endif*/