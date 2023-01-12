#include "logging.h"
#include <string.h>

static void print_usage() {
    fprintf(stderr, "usage: \n"
                    "   manager <register_pipe_name> create <box_name>\n"
                    "   manager <register_pipe_name> remove <box_name>\n"
                    "   manager <register_pipe_name> list\n");
}

int main(int argc, char **argv) {
    char register_pipe_name[256];
    char pipe_name[256];
    char box_name[32];
    if ( 0 < argc && argc > 5) {
        print_usage();
        return -1;
    }
    strncpy(register_pipe_name,argv[1],256);
    strncpy(pipe_name,argv[2],256);


    if(strncmp(argv[3], "create",6)) {
        strncpy(box_name,argv[4],32);
    }

    else if(strncmp(argv[3],"remove",6)) {
        strncpy(box_name,argv[4],32);
    }

    else if(strncmp(argv[3],"list",4)) {
    }

    return 0;
}
