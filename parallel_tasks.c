#include "parallel_tasks.h"
//#include "data_structs.h"


#define ANSI_COLOR_RESET "\x1b[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"



//==========================================================================//


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
	pthread_mutex_lock (&fnames_queue_lock);
	str_enqueue(fnames_queue, n_nodo);
	pthread_mutex_unlock (&fnames_queue_lock);
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


//==========================================================================//

void iniciar_cll_procesos_pendientes(){
	procesos_pendientes_cll = malloc(sizeof(procesos_pendientes_cll));
	procesos_pendientes_cll->size = malloc(sizeof(int*));
	*(procesos_pendientes_cll->size) = 0;
}

void *lector_planificador(void *tid) {
	int *n_blocks = (int *) tid;
	int count = 1;
	while(1){
		//printf("%d\n", count);
		pthread_mutex_lock (&fnames_queue_lock);//Se bloquea el acceso a la lista nombres de archivos por leer
		while(*(fnames_queue->size)<1){//Cuando no hay imagenes por leer en la lista, el hilo se suspende
				printf(KCYN"Lector/Planificador: Sin imagenes que cargar. Suspendiendo...\n"ANSI_COLOR_RESET);
				pthread_cond_wait(&fnames_queue_cond, &fnames_queue_lock);//se espera a que el usuario agregue archivos para leer
				printf(KCYN"Lector/Planificador: reactivado\n"ANSI_COLOR_RESET);
		}
		//Cuando una nueva imagen ha sido agregada a la lista el hilo se vuelve a activar
		struct str_queue_node *node = fnames_queue->head;
		char *image_name = node->str_data;
		str_dequeue(fnames_queue);
		free(node);
		pthread_mutex_unlock (&fnames_queue_lock);
		struct pgm_process *nuevo_proceso = malloc(sizeof(struct pgm_process));
		nuevo_proceso->id = count;
		set_img_as_process(image_name, nuevo_proceso, *n_blocks );
		
		pthread_mutex_lock (&procesos_pendientes_cll_lock);//Se bloquea el acceso a la cola de procesos
		agregar_proceso(procesos_pendientes_cll, nuevo_proceso);//Se agrega un nuevo proceso a la cola
		pthread_mutex_unlock (&procesos_pendientes_cll_lock);
		printf(KCYN"Lector/Planificador: Imagen %s cargada en memoria y divida en %d bloques.\n"ANSI_COLOR_RESET, image_name, *n_blocks);
		pthread_cond_broadcast(&procesos_pendientes_cll_cond);//Cuando la cola de procesos ha sido modificada se notifica a los worker thread
		count++;
			
	}

}
//==========================================================================//

void iniciar_cll_procesos_terminados(){
	procesos_terminados_cll = malloc(sizeof(procesos_pendientes_cll));
	procesos_terminados_cll->size = malloc(sizeof(int));
	*(procesos_terminados_cll->size) = 0;
}

void set_thread_count(int new_thread_count){
	pthread_mutex_lock (&setted_thread_count_lock);//se protege el acceso al contador de threads seteados por el usuario
	setted_thread_count = new_thread_count;//se actualiza el contador de worker_threads activos
	pthread_mutex_unlock (&setted_thread_count_lock);
	pthread_cond_broadcast(&setted_thread_count_cond);//Una vez seteado el valor se notifica a los worker_threads
}

int get_thread_count(){
	return setted_thread_count;
}

void *worker_thread(void *tid) {
	worker_id *thread_id = tid;
	int worker_id = (int) thread_id->id;
	int itr = 0;
	while(1){
		int complete = 0;
		pthread_mutex_lock (&setted_thread_count_lock);//se bloquea el acceso a la cantidad de hilos seteada
		while(setted_thread_count < worker_id){//Se suspende si la cantidad de hilos seteada es menor al id del thread
			printf(KMAG"WORKER_THREAD %d: El usuario ha modificado la cantidad de hilos de procesamiento. Suspendiendo...\n"ANSI_COLOR_RESET, worker_id);
			pthread_cond_wait(&setted_thread_count_cond, &setted_thread_count_lock);//se espera a que el usuario actualice la cantidad de worker_threads
		}
		pthread_mutex_unlock(&setted_thread_count_lock);
		//Cuando la cantidad de hilos seteada es mayor al worker_id el worker_thread se activa



		pthread_mutex_lock(&procesos_pendientes_cll_lock);//se bloquea el acceso a la cll de procesos
		while(*(procesos_pendientes_cll->size) < 1){//se suspende si no hay procesos disponibles
			printf(KMAG"WORKER_THREAD %d: Sin imagenes que procesar. Suspendiendo...\n"ANSI_COLOR_RESET, worker_id);
			pthread_cond_wait(&procesos_pendientes_cll_cond, &procesos_pendientes_cll_lock);//se espera a que el planificador agregue procesos
		}
		//cuando se encuentran procesos disponibles el worker thread se activa 
		struct pgm_process *proceso = procesos_pendientes_cll->pointer;//se accede al siguiente proceso en la cola
		struct pgm_task *my_task = proceso->head;//se extrae el siguiente task disponible del proceso
		move_pointer(procesos_pendientes_cll);//se mueve el puntero en la cola de procesos
		if(dequeue_task(proceso) == 0){
			remove_process_by_id(proceso->id, procesos_pendientes_cll);//si no quedan task pendientes se remueve el proceso de la ccl de proceso pendientes
		}
		int n_proc = proceso->n_tasks - proceso->tasks_restantes;
		pthread_mutex_unlock(&procesos_pendientes_cll_lock);

		printf(KMAG"WORKER_THREAD %d: Procesando imagen \"%s\" bloque numero: %d de %d\n"ANSI_COLOR_RESET, worker_id, proceso->img_data->name,n_proc,proceso->n_tasks);
		apply_sharpen(proceso->img_data, my_task);/*se aplica el filtro a la porcion de la imagen que representa el task*/
		free(my_task);

		pthread_mutex_lock(&procesos_pendientes_cll_lock);
		if((proceso->tasks_completados = proceso->tasks_completados + 1) == proceso->n_tasks) complete = 1;
		pthread_mutex_unlock(&procesos_pendientes_cll_lock);


		if(complete){//si el proceso ha sido completado se procede a guardarlo en la lista de procesos terminados
			pthread_mutex_lock(&procesos_terminados_cll_lock);//Se protege el acceso a la lista de procesos terminados
			agregar_proceso(procesos_terminados_cll, proceso);//se agrega el proceso a la lista de procesos terminados
			pthread_mutex_unlock(&procesos_terminados_cll_lock);
			pthread_cond_signal(&procesos_terminados_cll_cond);//se notifica al hilo almacenador
		}
		itr++;
	}

}

//==========================================================================//

void *almacenador(void *tid){
	struct stat st = {0};
	if (stat("./imagesOut", &st) == -1) {//se comprueba si existe el directorio destino
    	mkdir("./imagesOut", 0700);//si no existe se lo crea
    }
	while(1){
		pthread_mutex_lock(&procesos_terminados_cll_lock);
		while(*(procesos_terminados_cll->size) < 1){//se suspende si no hay procesos terminados
			pthread_cond_wait(&procesos_terminados_cll_cond, &procesos_terminados_cll_lock);//se espera a que los worked_thread agreguen procesos terminados
		}
		struct pgm_process *nuevo_terminado = procesos_terminados_cll->head;//se accede al proceso terminado mas viejo en la lista
		remove_older(procesos_terminados_cll);//se remueve el proceso terminado de la lista de procesos terminados
		pthread_mutex_unlock(&procesos_terminados_cll_lock);
		char *name_out = save_image(nuevo_terminado);
		printf(KYEL"ALMACENADOR: Imagen \"%s\" procesada con exito. La imagen resultante ha sido guardado en disco como: \"%s\"\n"ANSI_COLOR_RESET,
				nuevo_terminado->img_data->name, name_out);
		free(name_out);
		free_pgm_process(nuevo_terminado);//se libera la memoria que ocupaba el proceso

	}
}
