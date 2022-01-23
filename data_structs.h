#include <stdio.h>
#include <stdlib.h>

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
struct pgm_task{
	int x_0, y_0;//coordenada inicial
	int x_f,y_f;//coordenada final
	struct pgm_task *next;
};

/*Representacion struct de una imagen pgm*/
struct pgm_data{
	char *name;
	int *ancho;
	int *alto;
	int *max_gris;
	int *matrix_original;//apunta a la direccion de memoria donde se ubica la imagen original
	int *matrix_procesada;//apunta a la direccion de memoria donde se ubica la imagen procesada
};

/*T*/
struct pgm_process{
	int id;
	int n_tasks;
	int tasks_restantes;
	int tasks_completados;
	struct pgm_data *img_data;//apunta a los datos de la imagen a procesar
	struct pgm_task *head;//apunta al primer bloque que compone la imagen 
	struct pgm_task *tail;//apunta al ultimo bloque que compone la imagen
	struct pgm_process *next;//apunta al siguiente proceso en la lista de procesos
};

void enqueue_task(struct pgm_process *proceso, struct pgm_task *nuevo_bloque);
int dequeue_task(struct pgm_process *proceso);

/*Lista circular enlazada de procesos*/
struct pgm_process_cll{
	int *size;
	struct pgm_process *head;
	struct pgm_process *tail;
	struct pgm_process *pointer;
};


void agregar_proceso(struct pgm_process_cll *cll_procesos, struct pgm_process *nuevo_proceso);
void move_pointer(struct pgm_process_cll *cll_procesos);
int get_process_by_id(int p_id, struct pgm_process_cll *cll_procesos, struct pgm_process *proceso);
int remove_process_by_id(int p_id, struct pgm_process_cll *cll_procesos);
int remove_older(struct pgm_process_cll *cll_procesos);



void free_pgm_data(struct pgm_data *data);

void free_pgm_process(struct pgm_process *proceso);







