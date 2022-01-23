#include "common.h"

int open_listenfd(char *port) 
{
    struct addrinfo hints, *listp, *p;
    int listenfd, optval=1;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;  /* Accept TCP connections */
    hints.ai_flags = AI_PASSIVE;      /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV; /* ... using a numeric port arg. */
    hints.ai_flags |= AI_ADDRCONFIG;  /* Recommended for connections */
    getaddrinfo(NULL, port, &hints, &listp);

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {

        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;  /* Socket failed, try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                   (const void *)&optval , sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        close(listenfd); /* Bind failed, try the next */
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, 1024) < 0)
		return -1;
    return listenfd;
}


int open_clientfd(char *hostname, char *port) {
    int clientfd;
    struct addrinfo hints, *listp, *p;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;  
    hints.ai_flags = AI_NUMERICSERV; 
    hints.ai_flags |= AI_ADDRCONFIG; 
    getaddrinfo(hostname, port, &hints, &listp);
  
    for (p = listp; p; p = p->ai_next) {

        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue; 
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) 
            break; 
        close(clientfd); 
    } 

    freeaddrinfo(listp);
    if (!p) 
        return -1;
    else   
        return clientfd;
}

void connection_error(int connfd)
{
	fprintf(stderr, "Error de conexión: %s\n", strerror(errno));
	close(connfd);
	exit(-1);
}

void print_help_server(char *command)
{//imprime por consola mensja de ayuda (servidor)
  printf("Procesador (sharpen) de imagenes multiples\n");
  printf("uso:\n %s <puerto> <ruta/nombre listado de imagenes> <cantidad_hilos>\n", command);
  printf(" %s -h\n", command);
  printf("Opciones:\n");
  printf(" -h\t\t\tAyuda, muestra este mensaje\n");
}

void print_help_client(char *command){//imprime por consola mensaje de ayuda (cliente) de la opcion -h
    printf("Cliente para enviar instrucciones al server de procesamiento de imagenes pgm.\n");
    printf("uso:\n %s <puerto>\n", command);
    printf(" %s -h\n", command);
    printf("Opciones:\n");
    printf(" -h\t\t\tAyuda, muestra este mensaje\n");
}

void print_error_client(){//imprime por consola mensaje de error (cliente)
    printf("ERROR: Introduzca un comando valido [add | threads | exit].");
    printf("\nEjemplos:\n");
    printf("\t>>add prueba.pgm\t(agregar la imagen \"prueba.pgm\" a la cola de procesamiento)\n");
    printf("\t>>threads 12\t\t(Cambiar a 12 la cantidad de hilos de procesamiento)\n");
    printf("\t>>exit\t\t\t(terminar la ejecucion del programa)\n");
}

void remove_ln(char *buf){//elimina el salto de linea al final de un string
    for(int i = 0; i < strlen(buf); i++){
      if(*(buf+i)=='\n'){
        memset(buf+i,0,1);
        break;
      }
    }
}

//verifica si un archivo solicitado por el cliente existe o no, retorna el tamano en bytes del archivo, si no existe retorna 0
long validateFile(char* filename){
  int n = open(filename,O_RDONLY,0);
  if(n<0){
    close(n);
    return 0;
  }else{
    struct stat st;
    stat(filename, &st);
    char b[MAXLINE];
    sprintf(b,"%ld", st.st_size);
    return st.st_size;
  }
}