#include "apue.h"
#include <sys/wait.h>

int main(void)
{
	pid_t pid;

	if((pid=fork())<0){
		err_sys("fork error.\n");
	}else if(pid==0){
		sleep(2);
		printf("This is my first run.I am a child.\n");
	}else{
		waitpid(pid,NULL,WNOHANG);
		printf("I am your father.\n");
	}

	exit(0);
}
