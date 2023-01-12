#include "logging.h"
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

 void fill_string(char* input_string, char* dest) {
    int input_string_len = strlen(input_string);
    memset(dest, '\0', sizeof(input_string));
    memcpy(dest, input_string, input_string_len);
}

int main(int argc, char **argv) {
    char register_pipe_name[256];
    char pipe_name[256];
    char box_name[256]; 
    if (argc != 4) {
        fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");
        return -1;
    }

    fill_string(argv[1], register_pipe_name);
    fill_string(argv[2], pipe_name);
    fill_string(argv[3], pipe_name);

    int rp = open(register_pipe_name, O_WRONLY);
    char text[289];
    strncpy(text,"2",1);
    
    strncpy(text, pipe_name,256);
    
    strncpy(text, box_name, 32);
    write(rp,text,289);

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
    int p = open(pipe_name, O_WRONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }

    char message[1024];

    while (scanf("%1024s", message) != EOF){
        write(p, message, 1024);
    }

    close(rp);
    unlink(pipe_name);
    
    return 0;
}
