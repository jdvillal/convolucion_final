#include "parallel_tasks.h"


//===========================================================//

void iniciar_fnames_queue(){
  fnames_queue = malloc(sizeof(struct str_queue));
  fnames_queue->size = 0;
}

void add_to_fnames_queue(char *filename){
    //PENDIENTE: Agregar validacion para *.pgm
    struct str_queue_node *n_nodo = malloc(sizeof(struct str_queue_node));
    n_nodo->str_data = (char*) malloc(strlen(filename)*sizeof(char));
    strcpy(n_nodo->str_data, filename);
    str_enqueue(fnames_queue, n_nodo);
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
    for(int i = 0; i < fnames_queue->size; i ++){
        printf("%s\n", n->str_data);
        n = n->next;
    }
}