#include "data_structs.h"

//================================================================//

void str_enqueue(struct str_queue *queue, struct str_queue_node *nuevo_nodo){
  if(queue->size == 0){
    queue->head = nuevo_nodo;
    queue->tail = nuevo_nodo;
  }else{
    queue->tail->next = nuevo_nodo;
    queue->tail = nuevo_nodo;
  }
  //printf("%s\n", cola->tail->str_data);
  queue->size++;
}

//Desencolar un elemento en el queue de forma segura
int str_dequeue(struct str_queue *queue){
  if(queue->size > 1){
    queue->head = queue->head->next;
  }else if(queue->size == 1){
    queue->head = NULL;
    queue->tail = NULL;
  }else{
    return -1;
  }
  queue->size--;
  return queue->size;
}


//================================================================//

