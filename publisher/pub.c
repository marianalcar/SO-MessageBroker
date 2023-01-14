#include "logging.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void fill_string(char* input_string, char* dest, int i) {
    memcpy(dest + i, input_string, strlen(input_string));
}

int main(int argc, char **argv) {
    char register_pipe_name[256];
    char pipe_name[256];
     
    int p;
    if (argc != 4) {
        fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");
        return -1;
    }  

    int rp = open(register_pipe_name, O_WRONLY);
    if(rp == -1 || rp == EOF) {
        return -1;
    }
    char text[289];
    memset(text, '\0', 289);

    fill_string("2", text ,0);
    fill_string(argv[1], text, 1);   
    fill_string(argv[2], text, 257); 
    text[288] = '\0';

    if(write(rp,text,289) == -1) {
        printf("ola_write\n");
        return -1;
    }

    // remove pipe if it does exist
    if (unlink(pipe_name) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", pipe_name,
                strerror(errno));
        return -1;
    }

    // create pipe
    if (mkfifo(pipe_name, 0640) != 0) {
        fprintf(stderr, "[ERR]: mkfifo failed: %s\n", strerror(errno));
        return -1;
    }


    // open pipe for writing


    p = open(pipe_name, O_WRONLY);
    if (p == -1) {
        printf("ola_open\n");
        return -1;
    }
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }

    char message_full[1025];
    char message[1024];
    

    while (scanf("%s", &message[1024]) != EOF){
        fill_string("9", message_full,0);
        fill_string( message,message_full,1);
        message_full[1024] = '\0';
        if (write(p, message_full, 1025) == -1){
            return -1;
        }
    }
    close(rp);
    close(p);
    unlink(pipe_name);
    
    return 0;
}
