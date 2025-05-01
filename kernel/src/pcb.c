#include "pcb.h"


t_pcb* crear_pcb(/*char* ruta,*/ int tam_proceso){
    t_pcb* nueva_pcb = malloc(sizeof(t_pcb));

    static int pid_counter = 0;           
    nueva_pcb->pid = pid_counter++;
    
    nueva_pcb->pc = 0;                
    nueva_pcb->tamanio_proceso = tam_proceso;
    nueva_pcb->estado = NEW_PROCCES;
    nueva_pcb->tiempo_inicio_estado = time(NULL);

    //nueva_pcb->path = strdup(ruta); // TODO Duda: Deberiamos incluir un atributo "PATH" en el struct de PCB???   Guardás el path asociado al proceso  

    // Inicializar métricas, son 7 estados y 7 tiempos

    for (int i = 0; i < CANT_ESTADOS; i++) {
        nueva_pcb->metricas_estado[i] = 0;
        nueva_pcb->metricas_tiempo[i] = 0.0;
    }
    nueva_pcb->metricas_estado[NEW_PROCCES] = 1; 
    
    return nueva_pcb;
}

void agregar_pcb_lista(t_pcb* pcb, t_list* lista_estado, pthread_mutex_t mutex_lista){
	pthread_mutex_lock(&mutex_lista);
	list_add(lista_estado, pcb);
	pthread_mutex_unlock(&mutex_lista);
}

void cambiar_estado(t_pcb* un_pcb, estado_pcb proximo_estado) {

    double tiempo_en_estado = difftime(time(NULL), un_pcb->tiempo_inicio_estado);
    un_pcb->metricas_tiempo[un_pcb->estado] += tiempo_en_estado;
    log_info("## (%d) Pasa del estado %s al estado %s", un_pcb->pid,estado_a_string(un_pcb->estado),estado_a_string(proximo_estado));
    un_pcb->estado = proximo_estado;
    un_pcb->tiempo_inicio_estado = time(NULL);
    un_pcb->metricas_estado[proximo_estado]++;
}

const char* estado_a_string(estado_pcb estado) {
    switch(estado) {
        case NEW_PROCCES:         return "NEW_PROCCES";
        case READY_PROCCES:       return "READY_PROCCES";
        case EXEC_PROCCES:        return "EXEC_PROCCES";
        case BLOCKED_PROCCES:     return "BLOCKED_PROCCES";
        case EXIT_PROCCES:        return "EXIT_PROCCES";
        case SUSP_BLOCKED_PROCESS:return "SUSP_BLOCKED_PROCESS";
        case SUSP_READY_PROCESS: return "SUSP_READY_PROCESS";
        default:                  return "ESTADO_DESCONOCIDO";
    }
}



