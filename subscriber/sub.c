#include "logging.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void fill_string(char* input_string, char* dest,size_t size) {
    memset(dest, '\0', size);
    memcpy(dest, input_string, strlen(input_string));
}

int main(int argc, char **argv) {
    char register_pipe_name[256];
    char pipe_name[256];
    char box_name[32]; 

    if (argc != 4) {
        fprintf(stderr, "usage: sub <register_pipe_name> <box_name>\n");
        return -1;
    }

    fill_string(argv[1], register_pipe_name,256);
    fill_string(argv[2], pipe_name,256);
    fill_string(argv[3], box_name,32);
    

    int tx = open(register_pipe_name, O_WRONLY);
    char text[289];
    strncpy(text,"2",1);
    strncpy(text + 1, pipe_name,256);
    strncpy(text + 257, box_name, 32);
    write(tx,text,289);

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
    int rx = open(pipe_name, O_RDONLY);
    if (rx == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }
    close(tx);
    unlink(pipe_name);
    return 0;
}
