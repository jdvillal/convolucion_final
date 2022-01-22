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


int set_img_as_process(char* filename, struct pgm_process *nuevo_proceso, int n_tasks){
	nuevo_proceso->tasks_restantes = 0;
	nuevo_proceso->tasks_completados = 0;
	nuevo_proceso->n_tasks = n_tasks;
	int *ancho = (int*)malloc(32);//ancho de la imagen
	int *alto = (int*)malloc(32);//alto de la imagen
	int *max_gris = (int*)malloc(32);//maximo valor gris
	int *o_matrix=NULL;//matriz de numeros de la imagen original
	int r = pgma_read_test( filename,max_gris, ancho, alto, &o_matrix );
	int *p_matrix = ( int * ) malloc ((*ancho )*(*alto)*sizeof(int));//matriz donde se guarda imagen procesada

	struct pgm_data *nuevo_img_data = malloc(sizeof(struct pgm_data));
	nuevo_img_data->name = filename;
	nuevo_img_data->ancho = ancho;
	nuevo_img_data->alto = alto;
	nuevo_img_data->max_gris = max_gris;
	nuevo_img_data->matrix_original = o_matrix;
	nuevo_img_data->matrix_procesada = p_matrix;

	nuevo_proceso->img_data = nuevo_img_data;

	if(*alto < n_tasks){
		n_tasks = *alto;
	}

	int alto_task = ceil((float)*alto/n_tasks);
	int y_f_anterior = 0;
	for(int i = 1; i <= n_tasks; i++){
		struct pgm_task *nuevo_task =	malloc(sizeof(struct pgm_task));
		nuevo_task->x_0 = 0;
		nuevo_task->y_0 = y_f_anterior;
		nuevo_task->x_f = *ancho - 1;
		nuevo_task->y_f = (i * alto_task) - 1;
		y_f_anterior = nuevo_task->y_f + 1;

		if(i == n_tasks && i * alto_task != *alto){
				nuevo_task->y_f = *alto -1;
		}
		//printf("x_0: %d	y_0: %d	x_f: %d y_f: %d\n", nuevo_task->x_0, nuevo_task->y_0, nuevo_task->x_f, nuevo_task->y_f);
		enqueue_task(nuevo_proceso, nuevo_task);
	}

	return r;
}

int convolucion_pixel(int x, int y, int ancho, int alto, int *matrix, int *matr_filtro){
	int pxls_mults[9] = {0};
	int pxls_validos = 0;
	//validar pixel superior izquierdo
	if((x-1 >= 0) && (y-1 >= 0)){
		pxls_mults[pxls_validos] = get_matrix_item(x-1, y-1, ancho, matrix) * get_matrix_item(0,0,3,matr_filtro);
		pxls_validos = pxls_validos + 1;
	}
	//validar pixel superior
	if(y-1 >= 0){
		pxls_mults[pxls_validos] = get_matrix_item(x, y-1, ancho, matrix) * get_matrix_item(1,0,3,matr_filtro);
		pxls_validos = pxls_validos + 1;
	}
	//validar pixel superior derecho
	if((x+1 < ancho) && (y-1 >= 0)){
		pxls_mults[pxls_validos] = get_matrix_item(x+1, y-1, ancho, matrix) * get_matrix_item(2,0,3,matr_filtro);
		pxls_validos = pxls_validos + 1;
	}
	//validar pixel izquierdo
	if(x-1 >= 0){
		pxls_mults[pxls_validos] = get_matrix_item(x-1, y, ancho, matrix) * get_matrix_item(0,1,3,matr_filtro);
		pxls_validos = pxls_validos + 1;
	}
	//pixel central (KERNEL)
	pxls_mults[pxls_validos] = get_matrix_item(x, y, ancho, matrix) * get_matrix_item(1,1,3,matr_filtro);
	pxls_validos = pxls_validos + 1;

	//validar pixel derecho
	if(x+1 < ancho){
		pxls_mults[pxls_validos] = get_matrix_item(x+1, y, ancho, matrix) * get_matrix_item(2,1,3,matr_filtro);
		pxls_validos = pxls_validos + 1;
	}
	//validar pixel inferior izquierdo
	if((x-1 >= 0)&&(y+1 < alto)){
		pxls_mults[pxls_validos] = get_matrix_item(x-1, y+1, ancho, matrix) * get_matrix_item(0,2,3,matr_filtro);
		pxls_validos = pxls_validos + 1;
	}
	//validar pixel inferior
	if(y+1 < alto){
		pxls_mults[pxls_validos] = get_matrix_item(x, y+1, ancho, matrix) * get_matrix_item(1,2,3,matr_filtro);
		pxls_validos = pxls_validos + 1;
	}
	//validar pixel inferior derecho
	if((x+1 < ancho)&&(y+1 < alto)){
		pxls_mults[pxls_validos] = get_matrix_item(x+1, y+1, ancho, matrix) * get_matrix_item(2,2,3,matr_filtro);
		pxls_validos = pxls_validos + 1;
	}
	int sum = 0;
	for(int i = 0; i < pxls_validos; i++){
		sum = sum + pxls_mults[i];
	}
	if(sum < 0){
		sum = -sum;
	}
	return (int)(sum);
}

void apply_sharpen(struct pgm_data *my_data, struct pgm_task *my_task){
	int sharpen[3][3] = {{0,-1,0},{-1,5,-1},{0,-1,0}};//matriz filtro SHARPEN
	for(int j = my_task->y_0; j <= my_task->y_f; j++){
		for(int i = my_task->x_0 ; i <= my_task->x_f; i++){
			int n_pixel = convolucion_pixel(i, j, *(my_data->ancho), *(my_data->alto), my_data->matrix_original, *sharpen);
			put_matrix_item(n_pixel, i, j, *(my_data->ancho), my_data->matrix_procesada);
		}
	}

	//print_matrix(*(my_data->ancho),*(my_data->alto),my_data->matrix_original);
	//print_matrix(*(my_data->ancho),*(my_data->alto),my_data->matrix_procesada);

}



char *get_file_out_name(struct pgm_process *my_process){
	char *str = malloc(100*sizeof(char));
	strcpy(str, "imagesOut/" );
	char id_str[20];
	sprintf(id_str, "sharpen_%d_", my_process->id);
	strcat(str, id_str);

	const char ch = '/';
	char *ret;
	ret = strrchr(my_process->img_data->name, ch);

	if(ret == NULL){
		strcat(str, my_process->img_data->name);
	}else{
		strcat(str, ret+1);
	}
	//printf("%s\n", str);
	return str;
}

int save_image(struct pgm_process *my_process){
	char comment[] = "Imagen filtrada por filter_server.c";
	pgma_write( get_file_out_name(my_process), comment, *(my_process->img_data->ancho),*(my_process->img_data->alto),
									*(my_process->img_data->matrix_original), my_process->img_data->matrix_procesada);
	return 1;
}