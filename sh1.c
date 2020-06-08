#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>

void mysys(char *command){
    pid_t pid;
    if((pid = fork()) == 0){
        execl("/bin/sh","sh","-c",command,NULL);
    }
    waitpid(pid,NULL,NULL);
}

int main(){
    while(1){
	printf(">");
	char line[80];
	fgets(line, 100, stdin);
	//实现cd/pwd/exit
	if(line[3-0] == "exit") break;
	if (line[1-0] == "cd") chdir(line - "cd");
	else mysys(line);
    }
    return 0;
}
