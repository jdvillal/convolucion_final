#include <getopt.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#include "parallel_tasks.h"

void escuchar_cliente(int connfd);

int *S_blocks;

int current_max_threads = 0;
pthread_t w_tid[1000];

int main(int argc, char **argv){
	S_blocks = malloc(sizeof(int));
	*S_blocks = 4;
	int opt;

	//Sockets
	int listenfd, connfd;
	unsigned int clientlen;
	//Direcciones y puertos
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp, *port, *ruta_lista_imagenes;
	while ((opt = getopt (argc, argv, "ht")) != -1){
		switch(opt)
		{
			case 'h':
				print_help_server(argv[0]);
				return 0;
			default:
				fprintf(stderr, "uso: %s <puerto>\n", argv[0]);
				fprintf(stderr, "		 %s -h\n", argv[0]);
				return -1;
		}
	}

	if(argc != 5){
		fprintf(stderr, "uso:\t%s <puerto> <ruta/nombre listado de imagenes> <cantidad_hilos> <cantidad_bloques>\n", argv[0]);
		fprintf(stderr, "\t%s -h\n", argv[0]);
		return -1;
	}else{
		port = argv[1];//puerto en el que escuchara al cliente
		ruta_lista_imagenes = argv[2];//nombre o ruta del archivo txt con la lista de imagenes
		current_max_threads = atoi(argv[3]);//Cantidad inicial de hilos
		*S_blocks = atoi(argv[4]);//Cantidad de bloques a dividir las imagenes
	}
	//Valida el puerto
	int port_n = atoi(port);
	if(port_n <= 0 || port_n > USHRT_MAX){
		fprintf(stderr, "Puerto: %s invalido. Ingrese un número entre 1 y %d.\n", port, USHRT_MAX);
		return -1;
	}

	if(*S_blocks < 1){
		printf("ERROR: Ingrese una cantidad de bloques mayor a 0");
		return -1;
	}
	//valida que la cantidad de hilos ingreasada por el usuario sea numerica
	if(current_max_threads == 0){
		printf("ERROR: Ingrese una cantidad inicial de hilos válida\n");
		return -1;
	}else{
		set_thread_count(current_max_threads);
	}


	//Valida si el archivo .txt (lista de imagenes) existe
	if(validateFile(ruta_lista_imagenes) == 0){
		fprintf(stderr, "MAIN: El archivo solicitado: \"%s\" no ha sido encontrado\n", ruta_lista_imagenes);
		return -1;
	}
	printf("MAIN: Leyendo el archivo \"%s\"\n", ruta_lista_imagenes);

	iniciar_fnames_queue();
	iniciar_cll_procesos_pendientes();
	iniciar_cll_procesos_terminados();
	load_fnames_list(ruta_lista_imagenes);
	pthread_t th;
	pthread_create(&th, NULL, lector_planificador, (void*)S_blocks);
	printf("MAIN: Hilo Lector/Planificador inciado.\n");
	pthread_t th2;
	pthread_create(&th2, NULL, almacenador, NULL);
	printf("MAIN: Hilo Almacenador inciado.\n");
	for(int i = 1; i <= get_thread_count(); i++){
		worker_id *thread_id = malloc(sizeof(worker_id));
		thread_id->id = i;
		pthread_create(&w_tid[i-1], NULL, worker_thread, (void*)thread_id);
	}
	printf("MAIN: Cantidad inicial de hilos de procesamiento seteada a %d threads.\n",get_thread_count());
	printf("MAIN: Cantidad fija de bloques seteada en %d.\n",*S_blocks);


	//Abre un socket de escucha en puerto indicado por el usuario
	listenfd = open_listenfd(port);
	if(listenfd < 0)
		connection_error(listenfd);

	printf("MAIN: Escuchando instrucciones remotas en el puerto %s...\n", port);

	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

		/* Determina el nombre dominio y la IP del cliente */
		hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);

		printf("MAIN: conectado a %s (%s)\n", hp->h_name, haddrp);
		escuchar_cliente(connfd);
		printf("MAIN: desconectando de %s (%s)\n", hp->h_name, haddrp);

		close(connfd);
	}
}

void escuchar_cliente(int connfd){
	int n;
	char buf[MAXLINE] = {0};
	while(1){
		n = read(connfd, buf, MAXLINE);
		if(n <= 0)
			return;

		const char s[2] = " ";
		char *token;
		token = strtok(buf, s);

		if(strcmp(token, "add")==0){
			token = strtok(NULL, s);
			long filesize = validateFile(token);//Valida si el archivo existe
			if(filesize > 0){
				printf("MAIN: Comando remoto recibido>> add %s\n\tEl archivo \"%s\" ha sido encontrado\n", token, token);
			}else{
				printf("MAIN: Comando remoto recibido>> add %s\n\tERROR:El archivo %s no exite\n", token, token);
			}
			char fsizeChar[MAXLINE] = {0};
			sprintf(fsizeChar,"%ld", filesize);
			n = write(connfd, fsizeChar, strlen(fsizeChar));//Envia el tamaño del archivo al cliente
			if(n <= 0)
				return;
			if(filesize>0){
				/*Si el archivo ingresado por el cliente existe, se agrega a la cola*/
				add_to_fnames_queue(token);
			}
		}else if(strcmp(token,"threads")==0){
			token = strtok(NULL, s);
			int n_threads = atoi(token);
			printf("MAIN: Comando remoto recibido>> threads %d \nMAIN: Cambiando a %d hilos de procesamiento disponibles\n",n_threads, n_threads);
			char n_threadsChar[MAXLINE] = {0};
			sprintf(n_threadsChar,"%d", get_thread_count());
			set_thread_count(n_threads);
			if(n_threads > current_max_threads){
				for(int i = current_max_threads + 1 ; i <= n_threads; i++ ){
					worker_id *thread_id = malloc(sizeof(worker_id));
					thread_id->id = i;
					pthread_create(&w_tid[i-1], NULL, worker_thread, (void*)thread_id);
				}
				current_max_threads = n_threads;
			}
			n = write(connfd, n_threadsChar, sizeof(n_threadsChar));
			if(n <=0)
				return;
			//print_fnames_queue();
		}
		
		memset(buf, 0, MAXLINE);
	}
}

