<<<<<<< HEAD
#ifndef  GESTOR_MEMORIA_H
#define GESTOR_MEMORIA_H

extern t_log* memoria_logger;
extern t_config* memoria_config;

extern int socket_memoria;



#endif
=======
#ifndef GESTOR_MEMORIA_H
#define GESTOR_MEMORIA_H

#include <utils/utils.h>

extern t_log* logger_memoria;
extern t_config* memoria_config;

extern char* PUERTO_ESCUCHA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
//NTRADAS_POR_TABL;
//CANTIDAD_NIVELES;
//RETARDO_MEMORIA;

extern int socket_memoria;
extern int memoria_usada;


#endif 
>>>>>>> c047ab4210c130883fab0dfce669ec9e94bab8c5
