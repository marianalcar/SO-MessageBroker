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
    //char register_pipe_name[256];
    int p;
    if (argc != 4) {
        fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");
        return -1;
    }  
    
    char text[289];
    memset(text, '\0', 289);

    fill_string("1", text ,0);
    fill_string(argv[2], text, 1);   
    fill_string(argv[3], text, 257);
    text[288] = '\0';

    int rp = open(argv[1], O_WRONLY);
    if(rp == -1 || rp == EOF) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }
    if(write(rp,text,289) == -1) {
        fprintf(stderr, "[ERR]: write failed: %s\n", strerror(errno));
        return -1;
    }
    char test[256];
    memset(test, '\0', 256);
    fill_string(argv[2],test, 0);
    // remove pipe if it does exist
    if (unlink(test) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", argv[2],
                strerror(errno));
        return -1;
    }
    
    // create pipe
    if (mkfifo(test, 0640) != 0) {
        fprintf(stderr, "[ERR]: mkfifo failed: %s\n", strerror(errno));
        return -1;
    }
    printf("%s\n",argv[2]);

    // open pipe for writin

    
    p = open(test, O_WRONLY);

    if (p == -1 || p == EOF) {
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
    unlink(argv[2]);
    
    return 0;
}
