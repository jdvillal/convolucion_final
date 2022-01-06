#include <getopt.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"

#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print_help(char *command){//imprime por consola mensaje de ayuda de la opcion -h
    printf("Cliente simple de descarga de archivos.\n");
    printf("uso:\n %s <hostname> <puerto>\n", command);
    printf(" %s -h\n", command);
    printf("Opciones:\n");
    printf(" -h\t\t\tAyuda, muestra este mensaje\n");
}

void print_error(){//imprime por consola mensaje de error
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

int main(int argc, char **argv)
{
    int opt;

    //Socket
    int clientfd;
    //Direcciones y puertos
    char *port;

    //Lectura desde consola
    char *linea_consola;
    char read_buffer[MAXLINE] = {0};
    size_t max = MAXLINE;
    ssize_t n, l = 0;

    while ((opt = getopt (argc, argv, "h")) != -1){
        switch(opt)
        {
            case 'h':
                print_help(argv[0]);
                return 0;
            default:
                fprintf(stderr, "uso: %s <hostname> <puerto>\n", argv[0]);
                fprintf(stderr, "     %s -h\n", argv[0]);
                return -1;
        }
    }

    if(argc != 2){
        fprintf(stderr, "uso: %s <puerto>\n", argv[0]);
        fprintf(stderr, "     %s -h\n", argv[0]);
        return -1;
    }else{
        port = argv[1];
    }

    //Valida el puerto
    int port_n = atoi(port);
    if(port_n <= 0 || port_n > USHRT_MAX){
        fprintf(stderr, "Puerto: %s invalido. Ingrese un número entre 1 y %d.\n", port, USHRT_MAX);
        return -1;
    }

    //Se conecta al servidor retornando un socket conectado
    clientfd = open_clientfd("127.0.0.1", port);

    if(clientfd < 0)
        connection_error(clientfd);

    printf("Conectado exitosamente a localhost en el puerto %s.\n", port);

    linea_consola = (char *) calloc(1, MAXLINE);
    l = 1;
    while(l > 0){
        printf(ANSI_COLOR_CYAN"sharpen_client>>"ANSI_COLOR_RESET);
        l = getline(&linea_consola, &max, stdin); //lee desde consola
        if(strcmp(linea_consola,"exit\n")==0){//Si el usuario ingresa el comando exit, se termina la ejecucion del programa
            break;
        }else{//Si el comando no es "exit" se verifica si es "add", "threads" o un comando invalido
            const char s[2] = " ";
            char *token;
            token = strtok(linea_consola, s);
            if( token != NULL ) {
                char to_send[MAXLINE];
                if(strcmp(token,"add")==0){//si el comando es "add"
                    token = strtok(NULL, s);
                    if(token != NULL){
                        remove_ln(token);
                        sprintf(to_send, "add %s", token);
                        n = write(clientfd, to_send, MAXLINE); //Envia el comando al servidor
                        if(n<=0)
                            break;
                        n = read(clientfd, read_buffer, MAXLINE); //Lee respuesta del servidor
                        if(n<=0)
                            break;
                        long filesize = atoi(read_buffer);
                        if(filesize==0){
                            printf("ERROR: El archivo ingresado no existe\n");
                        }else{
                            printf("EL archivo %s ha sido ingresado a la cola de procesamiento\n", token);
                        }
                    }
                }else if(strcmp(token, "threads")==0){//si el comando es "threads"
                    token = strtok(NULL, s);
                    if(token != NULL){
                        sprintf(to_send, "threads %d\n", atoi(token));
                        n = write(clientfd, to_send, MAXLINE); //Envia el comando al servidor
                        if(n<=0)
                            break;
                        n = read(clientfd, read_buffer, MAXLINE); //Lee respuesta del servidor
                        if(n<=0)
                            break;
                        long n_prev_threads = atoi(read_buffer);
                        printf("Cantidad de hilos de procesamientos cambiada con exito de %ld a %s", n_prev_threads, token);
                    }
                }else{//si el comando es invalido
                    print_error();
                }
            }
        }
        memset(read_buffer, 0, MAXLINE);
    }
    printf("Desconectando...\n");
    free(linea_consola);
    close(clientfd);
}
