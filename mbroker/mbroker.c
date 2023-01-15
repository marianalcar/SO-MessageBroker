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
        int have_publisher;

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


bool check_new_box(char* name_box) {
    for(int i = 0; i < count; i++) {
        if( strcmp(name_box, boxes[i].name)) {
            return true;
        }
    }
    return false;
}



int main(int argc, char **argv) {
    (void)argc;
    char pipe_name[256];
    char box_name[32];
    char char_code[1024];
    char message[1024];
    char message_error[1057];
    memset(message_error, '\0', 1057);

    int p, fh;
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

                read(rx, pipe_name,256);
                read(rx, box_name, 32);

                p = open(pipe_name, O_RDWR);
                if (p == -1) {
                    fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
                    return -1;
                }
                while(1) {
                    read(p, message,1024);
                    printf("%s\n",message);
                }

            case 2:

                read(rx, pipe_name,256);
                read(rx, box_name, 32);
                p = open(pipe_name, O_RDONLY);
                if (p == -1) {
                    fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
                    return -1;
                }


            case 3:

                read(rx, pipe_name,256);
                read(rx, box_name, 32);
                p = open(pipe_name, O_RDWR);
                if (p == -1) {
                    fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
                    return -1;
                }

                if(check_new_box(box_name)) {
                    fill_string("4", message_error ,0);
                    fill_string("0", message_error, 1);   
                    fill_string("box_name already existed", message_error, 2);
                    message_error[1056] = '\0';
                    write(p, message_error,1057);
                    break;
                }

                box b;
                strncpy(b.name, box_name,32);
                b.have_publisher = 1;
                fh = tfs_open(box_name, TFS_O_CREAT);
                if (fh == -1) {
                    printf("error mothafucker\n");
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
            case 5:
                // !!!!
                // falta eleminar do array
                read(rx, pipe_name,256);
                read(rx, box_name, 32);
                p = open(pipe_name, O_WRONLY);
                if (p == -1) {
                    fill_string("6", message_error ,0);
                    fill_string("0", message_error, 1);  
                    fill_string("[ERR]: open failed", message_error, 2); 
                    message_error[1056] = '\0';
                    write(p, message_error,1057);
                    return -1;
                }
                fh = tfs_open(box_name, TFS_O_CREAT);

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
                        break;
                    }
                }
                inode_delete(get_open_file_entry(fh) -> of_inumber);
                remove_from_open_file_table(fh);
                count--;


                fill_string("6", message_error ,0);
                fill_string("0", message_error, 1); 
                message_error[1056] = '\0';  
                write(p, message_error,1057);

            case 7:
                for(int i = 0; i < count; i++) {
                    printf("%s \n", boxes[i].name);
                }

            case 8:
                break;
        }
    }
    return 0;

}

