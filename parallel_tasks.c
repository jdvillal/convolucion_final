#include "parallel_tasks.h"
//#include "data_structs.h"


#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"



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

void print_fnames_queue(){
	struct str_queue_node *n = fnames_queue->head;
	for(int i = 0; i < *(fnames_queue->size); i ++){
		printf("%s\n", n->str_data);
		n = n->next;
	}
}

void print_process_cll(){
	struct pgm_process *n = procesos_pendientes_cll->head;
	for(int i = 0; i < *(procesos_pendientes_cll->size); i ++){
		printf("En cola de procesos: %ld\n", n);
		n = n->next;
	}
	printf("\n");
}

//==================================================================//

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
				printf(ANSI_COLOR_CYAN"Lector/Planificador: Sin imagenes que cargar. Suspendiendo...\n"ANSI_COLOR_RESET);
				pthread_cond_wait(&fnames_queue_cond, &fnames_queue_lock);//se espera a que el usuario agregue archivos para leer
				printf(ANSI_COLOR_CYAN"Lector/Planificador: reactivado\n"ANSI_COLOR_RESET);
		}
		//Cuando una nueva imagen ha sido agregada a la lista el hilo se vuelve a activar
		char *image_name = fnames_queue->head->str_data;
		str_dequeue(fnames_queue);
		pthread_mutex_unlock (&fnames_queue_lock);
		//printf(ANSI_COLOR_CYAN"Lector/Planificador: Cargando en memoria la imagen \"%s\" y dividiendo en %d bloques...\n"ANSI_COLOR_RESET,image_name, *n_blocks);
		struct pgm_process *nuevo_proceso = malloc(sizeof(struct pgm_process));
		nuevo_proceso->id = count;
		set_img_as_process(image_name, nuevo_proceso, *n_blocks );
		
		pthread_mutex_lock (&procesos_pendientes_cll_lock);//Se bloquea el acceso a la cola de procesos
		agregar_proceso(procesos_pendientes_cll, nuevo_proceso);//Se agrega un nuevo proceso a la cola
		//print_process_cll();//////////////
		pthread_mutex_unlock (&procesos_pendientes_cll_lock);
		pthread_cond_broadcast(&procesos_pendientes_cll_cond);//Cuando la cola de procesos ha sido modificada se notifica a los worker thread
		count++;
			
	}

}

//======================================================================//

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
	while(1){
		pthread_mutex_lock (&setted_thread_count_lock);//se bloquea el acceso a la cantidad de hilos seteada
		while(setted_thread_count < worker_id){//Se suspende si la cantidad de hilos seteada es menor al id del thread
			printf(ANSI_COLOR_CYAN"WORKER_THREAD: Suspendiendo...\n"ANSI_COLOR_RESET);
			pthread_cond_wait(&setted_thread_count_cond, &setted_thread_count_lock);//se espera a que el usuario actualice la cantidad de worker_threads
		}
		pthread_mutex_unlock(&setted_thread_count_lock);
		//Cuando la cantidad de hilos seteada es mayor al worker_id el worker_thread se activa


		pthread_mutex_lock(&procesos_pendientes_cll_lock);//se bloquea el acceso a la cll de procesos
		while(*(procesos_pendientes_cll->size) < 1){//se suspende si no hay procesos disponibles
			//printf(ANSI_COLOR_CYAN"WORKER_THREAD %d: Sin imagenes que procesar. Suspendiendo...\n"ANSI_COLOR_RESET, worker_id);
			pthread_cond_wait(&procesos_pendientes_cll_cond, &procesos_pendientes_cll_lock);//se espera a que el planificador agregue procesos
		}
		//cuando se encuentran procesos disponibles el worker thread se activa 
		struct pgm_process *proceso = procesos_pendientes_cll->pointer;//se accede al siguiente proceso en la cola
		struct pgm_task *my_task = proceso->head;//se extrae el siguiente task disponible del proceso
		move_pointer(procesos_pendientes_cll);//se mueve el puntero en la cola de procesos
		int complete;
		if((complete = !dequeue_task(proceso))==1){//se remueve el task extraido y se verifica si quedan tasks pendientes en el proceso
			remove_process_by_id(proceso->id, procesos_pendientes_cll);//si no quedan tasks pendientes se remueve el proceso de la cll de procesos
		}pthread_mutex_unlock(&procesos_pendientes_cll_lock);	

		apply_sharpen(proceso->img_data, my_task);//se aplica el filtro a la porcion de la imagen que representa el task

		if(complete){//si el proceso ha sido completado se procede a guardarlo en la lista de procesos terminados
			pthread_mutex_lock(&procesos_terminados_cll_lock);//Se protege el acceso a la lista de procesos terminados
			agregar_proceso(procesos_terminados_cll, proceso);//se agrega el proceso a la lista de procesos terminados
			pthread_mutex_unlock(&procesos_terminados_cll_lock);
			pthread_cond_signal(&procesos_terminados_cll_cond);//se notifica al hilo almacenador
		}

	}

}

//==========================================================================//

void *almacenador(void *tid){
	struct stat st = {0};
	if (stat("./imagesOut", &st) == -1) {
    	mkdir("./imagesOut", 0700);
    }
	while(1){
		pthread_mutex_lock(&procesos_terminados_cll_lock);
		while(*(procesos_terminados_cll->size) < 1){//se suspende si no hay procesos terminados
			printf(ANSI_COLOR_CYAN"ALMACENADOR: En espera de procesos terminados. Suspendiendo...\n"ANSI_COLOR_RESET);
			pthread_cond_wait(&procesos_terminados_cll_cond, &procesos_terminados_cll_lock);//se espera a que los worked_thread agreguen procesos terminados
		}
		//printf(ANSI_COLOR_CYAN"ALMACENADOR: despertado\n"ANSI_COLOR_RESET);
		struct pgm_process *nuevo_terminado = procesos_terminados_cll->head;//se accede al proceso terminado mas viejo en la lista
		remove_older(procesos_terminados_cll);//se remueve el proceso terminado de la lista de procesos terminados
		pthread_mutex_unlock(&procesos_terminados_cll_lock);
		save_image(nuevo_terminado);

	}
}
