#include <utils/utils.h>

int main(int argc, char* argv[]){

	//===========================================CONEXION MEMORIA (SERVER) CON CPU (CLIENTE)========================================================//
    char* puerto_escucha;
	
    t_log* logger = iniciar_logger("loggerMemoria.log", "Memoria");


    t_config* config = iniciar_config("memoria.config"); 
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");     //es una prueba
    
    int server_fd =  iniciar_servidor(logger, "memoriaSV","127.0.0.1", puerto_escucha);
    log_info(logger, "Esperando cliente en el puerto %s", puerto_escucha);
    int cliente_fd = esperar_cliente(logger, "memoriaSV", server_fd);
	

   t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
				recibir_mensaje(logger, cliente_fd);
				enviar_mensaje("Hola CPU soy memoria, recibi tu mensaje", cliente_fd, logger);		//falta identificar quien se conecta
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "El cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
	
    return 0;

}
void iterator(char* value) {
	t_log* logger = iniciar_logger("loggerMemoria.log", "Memoria");
    log_info(logger, "%s", value);  // Usamos el logger que se pasa como argumento
}