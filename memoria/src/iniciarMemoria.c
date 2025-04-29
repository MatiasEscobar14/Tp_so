#include "iniciarMemoria.h"


void iniciar_config_memoria(char* ruta){

    memoria_config = config_create(ruta);
	
	if(memoria_config ==  NULL){
		log_info(logger_memoria,  "No se pudo cargar la config.");
		return;
	}

PUERTO_ESCUCHA = config_get_string_value(memoria_config, "PUERTO_ESCUCHA");
TAM_MEMORIA = config_get_int_value(memoria_config, "TAM_MEMORIA");
TAM_PAGINA = config_get_int_value(memoria_config, "TAM_PAGINA");
//ENTRADAS_POR_TABLA
//CANTIDAD_NIVELES
//RETARDO_MEMORIA

log_info(logger_memoria, "Config de Memoria iniciado.");

}

//Falta logger;
//Faltan listas;
//Falta mutex;

//Agregar una funcion que inicie todo: void iniciar_memoria(char* ruta_del_config);

