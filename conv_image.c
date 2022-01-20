#include <stdlib.h>
#include <string.h>

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

