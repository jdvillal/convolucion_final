#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

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

int main(){
    char source[] = "demoIn.pgm";
    for(int i = 0; i < 10; i ++){
        char prefix[100];
        char i_str[10];
        sprintf(i_str,"%d",i+1);
        strcpy(prefix,i_str);
        strcat(prefix,source);
        int cp_n;
        if ((cp_n = cp(source, prefix))== 0){
            printf("SUCCESS\n");
        }else if(cp_n == -1){
            printf("FAIL S\n");
        }else if(cp_n == -2){
            printf("FAIL D\n");
        }
    }
}
