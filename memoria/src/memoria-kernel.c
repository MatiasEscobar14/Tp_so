#include "memoria-kernel.h"
#include <math.h>
void esperar_kernel()
{
    log_info(logger_memoria, "ESPERANDO COMUNICACION KERNEL");

    while (1)
    {
        int fd_conexion = accept(socket_memoria, NULL, NULL);
        // TODO: Validacion de error?
        log_info(logger_memoria, "## Kernel Conectado - FD: <%d>", fd_conexion);

        pthread_t thread;
        if (pthread_create(&thread, NULL, (void *)attender_memoria_kernel, (void *)(intptr_t) (fd_conexion)))
        {
            log_error(logger_memoria, "Error al crear hilo para Kernel");
            close(fd_conexion); // Cierra el socket si no se pudo crear el hilo.
        }
        else
        {
            pthread_detach(thread);
        }
    }
}

void attender_memoria_kernel(int socket_kernel)
{
    t_buffer *un_buffer;
    log_info(logger_memoria, "Esperando mensaje del kernel");
    int cod_op = recibir_operacion(socket_kernel);

    switch (cod_op)
    {
    case INICIALIZAR_ESTRUCTURAS_KM:
        un_buffer = recv_buffer(socket_kernel);
        inicializar_estructuras(un_buffer, socket_kernel);
        free(un_buffer->stream);
        free(un_buffer);
        break;
    
    case SUSPENSION_PROCESO_KM:
        un_buffer=recv_buffer(socket_kernel);
        suspender_proceso(un_buffer,socket_kernel);
        free(un_buffer->stream);
        free(un_buffer);
    break;
    /*
    case DES_SUSPENSION_PROCESO_KM:
        un_buffer=recv_buffer(socket_kernel);
        des_suspender_proceso(un_buffer,socket_kernel);
        free(un_buffer->stream);
        free(un_buffer);*/
    break;
    /* case FINALIZAR_ESTRUCTURAS_KM:
            un_buffer = recv_buffer(socket_kernel);
         //   finalizar_estructuras(un_buffer, socket_kernel);
            free(un_buffer);
        break;
    case DUMP_MEMORY_KM:
         finalizar_estructuras(un_buffer, socket_kernel);
         free(un_buffer->stream);
         free(un_buffer);
        break;*/
    /*
    case ACCESO_TABLAS_PAGINAS:
        break;
    case ACCESO_A_ESPACIO_USUARIO:
        break;
    //case LEER_PAGINA_COMPLETA:
        break;
    //case ACTUALIZAR_PAGINA_COMPLETA:
        break;     */
   /* case DUMP_MEMORY_KM:
        un_buffer = recv_buffer(socket_kernel);
        dumpear_memoria(un_buffer, socket_kernel);
        free(un_buffer);
        break;*/
    }   
   // free(un_buffer);
}

void inicializar_estructuras(t_buffer *buffer, int socket_kernel)
{

    int pid = extraer_int_buffer(buffer);
    int tamanio = extraer_int_buffer(buffer);

    log_info(logger_memoria, "Solicitud de inicialización PID: %d, tamanio: %d", pid, tamanio);

    t_buffer *otro_buffer = new_buffer();
    if (hay_espacio_disponible(tamanio)){
        log_info(logger_memoria, "Entre al IF (true)");
        crear_proceso(pid, tamanio);
        log_info(logger_memoria, "## Proceso Creado - PID: %d - Tamanio: %d", pid, tamanio);

        add_int_to_buffer(otro_buffer, 1);
    }
    else
    {
        log_info(logger_memoria, "Espacio insuficiente para el proceso - PID: %d", pid);
        add_int_to_buffer(otro_buffer, 0);
    }
    // free(path);
    usleep(RETARDO_MEMORIA *1000);
    t_paquete *rta_kernel = crear_paquete(RTA_INICIALIZAR_ESTRUCTURAS_MK, otro_buffer);
    enviar_paquete(rta_kernel, socket_kernel);
    eliminar_paquete(rta_kernel);
    //free(otro_buffer);
    log_info(logger_memoria, "Envie la rta a KERNEL");
    return;
}

bool hay_espacio_disponible(int tamanio) {
    int paginas_necesarias = tamanio / TAM_PAGINA;

    int marcos_libres = 0;

    for (int i = 0; i < cantidad_marcos; i++) {
        if (!bitmap_marcos[i]) {
            marcos_libres++;
        }
    }
printf("Páginas necesarias: %d, Marcos libres: %d\n", paginas_necesarias, marcos_libres);
    return marcos_libres >= paginas_necesarias;
}

t_proceso* crear_proceso(int pid, int tamanio_proceso)
{
    log_info(logger_memoria, "Entre a Crear Proceso");
    t_proceso* nuevo_proceso = malloc(sizeof(t_proceso));
    nuevo_proceso->pid = pid;
    nuevo_proceso->tamanio = tamanio_proceso;

    int nro_pagina = tamanio_proceso / TAM_PAGINA;
   nuevo_proceso->tabla_1er_nivel = crear_tabla_jerarquica(nro_pagina);
    
    pthread_mutex_lock(&mutex_lista_procesos);
    list_add(lista_procesos, nuevo_proceso);
    pthread_mutex_unlock(&mutex_lista_procesos);


    return nuevo_proceso;
}

tabla_pagina* crear_tabla_jerarquica(int nro_pagina){
    return crear_tabla_recursiva(0,&nro_pagina);
}    

tabla_pagina* crear_tabla_recursiva(int nivel_actual, int* nro_pagina) {
    tabla_pagina* tabla = malloc(sizeof(tabla_pagina));
    tabla->id_tabla = nivel_actual;
    tabla->entradas = list_create();

    for (int i = 0; i < ENTRADAS_POR_TABLA; i++) {
        if (*nro_pagina <= 0)
            break;  // Ya no hace falta seguir creando

        entrada_tabla_pagina* entrada = malloc(sizeof(entrada_tabla_pagina));
        entrada->nro_pagina = i;
        entrada->uso = false;
        entrada->modificado = false;

        if (nivel_actual == CANTIDAD_NIVELES - 1) {
            // Último nivel: asignar marco real
            bool marco_asignado = false;
            for (int j = 0; j < cantidad_marcos; j++) {
                if (!bitmap_marcos[j]) {
                    bitmap_marcos[j] = true;
                    entrada->numero_marco = j;
                    entrada->presente = true;
                    marco_asignado = true;
                    break;
                }
            }

            if (!marco_asignado) {
                log_warning(logger_memoria, "No se pudo asignar marco físico");
                entrada->numero_marco = -1;
                entrada->presente = false;
            }

            entrada->siguiente_nivel = NULL;
            (*nro_pagina)--;  // Se asignó una página

        } else {
            // Nivel intermedio: crear subtabla
            entrada->numero_marco = -1;
            entrada->presente = true;
            entrada->siguiente_nivel = crear_tabla_recursiva(nivel_actual + 1, nro_pagina);
        }

        list_add(tabla->entradas, entrada);
    }

    return tabla;
}
void suspender_proceso(t_buffer*buffer,int socket_kernel){
    int pid = extraer_int_buffer(buffer);

    log_info(logger_memoria, "Solicitud de suspender proceso PID %d",pid);

    pthread_mutex_lock(&mutex_lista_procesos);
    t_proceso* proceso = buscar_proceso_por_pid(pid);

    if(proceso!=NULL){
        liberar_espacio_memoria(proceso);
        log_info(logger_memoria, "Proceso PID %d suspendido correctamente",pid);

    }else{
        log_warning(logger_memoria, "No se encontro el proceso PID %d",pid);
    }
    pthread_mutex_unlock(&mutex_lista_procesos);

    t_buffer* respuesta = new_buffer();
    add_int_to_buffer(respuesta, 1); // éxito

    t_paquete* paquete = crear_paquete(RTA_SUSPENSION_PROCESO_MK, respuesta);
    enviar_paquete(paquete, socket_kernel);
    log_info(logger_memoria, "RTA enviada al Kernel sobre suspensión PID %d", pid);

    eliminar_paquete(paquete);
}

t_proceso* buscar_proceso_por_pid(int pid) {
    for (int i = 0; i < list_size(lista_procesos); i++) {
        t_proceso* proceso = list_get(lista_procesos, i);
        if (proceso->pid == pid) {
            return proceso;
        }
    }
    return NULL;
}

void liberar_espacio_memoria(t_proceso* proceso) {
    liberar_marcos_y_guardar_en_swap(proceso->tabla_1er_nivel, proceso->pid, 0, 0);
}
void liberar_marcos_y_guardar_en_swap(tabla_pagina* tabla, int pid, int nivel, int nro_pagina_base) {
    for (int i = 0; i < list_size(tabla->entradas); i++) {
        entrada_tabla_pagina* entrada = list_get(tabla->entradas, i);

        if (nivel == CANTIDAD_NIVELES - 1) {
            if (entrada->presente && entrada->numero_marco != -1) {
                void* marco_ptr = espacio_usuario + (entrada->numero_marco * TAM_PAGINA);

                int nro_pagina = nro_pagina_base + i;
                escribir_en_swap(pid, nro_pagina, marco_ptr);

                bitmap_marcos[entrada->numero_marco] = false;
                entrada->presente = false;
                entrada->numero_marco = -1;

                log_info(logger_memoria, "Liberado marco para PID %d página %d", pid, nro_pagina);
            }
        } else {
            liberar_marcos_y_guardar_en_swap(entrada->siguiente_nivel,
                                              pid, nivel + 1,
                                              nro_pagina_base * ENTRADAS_POR_TABLA + i);
        }
    }
}
void des_suspender_proceso(un_buffer,socket_kernel){
    int pid = extraer_int_buffer(un_buffer);
    log_info(logger_memoria, "Solicitud de des-suspender proceso PID %d",pid);

      pthread_mutex_lock(&mutex_lista_procesos);
    t_proceso* proceso = buscar_proceso_por_pid(pid);
    pthread_mutex_unlock(&mutex_lista_procesos);

    t_buffer* buffer_respuesta = new_buffer();
    if (proceso == NULL) {
        log_warning(logger_memoria, "Proceso PID %d no encontrado", pid);
        add_int_to_buffer(buffer_respuesta, 0);
    } else if (!hay_espacio_disponible(proceso->tamanio)) {
        log_warning(logger_memoria, "No hay espacio para des-suspender PID %d", pid);
        add_int_to_buffer(buffer_respuesta, 0);
    } else {
        int nro_pagina = 0;
        reasignar_y_cargar_paginas(proceso->tabla_1er_nivel, pid, 0, &nro_pagina);
        log_info(logger_memoria, "Proceso PID %d des-suspendido exitosamente", pid);
        add_int_to_buffer(buffer_respuesta, 1);
    }

    usleep(RETARDO_MEMORIA * 1000);
    t_paquete* paquete = crear_paquete(RTA_DESUSPENSION_PROCESO_MK, buffer_respuesta);
    enviar_paquete(paquete, socket_kernel);
    eliminar_paquete(paquete);
}
void reasignar_y_cargar_paginas(tabla_pagina* tabla, int pid, int nivel, int* nro_pagina) {
    for (int i = 0; i < list_size(tabla->entradas); i++) {
        entrada_tabla_pagina* entrada = list_get(tabla->entradas, i);

        if (nivel == CANTIDAD_NIVELES - 1) {
            // Asignar marco
            for (int j = 0; j < cantidad_marcos; j++) {
                if (!bitmap_marcos[j]) {
                    bitmap_marcos[j] = true;
                    entrada->numero_marco = j;
                    entrada->presente = true;

                    void* marco_ptr = espacio_usuario + (j * TAM_PAGINA);
                    leer_de_swap(pid, *nro_pagina, marco_ptr);  // lee del archivo
                    liberar_pagina_swap(pid, *nro_pagina);     // limpia en swap

                    log_info(logger_memoria, "PID %d página %d -> marco %d", pid, *nro_pagina, j);
                    break;
                }
            }
            (*nro_pagina)++;
        } else {
            reasignar_y_cargar_paginas(entrada->siguiente_nivel, pid, nivel + 1, nro_pagina);
        }
    }
}
*/
/*
void finalizar_estructuras(t_buffer* buffer, int socket_kernel) {
    int desplazamiento = 0;
    int pid = leer_int_de_buffer(buffer, &desplazamiento);

    log_info(logger_memoria, "Finalizando estructuras de proceso PID %d", pid);
    

    //liberar_tabla_de_paginas(pid);
    //liberar_swap(pid);
    //eliminar_metricas(pid);

    liberar_tabla_de_paginas(pid);
    liberar_swap(pid);
    loguear_metricas_proceso(pid);
    eliminar_metricas(pid);
    
    // Enviar confirmación al Kernel
    t_buffer* a_enviar = new_buffer();
    add_int_to_buffer(respuesta->buffer, 1); 
    t_paquete* respuesta = crear_paquete(RTA_ESTRUCTURA_LIBERADA_KM, a_enviar);
    enviar_paquete(respuesta, socket_kernel);
    eliminar_paquete(respuesta);
}
*/
int buscar_valor_pid;
/*
bool buscar_pid(t_proceso* un_proceso){
    return un_proceso->pid == buscar_valor_pid;
}

t_proceso* buscar_proceso_pid(int pid){
    buscar_valor_pid = pid;
    pthread_mutex_lock(&mutex_lista_procesos);
    t_proceso* un_proceso = list_find(lista_procesos, (void*)buscar_pid);
    pthread_mutex_unlock(&mutex_lista_procesos);
    if(un_proceso == NULL){
        log_info(logger_memoria, "PID - (%d) no fue encontrado en la lista de procesos", pid);
        exit(EXIT_FAILURE);
    }
    return un_proceso;
}


}*/
/*
void liberar_tabla_de_paginas(tabla_pagina* tabla) {
    for (int i = 0; i < list_size(tabla->entradas); i++) {
        entrada_tabla_pagina* entrada = list_get(tabla->entradas, i);
        if (entrada->siguiente_nivel != NULL) {
            liberar_tabla_de_paginas(entrada->siguiente_nivel);
        }
        if (entrada->presente && entrada->numero_marco != -1) {
            liberar_marco(entrada->numero_marco);
        }
        free(entrada);
    }
    list_destroy(tabla->entradas);
    free(tabla);
}
void eliminar_metricas(uint32_t pid) {
    int i = 0;
    while (i < list_size(lista_metricas)) {
        t_metricas_proceso* m = list_get(lista_metricas, i);
        if (m->pid == pid) {
            list_remove_and_destroy_element(lista_metricas, i, free);
            // No incrementamos i porque la lista se achica
        } else {
            i++; // Solo avanzo si no borré nada
        }
    }
}
*/
/*
void liberar_swap(int pid) {
    // Por ahora, no hace nada
}
*/
/*
void dumpear_memoria(t_buffer* un_buffer, int socket){
    int pid = extraer_int_buffer(un_buffer);
    log_info(logger_memoria, "## Memory Dump solicitado - (PID) - (%d)", pid);
    t_proceso* un_proceso = buscar_proceso_pid(pid);
    if (un_proceso == NULL) {
        log_warning(logger_memoria, "PID %d no encontrado en memoria", pid);
        return;
    }

    // Crear nombre del archivo con timestamp
    time_t t = time(NULL);
    char path_dump[256];
    snprintf(path_dump, sizeof(path_dump), "%s/%d-%ld.dmp", config_memoria->dump_path, pid, t);

    FILE* archivo = fopen(path_dump, "wb");
    if (!archivo) {
        log_error(logger_memoria, "No se pudo crear el archivo dump: %s", path_dump);
        return;
    }

    // Escribir en el dump las páginas reales del proceso en memoria principal
    for (int i = 0; i < cantidad_marcos; i++) {
        if (marcos[i].pid == pid) {
            void* inicio = espacio_usuario + (i * TAM_PAGINA);
            fwrite(inicio, TAM_PAGINA, 1, archivo);
        }
    }

    fclose(archivo);
    log_info(logger_memoria, "Memory Dump generado correctamente en: %s", path_dump);
}
//Devolver un valor fijo de espacio libre (mock)

int suma_tamanios = 0;

int valor_memoria_disponible(){

    suma_tamanios = 0;
    pthread_mutex_lock(&mutex_lista_procesos);
    list_iterate(lista_procesos, acumular_tamanio);
    pthread_mutex_unlock(&mutex_lista_procesos);
    return suma_tamanios;
}

void acumular_tamanio(void* elemento) {
    t_proceso* proceso = (t_proceso*) elemento;
    suma_tamanios += proceso->tamanio;
}
*/