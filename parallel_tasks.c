#include "parallel_tasks.h"
//#include "data_structs.h"


#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"



//===========================================================//


void iniciar_fnames_queue(){
  fnames_queue = malloc(sizeof(struct str_queue));
  fnames_queue->size = malloc(sizeof(int));
  *(fnames_queue->size) = 0;
}

void add_to_fnames_queue(char *filename){
  if(strcmp(get_filename_ext(filename),"pgm")!=0){
      // si el archivo no es pgm se ignora
      return;
  }
  struct str_queue_node *n_nodo = malloc(sizeof(struct str_queue_node));
  n_nodo->str_data = (char*) malloc(strlen(filename)*sizeof(char));
  strcpy(n_nodo->str_data, filename);
  pthread_mutex_lock (&fnames_queue_mutex);
  str_enqueue(fnames_queue, n_nodo);
  pthread_mutex_unlock (&fnames_queue_mutex);
  pthread_cond_signal(&fnames_queue_cond);
}

int load_fnames_list(char *filename){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  fp = fopen(filename, "r");
  if (fp == NULL){
    return -1;
  }
  while ((read = getline(&line, &len, fp)) != -1) {
    remove_ln(line);
    add_to_fnames_queue(line);
  }
  fclose(fp);
  if (line)
    free(line);
  return 1;
}

void print_fnames_queue(){
  struct str_queue_node *n = fnames_queue->head;
  for(int i = 0; i < *(fnames_queue->size); i ++){
    printf("%s\n", n->str_data);
    n = n->next;
  }
}

//==================================================================//

void iniciar_cll_procesos_pendientes(){
  procesos_pendientes_cll = malloc(sizeof(procesos_pendientes_cll));
  procesos_pendientes_cll->size = malloc(sizeof(int));
  *(procesos_pendientes_cll->size) = 0;
}

void iniciar_cll_procesos_terminados(){
  procesos_terminados_cll = malloc(sizeof(procesos_pendientes_cll));
  procesos_terminados_cll->size = malloc(sizeof(int));
  *(procesos_terminados_cll->size) = 0;
}

void *lector_planificador(void *tid) {
  int *n_blocks = (int *) tid;
  while(1){
      //Se protege el acceso a la lista nombres de archivos por leer
      pthread_mutex_lock (&fnames_queue_mutex);
      //Cuando no hay imagenes por leer en la lista, el hilo se suspende
      while(*(fnames_queue->size)<1){
          printf(ANSI_COLOR_CYAN"Lector/Planificador: Sin imagenes que cargar. Suspendiendo...\n"ANSI_COLOR_RESET);
          pthread_cond_wait(&fnames_queue_cond, &fnames_queue_mutex);
          printf(ANSI_COLOR_CYAN"Lector/Planificador: reactivado\n"ANSI_COLOR_RESET);
      }
      //Cuando una nueva imagen ha sido agregada a la lista el hilo se vuelve a activar
      char *image_name = fnames_queue->head->str_data;
      str_dequeue(fnames_queue);
      pthread_mutex_unlock (&fnames_queue_mutex);
      printf(ANSI_COLOR_CYAN"Lector/Planificador: Cargando en memoria la imagen \"%s\" y dividiendo en %d bloques...\n"ANSI_COLOR_RESET,image_name, *n_blocks);
      struct pgm_img_process *nuevo_proceso = malloc(sizeof(struct pgm_img_process));
      set_img_as_process(image_name, nuevo_proceso, 4 );
  }

}
