#include </home/utnso/tp-2025-1c-Linux-Learners/cpu/src/tlb.h>

static t_tlb TLB;

void tlb_init(int monto_entradas, char *tlb_algoritmo)
{
    TLB.entradas = list_create();
    TLB.monto_entradas = monto_entradas;
    if (!strcmp("FIFO", tlb_algoritmo))
    {
        TLB.algoritmo = FIFO_TLB;
    }
    else
    {
        TLB.algoritmo = LRU_TLB;
    }
}


int tlb_search(uint32_t pid, uint32_t pagina, t_log *logger)
{

    if (0 == TLB.monto_entradas)
    {
        // creo que no hay que loggear nada, ya que esto es equivalente a no tener tlb
        //log_info(logger, "PID: %d - TLB MISS - Pagina: %d", pid, page);
        return -1;
    }

    bool find_entry(void *elem)
    {
        t_tlb_fila *temp = (t_tlb_row *)elem;
        return pid == temp->pid && page == temp->page;
    }

    t_tlb_fila *entrada = (t_tlb_row *)list_find(TLB.entries, find_entry);

    if (!entrada)
    {
        log_info(logger, "PID: %d - TLB MISS - Pagina: %d", pid, pagina);
        return -1;
    }

    log_info(logger, "PID: %d - TLB HIT - Pagina: %d", pid, pagina);

    if (TLB.algorithm == LRU_TLB)
    {
        entry->last_use =  current_timestamp();
    }

    return entry->frame;
}