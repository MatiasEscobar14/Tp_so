#include "conexiones_io.h"

int crear_conexiones_io(t_log* logger, t_config* config){

	char* ip_kernel;
	char* puerto_kernel;

    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    
    int conexionKernel = crear_conexion(logger, "Servidor Kernel", ip_kernel, puerto_kernel);

	if (conexionKernel == -1) {
		log_error(logger, "No se pudo establecer conexión con el servidor");
		terminar_programa(conexionKernel, logger, config);
	}
	return conexionKernel;
}




