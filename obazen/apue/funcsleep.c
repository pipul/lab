#include "apue.h"
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf env_alrm;

static void sig_alrm(int signo)
{
	longjmp(env_alrm,1);
}

unsigned int sleep(unsigned int nsecs){
	if(signal(SIGALRM,sig_alrm)==SIG_ERR)
		return(nsecs);
	if(setjmp(env_alrm)==0){
		alarm(nsecs);
		pause();
	}
	return(alarm(0));
}

int main(void)
{
	//atexit(exitpr);
	pid_t pid;
	int status;

	signal(SIGALRM,sig_alrm); /* establish the signal */
	alarm(2);
	
	if((pid=fork())<0)
		err_sys("fork error.\n");
	else if(pid==0){
		sleep(4);
		exit(0);
	}else{
		alarm(6);
		if(waitpid(pid,NULL,0)<0)
			err_sys("wait the pid=%d error.\n",pid);
		else
			printf("wait sucess.\n");
		exit(0);
	}
}
