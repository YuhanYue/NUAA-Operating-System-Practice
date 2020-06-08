#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<pthread.h>


void mysys(char *command){
    pid_t pid;
    if((pid = fork()) == 0){
        execl("/bin/sh","sh","-c",command,NULL);
    }
    waitpid(pid,NULL,NULL);
}

int main(){
        printf("---------------------------------\n");
        mysys("echo HELLOW WORLD");
        printf("---------------------------------\n");
        mysys("ls /");
        printf("---------------------------------\n");
        return 0;
}


