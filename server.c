#include <getopt.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h> // for open
#include <unistd.h>

#include "common.h"

void escuchar_cliente(int connfd);
long validateFile(char* filename);

void print_help(char *command)
{
  printf("Procesador (sharpen) de imagenes multiples\n");
  printf("uso:\n %s <puerto> <ruta/nombre listado de imagenes>\n", command);
  printf(" %s -h\n", command);
  printf("Opciones:\n");
  printf(" -h\t\t\tAyuda, muestra este mensaje\n");
}

int main(int argc, char **argv)
{
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
        print_help(argv[0]);
        return 0;
      default:
        fprintf(stderr, "uso: %s <puerto>\n", argv[0]);
        fprintf(stderr, "     %s -h\n", argv[0]);
        return -1;
    }
  }

  if(argc != 3){
    fprintf(stderr, "uso: %s <puerto> <ruta/nombre listado de imagenes>\n", argv[0]);
    fprintf(stderr, "     %s -h\n", argv[0]);
    return -1;
  }else{
    port = argv[1];//puerto en el que escuchara al cliente
    ruta_lista_imagenes = argv[2];//nombre o ruta del archivo txt con la lista de imagenes
  }
  //Valida el puerto
  int port_n = atoi(port);
  if(port_n <= 0 || port_n > USHRT_MAX){
    fprintf(stderr, "Puerto: %s invalido. Ingrese un número entre 1 y %d.\n", port, USHRT_MAX);
    return -1;
  }
  //Valida si el archivo existe
  if(validateFile(ruta_lista_imagenes) == 0){
    fprintf(stderr, "El archivo <<%s>> no ha sido encontrado\n", ruta_lista_imagenes);
    return -1;
  }
  printf("Archivo <<%s>> encontrado\n", ruta_lista_imagenes);


  //Abre un socket de escucha en puerto indicado por el usuario
  listenfd = open_listenfd(port);
  if(listenfd < 0)
    connection_error(listenfd);

  printf("Escuchando instrucciones en el puerto %s...\n", port);

  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

    /* Determina el nombre dominio y la IP del cliente */
    hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    haddrp = inet_ntoa(clientaddr.sin_addr);

    printf("server conectado a %s (%s)\n", hp->h_name, haddrp);
    escuchar_cliente(connfd);
    printf("server desconectando de %s (%s)\n", hp->h_name, haddrp);

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
    for(int i = 0; i < strlen(buf); i++){
      if(*(buf+i)=='\n'){
        memset(buf+i,0,1);
        break;
      }
    }
    long filesize = validateFile(buf);
    char fsizeChar[MAXLINE] = {0};
    sprintf(fsizeChar,"%ld", filesize);
    n = write(connfd, fsizeChar, strlen(fsizeChar));//Envia el tamaño del archivo al cliente
    if(n <= 0)
      return;
    if(filesize>0){
      /*Si el archivo ingresado por el cliente existe, se agrega a la cola*/
    }
        memset(buf, 0, MAXLINE);
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

