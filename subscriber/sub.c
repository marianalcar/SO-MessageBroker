#include "logging.h"

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
    return -1;
}
