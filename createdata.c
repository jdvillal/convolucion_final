#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
struct stat st = {0};

/*   ** Function return value meaning
 * -1 cannot open source file 
 * -2 cannot open destination file
 * 0 Success
 */


int cp(char FileSource [], char FileDestination [])
{
    int   c;
    FILE *stream_R;
    FILE *stream_W; 

    stream_R = fopen (FileSource, "r");
    if (stream_R == NULL)
        return -1;
    stream_W = fopen (FileDestination, "w");   //create and write to file
    if (stream_W == NULL)
     {
        fclose (stream_R);
        return -2;
     }    
    while ((c = fgetc(stream_R)) != EOF)
        fputc (c, stream_W);
    fclose (stream_R);
    fclose (stream_W);

    return 0;
}

int main(int argc, char **argv){
    int n_copias;
    if(argc != 2){
		fprintf(stderr, "uso:\t%s <numero_copias> \n", argv[0]);
		return -1;
    }else{
        n_copias = atoi(argv[1]);
    }


    FILE *f = fopen("lista.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }


    if (stat("./images", &st) == -1) {
        mkdir("./images", 0700);
    }

    char source[] = "demoIn.pgm";
    for(int i = 0; i < n_copias; i ++){
        char prefix[100] = "images/";
        char i_str[10];
        sprintf(i_str,"%d",i+1);
        strcat(prefix,i_str);
        strcat(prefix,source);
        cp(source, prefix);
        fprintf(f, "%s\n", prefix);
    }
    fclose(f);
}
