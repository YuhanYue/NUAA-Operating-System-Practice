#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFFERSIZE 4096

int main(int argc, char* argv[]) {
    if(argc < 2){
        printf("INPUT ERROR!\n");
        exit(-1);
    }
    
    int fp;
    fp = open(argv[1], O_RDONLY|O_CREAT);
    if (fp == -1) {
        printf("FILE OPEN ERROR!\n");
        exit(-1);
    }
    
    char bf[BUFFERSIZE];
    int i =0;
    while((i = read(fp,bf,BUFFERSIZE))>0){
        if(write(STDOUT_FILENO, bf, i) != i)
            printf("PRINT ERROR!\n");
    }
    printf("\n");
    
    close(fp);
    
}

