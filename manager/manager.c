#include "logging.h"
#include <string.h>

void fill_string(char* input_string, char* dest, size_t size) {
    memset(dest, '\0', size);
    memcpy(dest, input_string, strlen(input_string));
}

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
    fill_string(argv[1], register_pipe_name,256);
    fill_string(argv[2], pipe_name,256);


    if(strncmp(argv[3], "create",6)) {
        fill_string(argv[4], box_name,32);
    }

    else if(strncmp(argv[3],"remove",6)) {
        fill_string(argv[4], box_name,32);
    }

    else if(strncmp(argv[3],"list",4)) {
    }

    return 0;
}
