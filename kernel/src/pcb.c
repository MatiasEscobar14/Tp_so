#include "pcb.h"


pcb_t* crear_pcb(char* ruta, int tam_proceso, int prioridad_hilo_main){
    pcb_t* nueva_pcb = malloc(sizeof(pcb_t));

    static int pid_counter = 0;            // Este se incrementa con cada PCB creado
    nueva_pcb->pid = pid_counter++;
    
    nueva_pcb->pc = 0;                  // El Program Counter arranca en 0
    nueva_pcb->tamanio = tam_proceso;

    nueva_pcb->path = strdup(ruta); // TODO Duda: Deberiamos incluir un atributo "PATH" en el struct de PCB???   Guardás el path asociado al proceso  

    // Inicializar métricas
    nueva_pcb->metricas_estado = calloc(5, sizeof(int));      // Para contar cuántas veces pasó por cada estado
    nueva_pcb->tiempo_estado = calloc(5, sizeof(double));     // Para contar cuánto tiempo pasó en cada estado

    return nueva_pcb;
}
