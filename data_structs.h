#include <stdio.h>

//=============================================================//
struct str_queue_node{
  struct str_queue_node *next;
  char *str_data;
};

struct str_queue{
  int size;
  struct str_queue_node *head;
  struct str_queue_node *tail;
};

void str_enqueue(struct str_queue *queue, struct str_queue_node *nuevo_nodo);
int str_dequeue(struct str_queue *queue);


//=============================================================//
struct cll_img_node{
  struct string_node *next;
  char *datos;
};

typedef struct {
    int size;
    struct cll_img_node *current_head;
}cll_imgs; //lista circular enlazada de imagenes






