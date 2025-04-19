#include "kernel.h"


int main(int argc, char* argv[]) {

	//===========================================CONEXION KERNEL (CLIENTE) CON MEMORIA (SERVER)========================================================//

	t_log* logger = iniciar_logger("kernel.log", "Kernel");
    t_config* config = iniciar_config("kernel.config"); 

 	crear_conexiones(logger, config);
	
	return 0;

}
	