#include "logging.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void fill_string(char* input_string, char* dest, size_t size) {
    memset(dest, '\0', size);
    memcpy(dest, input_string, strlen(input_string));
}



int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    char register_pipe_name[256];
    int max_sessions;
    char char_code;
    char register_pipe_name[256];
    char pipe_name[256];
    char box_name[32];
    char char_code[1024];
    char full_message[1025];
    char message[1024];
    char code_uno[1];
    int code9, p;
    int code;
    int max_sessions;

    if (argc != 1) {
        return -1;
    }

    fill_string(argv[1], register_pipe_name,256);
    max_sessions = atoi(argv[2]);

    



    if (unlink(register_pipe_name) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", register_pipe_name,
                strerror(errno));
        return -1;
    }

    // create pipe
    if (mkfifo(register_pipe_name, 0640) != 0) {
        fprintf(stderr, "[ERR]: mkfifo failed: %s\n", strerror(errno));
        return -1;
    }

    // open pipe for reading
    int rx = open(register_pipe_name, O_RDONLY);
    if (rx == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }
    while (1) {

        read(rx,char_code,1);
        code = atoi(char_code);
        switch(code) {
            case 1:

                sscanf(char_code, "%1c%256s%32s", &code_uno[1], &pipe_name[256], &box_name[32]);
                p = open(pipe_name, O_RDONLY);
                if (p == -1) {
                    fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
                    return -1;
                }
                read(p, full_message, 1025);
                sscanf(full_message,"%1d%1024s", &code9, &message[1024]);




            case 2:

                sscanf(char_code, "%1c%256s%32s", &code_uno[1], &pipe_name[256], &box_name[32]);
                p = open(pipe_name, O_RDONLY);
                if (p == -1) {
                    fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
                    return -1;
                }




            case 3:

                sscanf(char_code, "%1c%256s%32s", &code_uno[1], &pipe_name[256], &box_name[32]);
                p = open(pipe_name, O_RDONLY);
                if (p == -1) {
                    fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
                    return -1;
                }
                read(p, full_message, 1025);
                sscanf(full_message,"%1d%1024s", &code9, &message[1024]);

            case 4:

            case 5:

            case 6:

            case 7:

            case 8:
                break;

        }
    }*/
    return 0;

}
