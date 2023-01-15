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

volatile sig_atomic_t flag = 1;

void sigint_handler(int signo) {
    (void)signo;
    flag = 0;
}

int main(int argc, char **argv) {
    

    if (argc != 4) {
        fprintf(stderr, "usage: sub <register_pipe_name> <box_name>\n");
        return -1;
    }


    char text[289];
    memset(text, '\0', 289);

    fill_string("2", text , 0);
    fill_string(argv[2], text, 1);   
    fill_string(argv[3], text, 257); 
    text[288] = '\0';

    int tx = open(argv[1], O_WRONLY);
    if (tx == -1 || tx == EOF) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }

    if (write(tx,text,289) == -1){
        return -1;
    };

    // remove pipe if it does exist
    if (unlink(argv[2]) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", argv[2],
                strerror(errno));
        return -1;
    }

    // create pipe
    if (mkfifo(argv[2], 0640) != 0) {
        fprintf(stderr, "[ERR]: mkfifo failed: %s\n", strerror(errno));
        return -1;
    }

    // open pipe for writing
    // this waits for someone to open it for reading
    
    int p = open(argv[2], O_RDONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }

    signal(SIGINT,sigint_handler);

    char message[1024];

    while (flag) {
        ssize_t ret = read(p, message, 1024 - 1);
        if (ret == 0) {
            fprintf(stderr, "[INFO]: pipe closed\n");
            break;
        } else if (ret == -1){
            break;
        }
    }

    close(tx);
    close(p);
    unlink(argv[2]);
    return 0;
}