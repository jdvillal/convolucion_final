#include "data_structs.h"


//================================================================//

//Encolar un elemento en el str_queue
void str_enqueue(struct str_queue *queue, struct str_queue_node *nuevo_nodo){
	if(*(queue->size) == 0){
		queue->head = nuevo_nodo;
		queue->tail = nuevo_nodo;
	}else{
		queue->tail->next = nuevo_nodo;
		queue->tail = nuevo_nodo;
	}
	*(queue->size) = *(queue->size) + 1;
}

//Desencolar un elemento en el str_queue
int str_dequeue(struct str_queue *queue){
	if(*(queue->size) > 1){
		queue->head = queue->head->next;
	}else if(*(queue->size) == 1){
		queue->head = NULL;
		queue->tail = NULL;
	}else{
		return -1;
	}
	*(queue->size) = *(queue->size) - 1;
	return *(queue->size);
}

//================================================================//

void enqueue_task(struct pgm_process *proceso, struct pgm_task *nuevo_bloque){
	if(proceso->tasks_restantes == 0){
		proceso->head = nuevo_bloque;
		proceso->tail = nuevo_bloque;
	}else{
		proceso->tail->next = nuevo_bloque;
		proceso->tail = nuevo_bloque;
	}
	proceso->tasks_restantes = proceso->tasks_restantes + 1;
}

int dequeue_task(struct pgm_process *proceso){
	if(proceso->tasks_restantes > 1){
		proceso->head = proceso->head->next;
	}else if(proceso->tasks_restantes == 1){
		proceso->head = NULL;
		proceso->tail = NULL;
	}else{
		return -1;
	}
	proceso->tasks_restantes--;
	return proceso->tasks_restantes;
}


void agregar_proceso(struct pgm_process_cll *cll_procesos, struct pgm_process *nuevo_proceso){
	if(*(cll_procesos->size) == 0){
		cll_procesos->head = nuevo_proceso;
		cll_procesos->tail = nuevo_proceso;
		cll_procesos->tail->next = cll_procesos->head;
		cll_procesos->pointer = cll_procesos->head;
	}else{
		cll_procesos->tail->next = nuevo_proceso;
		cll_procesos->tail = nuevo_proceso;
		cll_procesos->tail->next = cll_procesos->head;
	}
	*(cll_procesos->size) = *(cll_procesos->size) + 1;
}

void move_pointer(struct pgm_process_cll *cll_procesos){
	cll_procesos->pointer = cll_procesos->pointer->next;
}

int get_process_by_id(int p_id, struct pgm_process_cll *cll_procesos, struct pgm_process *proceso){
	proceso = cll_procesos->head;
	for(int i = 1; i < *(cll_procesos->size)+1; i++){
		if(proceso->id == p_id){
			return i;
		}
		proceso = proceso->next;
	}
	return 0;
};

int remove_process_by_id(int p_id, struct pgm_process_cll *cll_procesos){
	struct pgm_process *anterior = cll_procesos->tail;
	struct pgm_process *current = cll_procesos->head;
	if(*(cll_procesos->size)==1 && current->id == p_id){
		cll_procesos->head = NULL;
		cll_procesos->tail = NULL;
		cll_procesos->pointer = NULL;
		*(cll_procesos->size) = *(cll_procesos->size) - 1;
		return 1;
	}
	cll_procesos->head = cll_procesos->head->next;
	anterior->next = cll_procesos->head;
	*(cll_procesos->size) = *(cll_procesos->size) - 1;
	return 1;
	/*for(int i = 0; i < *(cll_procesos->size); i++){
		printf("%d\n",i);
		if(current->id == p_id){
			if(cll_procesos->pointer == current){
				move_pointer(cll_procesos);
			}
			cll_procesos->head = cll_procesos->head->next;
			anterior->next = cll_procesos->head;
			*(cll_procesos->size) = *(cll_procesos->size) - 1;
			return 1;
		}
		anterior = current;
		current = current->next;
	}
	return 0;*/
}

int remove_older(struct pgm_process_cll *cll_procesos){
	if(*(cll_procesos->size)==0){
		return -1;
	}else if(*(cll_procesos->size)==1){
		cll_procesos->head = NULL;
		cll_procesos->tail = NULL;
		cll_procesos->pointer = NULL;
	}else{
		if(cll_procesos->pointer == cll_procesos->head){
				move_pointer(cll_procesos);
			}
		cll_procesos->head = cll_procesos->head->next;
		cll_procesos->tail->next = cll_procesos->head;
	}
	*(cll_procesos->size) = *(cll_procesos->size) - 1;
	return 1;
}
