#include "logging.h"
#include "state.h"
#include "operations.h"
#include <string.h>
#include <fcntl.h> 
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include "state.h"


typedef struct{
        char name[32];
        int number_subscribers;
        uint64_t n_publisher;
        uint64_t n_subscribers;
}box;


//TODO: CHECK MAX NUMBER OF BOXES
box boxes[64];
int count = 0;

void fill_string(char* input_string, char* dest, int i) {
    memcpy(dest + i, input_string, strlen(input_string));
}

void removeElement(box array[], int size, int index) {
    if (index >= size) {
    return;
    }
    for (int i = index; i < size - 1; i++) {
        array[i] = array[i + 1];
    }
}


int check_new_box(char* name_box) {
    for(int i = 0; i < count; i++) {
        if( strcmp(name_box, boxes[i].name)) {
            return -1;
        }
    }
    return 0;
}

int check_if_has_publisher(char* name_box) {
    for(int i = 0; i < count; i++) {
        if( strcmp(name_box, boxes[i].name)) {
            if(boxes[i].n_publisher == 1){
                return -1;
            }
        }
    }
    return 0;
}

int publisher_handle(int rx, char *box_name, char *pipe_name){
    if (read(rx, pipe_name,256) == -1){
        return -1;
    };
    if (read(rx, box_name, 32) == -1){
        return -1;
    };
    //printf("%s\n",pipe_name);
    if (check_if_has_publisher(box_name) == -1){
        return -1;
    }
    int p = open(pipe_name, O_RDONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }

    /*    while(1) {
        read(p, message,1024);
        printf("%s\n",meessage);
                }*/
    return 0;
}

int subscriber_handle(int rx, char *box_name, char *pipe_name){
    if (read(rx, pipe_name,256) == -1){
        return -1;
    };
    if (read(rx, box_name, 32) == -1){
        return -1;
    };
    int p = open(pipe_name, O_RDONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;;
    }
    return 0;
}

int create_box_handle(int rx, char *box_name, char *pipe_name){
    char message_error[1057];
    memset(message_error, '\0', 1057);
    read(rx, pipe_name,256);
    read(rx, box_name, 32);

    int p = open(pipe_name, O_WRONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }
    if(check_new_box(box_name) == -1) {
        fill_string("4", message_error ,0);
        fill_string("-1", message_error, 1);   
        fill_string("box_name already existed", message_error, 3);
        message_error[1056] = '\0';
        write(p, message_error,1057);
        return -1;
    }
    
    box b;
    strncpy(b.name, box_name,32);
    b.n_publisher = 1;
    int fh = tfs_open(box_name, TFS_O_CREAT);
    if (fh == -1) {
        fill_string("4", message_error ,0);
        fill_string("-1", message_error, 1);  
        fill_string("open failed\n", message_error, 3);
        message_error[1056] = '\0'; 
        write(p, message_error,1057);
        close(p);
        return -1;
    }
    boxes[count] = b;
    count ++;
    tfs_close(fh);
    fill_string("4", message_error ,0);
    fill_string("0", message_error, 1);  
    message_error[1056] = '\0'; 
    write(p, message_error,1057);
    close(p);
    return 0;;
}

int remove_box_handler(int rx, char *box_name, char *pipe_name){
    char message_error[1057];
    memset(message_error, '\0', 1057);
    read(rx, pipe_name,256);
    read(rx, box_name, 32);
    int p = open(pipe_name, O_WRONLY);
    if (p == -1) {
        fill_string("6", message_error ,0);
        fill_string("0", message_error, 1);  
        fill_string("[ERR]: open failed", message_error, 2); 
        message_error[1056] = '\0';
        write(p, message_error,1057);
        return -1;
    }
    int fh = tfs_open(box_name, TFS_O_CREAT);

    if (fh == -1) {
        fill_string("6", message_error ,0);
        fill_string("0", message_error, 1);  
        fill_string("[ERR]: open failed", message_error, 2); 
        message_error[1056] = '\0';
        write(p, message_error,1057);
        return -1;
    }

    tfs_close(fh);
    for(int i = 0; i < count; i++) {
        if( strcmp(box_name, boxes[i].name)) {
            removeElement(boxes,count,i);
            return 0;;
        }
    }
    inode_delete(get_open_file_entry(fh) -> of_inumber);
    remove_from_open_file_table(fh);
    count--;
    
    fill_string("6", message_error ,0);
    fill_string("0", message_error, 1); 
    message_error[1056] = '\0';  
    write(p, message_error,1057);
    return -1;
}

int main(int argc, char **argv) {
    (void)argc;
    char pipe_name[256];
    char box_name[32];
    char char_code[1024];
    char message[1024];

    int p,fh;
    uint8_t code;
    tfs_init(NULL);
    //int max_sessions;
    
    if(argc < 3 || argc > 6) {
        return -1;

    }

    //fill_string(argv[1], register_pipe_name,256);
    //max_sessions = atoi(argv[2]);

    if (unlink(argv[1]) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", argv[1],
                strerror(errno));
        return -1;
    }

    // create pipe
    if (mkfifo(argv[1], 0640) != 0) {
        fprintf(stderr, "[ERR]: mkfifo failed: %s\n", strerror(errno));
        return -1;
    }

    // open pipe for reading
    int rx = open(argv[1], O_RDONLY);
    if (rx == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return -1;
    }

    while (1) {

        if (read(rx,char_code,1) == -1){
            fprintf(stderr, "[ERR]: read failed: %s\n", strerror(errno));
            return -1;
        }

        code = (uint8_t)atoi(char_code);

        switch(code) {
            case 1:
                if (publisher_handle(rx, box_name, pipe_name) == -1){
                    break;
                }
                break;

            case 2:
                if (subscriber_handle(rx, box_name, pipe_name) == -1){
                    break;
                }
                break;

            case 3:
                if (create_box_handle(rx, box_name, pipe_name) == -1){
                    break;
                }
                break;
            
            case 5:
                if (remove_box_handle(rx, box_name, pipe_name) == -1){
                    break;
                }
                break;

            case 7:
                for(int i = 0; i < count; i++) {
                    printf("%s \n", boxes[i].name);
                }
                break;

            case 8:
                break;
            default:
                break;
        }
    }
    return 0;

}

