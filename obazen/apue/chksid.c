#include "apue.h"

int main(void)
{
	pid_t pid[5];
	pid[0]=setsid(); //session father process id
	if((pid[1]=fork())<0){
		err_quit("fork error.\n");
	}else if(pid[1]==0){
		sleep(2);
		if((pid[2]=getsid(0))!=-1)
			printf("the child's session id:%d.\n",pid[2]);
		else if(pid[2]==0)
			err_quit("get the session id error.\n");
	}else{
		exit(0);
	}
}
