#include <utils/utils.h>

int main(int argc, char* argv[]) {
    char* ip;
	
    t_log* logger = iniciar_logger("loggerCPU.log", "Cpu");
    log_info(logger, "Hola mundo");

    t_config* config = iniciar_config("cpu.config"); 
    
    ip = config_get_string_value(config, "IP_MEMORIA");
    log_info(logger, "IP: %s", ip);
    //saludar("cpu");
    return 0;
}
