#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFFERSIZE 4096

int main(int argc, char *argv[])
{
    int fpR=0;
    int fpD=0;
    char bf[BUFFERSIZE];
    
    if(argc != 3){
        printf("ERROR\n");
        exit(1);
    }
    
    if((fpR = open(argv[1], O_RDONLY)) == -1){
        printf("CANNOT OPEN\n");
        exit(1);
    }
    
    if((fpD = creat(argv[2],0644)) == -1){
        printf("CANNOT CREATE\n");
        exit(1);
    }
    
    int i=0;
    
    while((i = read(fpR, bf,BUFFERSIZE)) > 0){
        if(write(fpD, bf, i) != i){
            printf("WRITE ERROR\n");
            exit(1);
        }
    }
    
    close(fpR);
    close(fpD);
}

