
/*#include "utils_pcb.h"

void recibir_pcb(t_pcb *pcb, int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_pcb(paquete->buffer, pcb); //(6)
    eliminar_paquete(paquete);
}

void enviar_pcb(t_pcb *pcb, int socket_cliente)
{
    t_paquete *paquete = crear_paquete_PCB(pcb); //(7)
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void deserializar_pcb(t_buffer *buffer, t_pcb *pcb)
{
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(pcb->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb->quantum), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->tiempo_q), stream + desplazamiento, sizeof(uint64_t));
    desplazamiento += sizeof(uint64_t);

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
}

t_paquete *crear_paquete_PCB(t_pcb *pcb)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = PCB;
    paquete->buffer = crear_buffer_pcb(pcb); //(8)
    return paquete;
}

//(DEPENDE COMO HAYAN ARMADO EL PCB, VAS A TENER QUE REVISARLO)

t_buffer *crear_buffer_pcb(t_pcb *pcb)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    buffer->size = sizeof(uint32_t) +
                   sizeof(t_estado_proceso) +
                   sizeof(uint32_t) +
                   sizeof(uint64_t) +
                   tam_registros +
                   sizeof(t_motivo_desalojo) +
                   sizeof(t_motivo_finalizacion);

    buffer->stream = malloc(buffer->size);
    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(pcb->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb->estado), sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(buffer->stream + desplazamiento, &(pcb->quantum), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb->tiempo_q), sizeof(uint64_t));
    desplazamiento += sizeof(uint64_t);

    memcpy(buffer->stream + desplazamiento, pcb->contexto_ejecucion->registros, tam_registros);
    desplazamiento += tam_registros;

    memcpy(buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_finalizacion), sizeof(t_motivo_finalizacion));

    return buffer;
}*/