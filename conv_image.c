#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "conv_image.h"


void print_matrix(int ancho, int alto, int *matrix){
  printf("columnas: %d Filas: %d\n", ancho, alto);
  for(int j = 0; j < alto; j++){
    for(int i = 0; i < ancho; i ++){
      printf("%d ", *matrix);
      matrix = matrix + 1;
    }
    printf("\n");
  }
}

int get_matrix_item(int x, int y, int ancho, int *matrix){
  return matrix[y * ancho + x];
}

void put_matrix_item(int item, int x, int y, int ancho, int *matrix){
  matrix[y * ancho + x] = item;
}

int pgma_read_test (char *file_name, int *maxg,int *xsize,int *ysize, int **g ) {
  return pgma_read ( file_name, xsize, ysize, maxg, g );
}

int load_image(char *file_name, struct pgm_image_matrix *img_matrix){
  img_matrix->maxg = (int*)malloc(32);
  img_matrix->ancho = (int*)malloc(32);
  img_matrix->alto = (int*)malloc(32);
  int *matrix = NULL;
  int r = pgma_read_test ( file_name, img_matrix->maxg, img_matrix->ancho, img_matrix->alto, &matrix );
  img_matrix->matrix = matrix;
  return r;
}

const char *get_filename_ext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}


int set_img_as_process(char* filename, struct pgm_process *nuevo_proceso, int n_bloques){
  nuevo_proceso->b_restantes = 0;
  int *ancho = (int*)malloc(32);//ancho de la imagen
  int *alto = (int*)malloc(32);//alto de la imagen
  int *max_gris = (int*)malloc(32);//maximo valor gris
  int *o_matrix=NULL;//matriz de numeros de la imagen original
  int r = pgma_read_test( filename,max_gris, ancho, alto, &o_matrix );
  int *p_matrix = ( int * ) malloc ((*ancho )*(*alto)*sizeof(int));//matriz donde se guarda imagen procesada

  struct pgm_data *nuevo_img_data = malloc(sizeof(struct pgm_data));
  nuevo_img_data->ancho = ancho;
  nuevo_img_data->alto = alto;
  nuevo_img_data->max_gris = max_gris;
  nuevo_img_data->matrix_original = o_matrix;
  nuevo_img_data->matrix_procesada = p_matrix;

  nuevo_proceso->img_data = nuevo_img_data;

  if(*alto < n_bloques){
    n_bloques = *alto;
  }

  int alto_bloques = ceil((float)*alto/n_bloques);
  int y_f_anterior = 0;
  for(int i = 1; i <= n_bloques; i++){
    struct pgm_task *nuevo_bloque =  malloc(sizeof(struct pgm_task));
    nuevo_bloque->x_0 = 0;
    nuevo_bloque->y_0 = y_f_anterior;
    nuevo_bloque->x_f = *ancho - 1;
    nuevo_bloque->y_f = (i * alto_bloques) - 1;
    y_f_anterior = nuevo_bloque->y_f + 1;

    if(i == n_bloques && i * alto_bloques != *alto){
        nuevo_bloque->y_f = *alto -1;
    }
    //printf("x_0: %d  y_0: %d  x_f: %d y_f: %d\n", nuevo_bloque->x_0, nuevo_bloque->y_0, nuevo_bloque->x_f, nuevo_bloque->y_f);
    enqueue_task(nuevo_proceso, nuevo_bloque);
  }

  return r;
}


