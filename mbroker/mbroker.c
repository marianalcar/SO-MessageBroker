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
#include <pthread.h>
#include "producer-consumer.h"


typedef struct{
    char name[32];
    uint64_t n_publisher;
    uint64_t n_subscribers;
}box;

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
        if( strcmp(name_box, boxes[i].name) == 0) {
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

void publisher_handle(int rx, char *box_name, char *pipe_name){
    char message[1025];
    if (read(rx, pipe_name,256) == -1){
        return;
    };
    if (read(rx, box_name, 32) == -1){
        return;
    };
    //printf("%s\n",pipe_name);
    if (check_if_has_publisher(box_name) == -1){
        return;
    }
    int p = open(pipe_name, O_RDONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return;
    }
    

    while(1) {
        if(read(p, message,1024) == -1){
            return;
        }
        int i = tfs_open(box_name,0);
        tfs_write(i, message, 1024);
        tfs_close(i);
    }
}   

void subscriber_handle(int rx, char *box_name, char *pipe_name){
    char message[1025];
    if (read(rx, pipe_name,256) == -1){
        return;
    }
    if (read(rx, box_name, 32) == -1){
        return;
    }
    int i = tfs_open(box_name, 0);
    tfs_read(i, message, 1024);
    int p = open(pipe_name, O_WRONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return;
    }
    if (write(p,message,1024) == -1){
        return;
    }
    return;
 }

void create_box_handle(int rx, char *box_name, char *pipe_name){
    char message_error[1057];
    memset(message_error, '\0', 1057);
    if(read(rx, pipe_name,256) == -1){
        return;
    }
    if(read(rx, box_name, 32) == -1){
        return;
    }

    if (count == 64){
        return;
    }

    int p = open(pipe_name, O_WRONLY);
    if (p == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        return;
    }
    if(check_new_box(box_name) == -1) {
        fill_string("4", message_error ,0);
        fill_string("-1", message_error, 1);   
        fill_string("box_name already existed", message_error, 3);
        message_error[1056] = '\0';
        if(write(p, message_error,1057) == -1){
            return;
        }
        return;
    }
    
    box b;
    strcpy(b.name, box_name);
    b.n_publisher = 1;
    int fh = tfs_open(box_name, TFS_O_CREAT);
    if (fh == -1) {
        fill_string("4", message_error ,0);
        fill_string("-1", message_error, 1);  
        fill_string("open failed\n", message_error, 3);
        message_error[1056] = '\0'; 
        if (write(p, message_error,1057) == -1){
            return;
        }
        close(p);
        return;
    }
    //works
    boxes[count] = b;
    count ++;
    tfs_close(fh);
    fill_string("4", message_error ,0);
    fill_string("0", message_error, 1);  
    message_error[1056] = '\0'; 
    if (write(p, message_error,1057) == -1){
        return;
    }
    fprintf(stdout, "OK\n");
    close(p);

}


void remove_box_handler(int rx, char *box_name, char *pipe_name){
    char message_error[1057];
    memset(message_error, '\0', 1057);
    if(read(rx, pipe_name,256) == -1){
        return;
    }
    if (read(rx, box_name, 32) == -1){
        return;
    }
    int p = open(pipe_name, O_WRONLY);
    if (p == -1) {
        fill_string("6", message_error ,0);
        fill_string("0", message_error, 1);  
        fill_string("[ERR]: open failed", message_error, 2); 
        message_error[1056] = '\0';
        if (write(p, message_error,1057)== -1){
            return;
        }
        return;
    }

    int fh = tfs_open(box_name, TFS_O_CREAT);

    if (fh == -1) {
        fill_string("6", message_error ,0);
        fill_string("0", message_error, 1);  
        fill_string("[ERR]: open failed", message_error, 2); 
        message_error[1056] = '\0';
        if (write(p, message_error,1057) == -1){
            return;
        }
        return;
    }
    

    tfs_close(fh);
    for(int i = 0; i < count; i++) {
        if(strcmp(box_name, boxes[i].name) == 0) {
            removeElement(boxes,count,i);
            inode_delete(get_open_file_entry(fh) -> of_inumber);
            remove_from_open_file_table(fh);
            count--;
            fprintf(stdout, "OK\n");
            return;
        }
    }

    
    fill_string("6", message_error ,0);
    fill_string("0", message_error, 1); 
    message_error[1056] = '\0';  
    if (write(p, message_error,1057) == -1){
        return;
    }
}

int compare(const void *a, const void *b){
    box* box1 = (box*)a;
    box* box2 = (box*)b;
    return strcmp(box1->name, box2->name);
}

void listing(){
    if (count == 0){
        fprintf(stdout, "NO BOXES FOUND\n");
        return;
    }
    qsort(boxes, 64, sizeof(box),compare);
    for(int i = 0; i < count; i++) {
        fprintf(stdout, "%s %zu %zu %zu\n", boxes[i].name, 
        sizeof(box), boxes[i].n_publisher, boxes[i].n_subscribers);
    }
    return;
}

int main(int argc, char **argv) {
    (void)argc;
    char pipe_name[256];
    char box_name[32];
    char char_code[1024];

    pc_queue_t queue;
    pcq_create(&queue,100);


    uint8_t code;
    tfs_init(NULL);
    
    if(argc < 3 || argc > 6) {
        return -1;

    }


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
            break;;
        }

        code = (uint8_t)atoi(char_code);
        
        switch(code) {
            case 1:
                publisher_handle(rx, box_name, pipe_name);
                break;
            case 2:
                subscriber_handle(rx, box_name, pipe_name);
                break;

            case 3:
                create_box_handle(rx, box_name, pipe_name);
                break;
                
            case 5:
                remove_box_handler(rx, box_name, pipe_name);
                break;

            case 7:
                
                break;

            default:
                break;
        }
    }
    pcq_destroy(&queue);
    return 0;

}

