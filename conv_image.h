#include <string.h>
#include"pgma_io.h"
#include "data_structs.h"

//Estructura, representa de forma matricial una imagen pgm
struct pgm_image_matrix{
	int *ancho;
	int *alto;
	int *maxg;
	int *matrix;
};


void print_matrix(int ancho, int alto, int *matrix);
int get_matrix_item(int x, int y, int ancho, int *matrix);
void put_matrix_item(int item, int x, int y, int ancho, int *matrix);
int pgma_read_test (char *file_name, int *maxg,int *xsize,int *ysize, int **g );
int load_image(char *file_name, struct pgm_image_matrix *img_matrix);
const char *get_filename_ext(const char *filename);

int set_img_as_process(char* filename, struct pgm_process *n_proceso, int n_bloques);
void apply_sharpen(struct pgm_data *my_data, struct pgm_task *my_task);
char *get_file_out_name(struct pgm_process *my_process);

int save_image(struct pgm_process *my_process);