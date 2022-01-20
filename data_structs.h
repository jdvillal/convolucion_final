#include <stdio.h>

//=============================================================//
struct str_queue_node{
  struct str_queue_node *next;
  char *str_data;
};

struct str_queue{
  int *size;
  struct str_queue_node *head;
  struct str_queue_node *tail;
};

void str_enqueue(struct str_queue *queue, struct str_queue_node *nuevo_nodo);
int str_dequeue(struct str_queue *queue);


//=============================================================//

/*Representacion struct de un fragmento/bloque de una imagen pgm*/
struct pgm_img_block{
  int x_0, y_0;//coordenada inicial
  int x_f,y_f;//coordenada final
  struct pgm_img_block *next;
};

/*TDA: contiene la matriz de una imagen asi como los bloques que bloques que la componen*/
struct pgm_img_process{
  int id;
  int b_restantes;
  int *matrix_original;//apunta a la direccion de memoria donde se ubica la imagen original
  int *matrix_procesada;//apunta a la direccion de memoria donde se ubica la imagen procesada
  struct pgm_img_block *head;
  struct pgm_img_block *tail;
  struct pgm_img_process *next;
};

void pgm_img_process_enqueue(struct pgm_img_process *cola_bloques, struct pgm_img_block *nuevo_bloque);
int pgm_img_process_dequeue(struct pgm_img_process *cola_bloques);

/*Lista circular enlazada de pgm_img*/
struct imgs_process_cll{
  int *size;
  struct pgm_img_process *head;
  struct pgm_img_process *tail;
  struct pgm_img_process *pointer;
};


void imgs_cll_add(struct imgs_process_cll *cll_procesos, struct pgm_img_process *nuevo_proceso);
void move_pointer(struct imgs_process_cll *cll_processos);
int imgs_cll_get_by_id(int p_id, struct imgs_process_cll *cll_procesos, struct pgm_img_process *proceso);
int imgs_cll_remove_by_id(int p_id, struct imgs_process_cll *cll_procesos);







