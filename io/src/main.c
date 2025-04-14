#include <utils/utils.h>

int main(int argc, char* argv[]) {
    char* ip_kernel;
	char* puerto_kernel;
	
    t_log* logger = iniciar_logger("loggerIo.log");
    log_info(logger, "Hola mundo");

    t_config* config = iniciar_config("io.config"); 
    
    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    
    int conexionKernel = crear_conexion(logger, "kernelServer", ip_kernel, puerto_kernel);

	if (conexionKernel == -1) {
		log_error(logger, "No se pudo establecer conexión con el servidor");
		terminar_programa(conexionKernel, logger, config);
		return EXIT_FAILURE;
	}

	enviar_mensaje("Hola kernel desde IO", conexionKernel,logger);   //prueba
    
    recibir_mensaje(logger, conexionKernel);
    

   paquete(conexionKernel);
   
    //saludar("io");







    return 0;
}
