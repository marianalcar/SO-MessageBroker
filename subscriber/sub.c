#include "logging.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void fill_string(char* input_string, char* dest, int i) {
    memcpy(dest + i, input_string, strlen(input_string));
}


/*int sigint_handler(int signo) {
    return 0;
}*/

int main(int argc, char **argv) {
    char register_pipe_name[256];
    char pipe_name[256]; 

    if (argc != 4) {
        fprintf(stderr, "usage: sub <register_pipe_name> <box_name>\n");
        return -1;
    }

    int tx = open(register_pipe_name, O_WRONLY);
    if (tx == -1 || tx == EOF) {
        printf("mariana talvez seja gay\n");
        return -1;
    }
    char text[289];
    memset(text, '\0', 289);

    fill_string("2", text , 0);
    fill_string(argv[2], text, 1);   
    fill_string(argv[3], text, 257); 
    text[288] = '\0';
    if (write(tx,text,289) == -1){
        printf("mariana é gay\n");
        return -1;
    };

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
    // this waits for someone to open it for reading
    
    int p = open(pipe_name, O_RDONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }

    //signal(SIGINT,sigint_handler);

    char message[1024];

    while (1) {
        ssize_t ret = read(p, message, 1024 - 1);
        if (ret == 0) {
            fprintf(stderr, "[INFO]: pipe closed\n");
            break;
        } else if (ret == -1) {
            close(tx);
            close(p);
            unlink(pipe_name);
            //sigint_handler;
        }
    }

    close(tx);
    unlink(pipe_name);
    return 0;
}