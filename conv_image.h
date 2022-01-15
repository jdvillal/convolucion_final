#include"pgma_io.h"

//Estructura, representa de forma matricial una imagen pgm
typedef struct {
  int *ancho;
  int *alto;
  int *maxg;
  int *matrix;
} pgm_image_matrix;

typedef struct {
  int x_0, y_0;//coordenada inicial
  int x_f,y_f;//coordenada final
  pgm_image_matrix *img_matrix;
} Matrix_seccion;

void print_matrix(int ancho, int alto, int *matrix);
int get_matrix_item(int x, int y, int ancho, int *matrix);
void put_matrix_item(int item, int x, int y, int ancho, int *matrix);
int pgma_read_test (char *file_name, int *maxg,int *xsize,int *ysize, int **g );
int load_image(char *file_name, pgm_image_matrix *img_matrix);