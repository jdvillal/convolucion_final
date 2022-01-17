#include "data_structs.h"

//================================================================//

void str_enqueue(struct str_queue *cola, struct str_queue_node *nuevo_nodo){
  if(cola->size == 0){
    cola->head = nuevo_nodo;
    cola->tail = nuevo_nodo;
  }else{
    cola->tail->next = nuevo_nodo;
    cola->tail = nuevo_nodo;
  }
  cola->tamano++;
}

//Desencolar un elemento en el queue de forma segura
int str_dequeue(struct str_queue *cola, struct str_queue_node *nodo_extraido){
  nodo_extraido = cola->head;
  if(cola->size > 0){
    cola->head = cola->head->next;
    cola->size--;
    return cola->size;
  }else{
    return -1;
  }
}


//================================================================//


void cll_next(struct cll_imgs *list, struct cll_img_node *node){
  node = cll_img_node->current_head;
   
}
