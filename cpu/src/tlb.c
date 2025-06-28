/*#include "tlb.h"

t_tlb tlb;

void inizializar_tlb()
{
    tlb = (t_tlb *)malloc(sizeof(t_tlb));
    tlb->entradas = (entrada_tlb *)malloc(sizeof(entrada_tlb) * ENTRADAS_TLB);
    tlb->monto_entradas = ENTRADAS_TLB;
    tlb->monto_actual = 0

    if (strcmp(REEMPLAZO_TLB, "FIFO") == 0)
    {
        tlb->algoritmo = FIFO_TLB;
    }
    else
    {
        tlb->algoritmo = LRU_TLB;
    }
    
    return tlb;
}

int buscar_en_tlb(int pid, int pagina) {

    time_t tiempo_actual = time(NULL); // calcula cual es el tiempo que hace que fue usado por ultima vez

    for (int i = 0; i < tlb->monto_actual; i++)
    {
        if (tlb->entradas[i].pid == pid && tlb->entradas[i].pagina == pagina)
        {
            tlb->entradas[i].tiempo_lru = (int) tiempo_actual;
            return tlb->entradas[i].frame; // TLB-HIT
        }
    }
    return -1; // TLB-MISS

}

// Reemplazo por FIFO
void reemplazo_algoritmo_FIFO(int pid, int pagina, int frame)
{
    for (int i = 1; i < tlb->monto_actual; i++)
    {
        tlb->entradas[i - 1] = tlb->entradas[i];
    }

    tlb->entradas[tlb->monto_actual - 1].pid = pid;
    tlb->entradas[tlb->monto_actual - 1].pagina = pagina;
    tlb->entradas[tlb->monto_actual - 1].frame = frame;
}

// Reemplazo por LRU
void reemplazo_algoritmo_LRU(int pid, int pagina, int frame)
{
    int lruIndice = 0;
    time_t tiempo_actual = time(NULL);
    for (int i = 1; i < tlb->monto_actual; i++)
    {
        if (tlb->entradas[i].tiempo_lru < tlb->entradas[lruIndice].tiempo_lru)
        {
            lruIndice = i;
        }
    }

    tlb->entradas[lruIndice].pid = pid;
    tlb->entradas[lruIndice].pagina = pagina;
    tlb->entradas[lruIndice].frame = frame;
    tlb->entradas[lruIndice].tiempo_lru = tiempo_actual;
}

void actualizar_tlb(int pid, int pagina, int frame)
{
    time_t tiempo_actual = time(NULL);
    if (tlb->monto_actual < tlb->monto_entradas)
    {
        tlb->entradas[tlb->monto_actual].pid = pid;
        tlb->entradas[tlb->monto_actual].pagina = pagina;
        tlb->entradas[tlb->monto_actual].frame = frame;
        if (tlb->algoritmo == LRU)
        {
            tlb->entradas[tlb->monto_actual].tiempo_lru = (int) tiempo_actual;
        }
        tlb->monto_actual++;
    }
    else
    {
        if (tlb->algoritmo == FIFO)
        {
            reemplazo_algoritmo_FIFO(pid, pagina, frame);
        }
        else
        {
            reemplazo_algoritmo_LRU(pid, pagina, frame);
        }
    }
}



}*/