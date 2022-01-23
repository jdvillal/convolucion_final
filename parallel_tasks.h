#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "conv_image.h"
//#include "data_structs.h"


//===============================================================//

pthread_mutex_t fnames_queue_lock;
pthread_cond_t fnames_queue_cond;
struct str_queue *fnames_queue;//cola con los nombres/rutas de imagenes POR LEER

//inicializa la cola fnames_queue
void iniciar_fnames_queue();

//Agrega el nombre/ruta de una imagen a la cola fnames_queue
void add_to_fnames_queue(char *filename);

//Lee una lista en un archivo .txt y agrega todos los nombres/rutas contenidas haciendo uso de add_file()
int load_fnames_list(char *filename);

//===============================================================//

pthread_mutex_t procesos_pendientes_cll_lock;
pthread_cond_t procesos_pendientes_cll_cond;
struct pgm_process_cll *procesos_pendientes_cll;

void iniciar_cll_procesos_pendientes();

void *lector_planificador(void *t);

//===============================================================//

pthread_mutex_t setted_thread_count_lock;
pthread_cond_t setted_thread_count_cond;
int setted_thread_count;

void set_thread_count(int new_thread_count);
int get_thread_count();

pthread_mutex_t procesos_terminados_cll_lock;
pthread_cond_t procesos_terminados_cll_cond;
struct pgm_process_cll *procesos_terminados_cll;

void iniciar_cll_procesos_terminados();

typedef struct {
	int id;
} worker_id;

void *worker_thread(void *tid);

void *almacenador(void *tid);



