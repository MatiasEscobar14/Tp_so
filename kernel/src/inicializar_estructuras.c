#include "inicializar_estructuras.h"

	t_list* lista_new;
	t_list* lista_ready;
	t_list* lista_execute;
	t_list* lista_blocked;
	t_list* lista_exit;
	t_list* lista_susp_blocked;
	t_list* lista_susp_ready;
	
void iniciar_lista(){

    //LISTAS PARA LOS PROCESOS
	lista_new = list_create();
	lista_ready = list_create();
	lista_execute = list_create();
	lista_blocked = list_create();
	lista_exit = list_create();
    lista_susp_blocked = list_create();
    lista_susp_ready = list_create();
	//LISTAS PARA LOS HILOS
	//lista_new_thread = list_create();
	//lista_ready_thread = list_create();
	//lista_execute_thread = list_create();
	//lista_blocked_thread= list_create();
	//lista_exit_thread = list_create();
	//lista_mutex_thread = list_create();
}