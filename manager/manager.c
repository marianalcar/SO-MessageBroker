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

static void print_usage() {
    fprintf(stderr, "usage: \n"
                    "   manager <register_pipe_name> create <box_name>\n"
                    "   manager <register_pipe_name> remove <box_name>\n"
                    "   manager <register_pipe_name> list\n");
}

int main(int argc, char **argv) {
    if ( 0 < argc && argc > 5) {
        print_usage();
        return -1;
    }
    char test[256];
    memset(test, '\0', 256);
    fill_string(argv[2],test, 0);
    test[255] = '\0';
    printf("argv %s\n",argv[2]);
    printf("test %s\n",test);

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


    char text[289];
    char message[1057];
    int p;
    memset(text, '\0', 289);
    memset(message, '\0', 1056);

    int rp = open(argv[1], O_WRONLY);
    if(rp == -1 || rp == EOF) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }
    printf("%s\n",argv[2]);
    if(strcmp(argv[3], "create") == 0) {
        fill_string("3", text ,0);
        fill_string(argv[2], text, 1);   
        fill_string(argv[4], text, 257);
        text[288] = '\0';
        if(write(rp,text,289) == -1) {
            fprintf(stderr, "[ERR]: write failed: %s\n", strerror(errno));
            return -1;
        }

    }

    else if(strcmp(argv[3],"remove") == 0) {
        fill_string("5", text ,0);
        fill_string(argv[2], text, 1);   
        fill_string(argv[4], text, 257);
        text[288] = '\0';
        if(write(rp,text,289) == -1) {
            fprintf(stderr, "[ERR]: write failed: %s\n", strerror(errno));
            return -1;
        }
    }

    else if(strcmp(argv[3],"list") == 0) {
        fill_string("7", text ,0);
        fill_string(argv[2], text, 1);   
        fill_string(argv[4], text, 257);
        text[288] = '\0';
        if(write(rp,text,289) == -1) {
            fprintf(stderr, "[ERR]: write failed: %s\n", strerror(errno));
            return -1;
        }
    }

    

    // open pipe for writing
   
    p = open(pipe_final, O_RDONLY);
    if (p == -1 || p == EOF) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }

    
    read(p,message ,1057);
    printf("%s\n",message);

    close(rp);
    close(p);
    unlink(pipe_final);

    return 0;
}
