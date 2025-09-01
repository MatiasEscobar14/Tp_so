#ifndef INICIAR_MEMORIA_H_
#define INICIAR_MEMORIA_H_


#include "gestorMemoria.h"

void iniciar_config_memoria(char* ruta);
void iniciar_logger_memoria();
void iniciar_mutex();
void iniciar_listas();

void initialize_semaphores();
void iniciar_memoria(char* ruta_config);
void iniciar_logger_memoria();
void reservar_memoria_usuario(); 
void iniciar_marcos_paginacion();
t_marco* marco_create(int base, bool libre, int pid, int tamanio);
void iniciar_swap();
void escribir_en_swap(void* contenido);
extern void* espacio_usuario;

#endif