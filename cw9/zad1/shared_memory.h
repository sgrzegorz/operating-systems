
#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H



#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define W(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}
#define MAX_BUFFER_SIZE 4096;
#define PROJECT_ID 12

typedef struct{
    char* array[MAX_BUFFER_SIZE];
    int consume_i ,produce_i, nelements, size;
    
}Buffer;





int getSize(Fifo * fifo);

int push(Fifo *fifo,int val);

int pop(Fifo * fifo);

void init(Fifo * fifo);

int isEmpty(Fifo * fifo);

int isFull(Fifo * fifo);
long getTime();



#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#endif //SHAREDMEMORY_H
