#include "iniciarMemoria.h"

void iniciar_logger_memoria(){
    logger_memoria = log_create("memoria.log","Memoria",1,LOG_LEVEL_INFO);
}
void iniciar_config_memoria(char* ruta){
	
    memoria_config = config_create(ruta);
	
	if(memoria_config ==  NULL){
		log_info(logger_memoria,  "No se pudo cargar la config.");
		return;
	}

PUERTO_ESCUCHA = config_get_string_value(memoria_config, "PUERTO_ESCUCHA");
TAM_MEMORIA = config_get_int_value(memoria_config, "TAM_MEMORIA");
TAM_PAGINA = config_get_int_value(memoria_config, "TAM_PAGINA");
ENTRADAS_POR_TABLA = config_get_int_value(memoria_config, "ENTRADAS_POR_TABLA");
CANTIDAD_NIVELES = config_get_int_value(memoria_config, "CANTIDAD_NIVELES");
RETARDO_MEMORIA = config_get_int_value(memoria_config, "RETARDO_MEMORIA");
PATH_SWAPFILE = config_get_string_value(memoria_config,"PATH_SWAPFILE");
//DUMP_PATH = config_get_int_value(memoria_config,"DUMP_PATH");
RETARDO_SWAP = config_get_int_value(memoria_config, "RETARDO_SWAP");
PATH_INSTRUCCIONES =config_get_string_value(memoria_config, "PATH_INSTRUCCIONES");

log_info(logger_memoria, "Config de Memoria iniciado.");

}


void iniciar_memoria(char* ruta_config){
	iniciar_logger_memoria();
    iniciar_config_memoria(ruta_config);
	iniciar_listas();
	reservar_memoria_usuario();
	initialize_semaphores();
	iniciar_swap();
	socket_memoria = iniciar_servidor(logger_memoria, "Memoria Server", PUERTO_ESCUCHA);
	log_info(logger_memoria, "Esperando cliente en el puerto %s", PUERTO_ESCUCHA);
}
void initialize_semaphores(){

}
void iniciar_listas(){
	lista_procesos = list_create();
    procesos_en_swap = list_create();
}

pthread_mutex_t mutex_lista_modulos_io_conectadas = PTHREAD_MUTEX_INITIALIZER;

void iniciar_mutex(){
    pthread_mutex_init(&mutex_lista_procesos, NULL);
	pthread_mutex_init(&mutex_procesos,NULL);
}

void* memoria_principal;
uint32_t cantidad_marcos;
bool* bitmap_marcos;
void* espacio_usuario;
void reservar_memoria_usuario() {
    espacio_usuario = malloc(TAM_MEMORIA);
    if (espacio_usuario == NULL) {
        log_error(logger_memoria, "No se pudo reservar el espacio de memoria de usuario");
        exit(EXIT_FAILURE);
    }
    memset(espacio_usuario, 0, TAM_MEMORIA);
    log_info(logger_memoria, "Espacio de memoria de usuario reservado: %d bytes", TAM_MEMORIA);
	
	cantidad_marcos= TAM_MEMORIA/TAM_PAGINA;
    bitmap_marcos = calloc(cantidad_marcos, sizeof(bool)); // false = libre
    if (bitmap_marcos == NULL) {
        log_error(logger_memoria, "No se pudo reservar el bitmap de marcos");
        exit(EXIT_FAILURE);
    }

    log_info(logger_memoria, "Bitmap de marcos inicializado con %d marcos", cantidad_marcos);
}

void iniciar_swap(){
    FILE * archivo = fopen(PATH_SWAPFILE, "w+b");
   if (archivo == NULL) {
    perror("fopen swapfile");
    log_error(logger_memoria, "no se pudo abrir el archivo SWAP");
    exit(EXIT_FAILURE);
}
    fclose(archivo);
}
/*
void escribir_en_swap(int pid, int nro_pagina, void* contenido) {
    FILE* archivo = fopen(PATH_SWAPFILE, "r+b");  // abrir para escritura binaria
    if (!archivo) {
        log_error(logger_memoria, "No se pudo abrir swapfile para escritura");
        return;
    }

    int offset = calcular_offset_swap(pid, nro_pagina);

    fseek(archivo, offset, SEEK_SET);
    fwrite(contenido, TAM_PAGINA, 1, archivo);
    fclose(archivo);

    log_info(logger_memoria, "Se escribió página %d del PID %d en swap (offset %d)",
             nro_pagina, pid, offset);
}
void leer_de_swap(int pid, int nro_pagina, void* destino) {
    FILE* archivo = fopen(PATH_SWAPFILE, "rb");
    if (!archivo) {
        log_error(logger_memoria, "No se pudo abrir swapfile para lectura");
        return;
    }

    int offset = calcular_offset_swap(pid, nro_pagina);
    fseek(archivo, offset, SEEK_SET);
    fread(destino, TAM_PAGINA, 1, archivo);
    fclose(archivo);

    log_info(logger_memoria, "Leída página %d del PID %d desde swap (offset %d)",
             nro_pagina, pid, offset);
}
void liberar_pagina_swap(int pid, int nro_pagina) {
    // Opcional: podés limpiar el contenido con 0s, si querés
    FILE* archivo = fopen(PATH_SWAPFILE, "r+b");
    if (!archivo) {
        log_error(logger_memoria, "No se pudo abrir swapfile para liberar página");
        return;
    }

    int offset = calcular_offset_swap(pid, nro_pagina);
    void* buffer_vacio = calloc(1, TAM_PAGINA);  // página vacía
    fseek(archivo, offset, SEEK_SET);
    fwrite(buffer_vacio, TAM_PAGINA, 1, archivo);
    fclose(archivo);
    free(buffer_vacio);

    log_info(logger_memoria, "Liberada página %d del PID %d en swap", nro_pagina, pid);
}
int calcular_offset_swap(int pid, int nro_pagina) {
    // Suponiendo que cada PID tiene una zona contigua de páginas.
    int paginas_por_proceso = 1000; // ajustalo si sabés el máximo
    return (pid * paginas_por_proceso + nro_pagina) * TAM_PAGINA;
}
*/