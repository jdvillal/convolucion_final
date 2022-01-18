#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "data_structs.h"
#include "common.h"

//=============================================================//

pthread_mutex_t fnames_queue_lock;
struct str_queue *fnames_queue;//cola con los nombres/rutas de imagenes POR LEER

//inicializa la cola fnames_queue
void iniciar_fnames_queue();

//Agrega el nombre/ruta de una imagen a la cola fnames_queue
void add_to_fnames_queue(char *filename);

//Lee una lista en un archivo .txt y agrega todos los nombres/rutas contenidas haciendo uso de add_file()
int load_fnames_list(char *filename);

//imprime todas los nombres/rutas almacenados en fnames_queue
void print_fnames_queue();



