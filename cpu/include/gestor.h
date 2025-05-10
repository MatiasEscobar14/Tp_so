#ifndef GESTOR_H_
#define GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <signal.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/memory.h>
#include <readline/readline.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <signal.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/memory.h>
#include <readline/readline.h>

#include </home/utnso/tp-2025-1c-Linux-Learners/utils/src/utils/protocolo.h>
#include </home/utnso/tp-2025-1c-Linux-Learners/utils/src/utils/utils.h>

extern int cliente_de_memoria;
extern int cliente_de_kernel_interrupt;
extern int cliente_de_kernel_dispatch;
extern t_log* logger;
extern pthread_mutex_t mutex_pcb_actual;
extern pthread_mutex_t mutex_interrupt;

#endif 