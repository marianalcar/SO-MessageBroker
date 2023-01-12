#include "logging.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char register_pipe_name[256];
    char pipe_name[256];
    char box_name[256]; 
    if (argc != 4) {
        fprintf(stderr, "usage: sub <register_pipe_name> <box_name>\n");
        return -1;
    }
    strncpy(register_pipe_name,argv[1],256);
    strncpy(pipe_name,argv[2],256);
    strncpy(box_name,argv[3],256);


    int tx = open(register_pipe_name, O_WRONLY);
    char text[289];
    strncpy(text,"2",1);
    strncpy(text, pipe_name,256);
    strncpy(text, box_name, 32);
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
