#include <utils/utils.h>

int main(int argc, char* argv[]) {


    //===========================================CONEXION CPU (CLIENTE) CON MEMORIA (SERVER)========================================================//
    char* ip_memoria;
    char* puerto_memoria;
	
    t_log* logger = iniciar_logger("loggerCPU.log", "Cpu");

    t_config* config = iniciar_config("cpu.config"); 
    
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria= config_get_string_value(config, "PUERTO_MEMORIA");


    int conexionCpuMemoria = crear_conexion(logger, "memoriaServer", ip_memoria, puerto_memoria);

	if (conexionCpuMemoria == -1) {
		log_error(logger, "No se pudo establecer conexión con el servidor");
		terminar_programa(conexionCpuMemoria, logger, config);
		return EXIT_FAILURE;
	}

    enviar_mensaje("Hola memoria desde CPU", conexionCpuMemoria,logger);   //prueba
    log_info(logger, "Mensaje enviado, esperando respuesta del memoria...");
    while (1) {
        int cod_op = recibir_operacion(conexionCpuMemoria);  
        switch (cod_op) {
            case MENSAJE:
                recibir_mensaje(logger, conexionCpuMemoria);
                break;
            case PAQUETE:
                t_list* paquete = recibir_paquete(conexionCpuMemoria);

                break;
            case -1:
                log_error(logger, "Se cerró la conexión con memoria");
                return EXIT_FAILURE;
            default:
                log_warning(logger, "Operación desconocida recibida");
                break;
        }
    }

   paquete(conexionCpuMemoria);



    //saludar("cpu");
    return 0;
}
