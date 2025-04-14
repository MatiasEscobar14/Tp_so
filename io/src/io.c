#include <utils/utils.h>

int main(int argc, char* argv[]) {

    //===========================================CONEXION IO (CLIENTE) CON KERNEL (SERVER)========================================================//
    char* ip_kernel;
	char* puerto_kernel;
	
    t_log* logger = iniciar_logger("loggerIo.log", "Io");
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
    log_info(logger, "Mensaje enviado, esperando respuesta del kernel...");
    while (1) {
        int cod_op = recibir_operacion(conexionKernel);  
        switch (cod_op) {
            case MENSAJE:
                recibir_mensaje(logger, conexionKernel);
                break;
            case PAQUETE:
                t_list* paquete = recibir_paquete(conexionKernel);

                break;
            case -1:
                log_error(logger, "Se cerró la conexión con el Kernel");
                return EXIT_FAILURE;
            default:
                log_warning(logger, "Operación desconocida recibida");
                break;
        }
    }
    
   

    

   paquete(conexionKernel);
   
    //saludar("io");







    return 0;
}
