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

void pgm_img_process_enqueue(struct pgm_img_process *cola_bloques, struct pgm_img_block *nuevo_bloque){
  if(cola_bloques->b_restantes == 0){
    cola_bloques->head = nuevo_bloque;
    cola_bloques->tail = nuevo_bloque;
  }else{
    cola_bloques->tail->next = nuevo_bloque;
    cola_bloques->tail = nuevo_bloque;
  }
  cola_bloques->b_restantes = cola_bloques->b_restantes + 1;
}

int pgm_img_process_dequeue(struct pgm_img_process *cola_bloques){
  if(cola_bloques->b_restantes > 1){
    cola_bloques->head = cola_bloques->head->next;
  }else if(cola_bloques->b_restantes == 1){
    cola_bloques->head = NULL;
    cola_bloques->tail = NULL;
  }else{
    return -1;
  }
  cola_bloques->b_restantes--;
  return cola_bloques->b_restantes;
}


void imgs_cll_add(struct imgs_process_cll *cll_procesos, struct pgm_img_process *nuevo_proceso){
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

void move_pointer(struct imgs_process_cll *cll_procesos){
  cll_procesos->pointer = cll_procesos->pointer->next;
}

int imgs_cll_get_by_id(int p_id, struct imgs_process_cll *cll_procesos, struct pgm_img_process *proceso){
  proceso = cll_procesos->head;
  for(int i = 1; i < *(cll_procesos->size)+1; i++){
    if(proceso->id == p_id){
      return i;
    }
    proceso = proceso->next;
  }
  return 0;
};

int imgs_cll_remove_by_id(int p_id, struct imgs_process_cll *cll_procesos){
  struct pgm_img_process *anterior = cll_procesos->tail;
  struct pgm_img_process *current = cll_procesos->head;
  if(*(cll_procesos->size)==1 && current->id == p_id){
      cll_procesos->head = NULL;
      cll_procesos->tail = NULL;
      cll_procesos->pointer = NULL;
      return 1;
  }
  for(int i = 0; i < *(cll_procesos->size); i++){
    if(current->id == p_id){
      anterior->next = current->next;
      *(cll_procesos->size) = *(cll_procesos->size) - 1;
      return 1;
    }
    anterior = current;
    current = current->next;
  }
  return 0;
}