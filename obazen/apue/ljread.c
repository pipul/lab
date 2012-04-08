#include "apue.h"
#include <setjmp.h>

static void sig_alrm(int);
static jmp_buf env_alrm;
static int count=1;

int main(void)
{
	int k,n;
	char line[MAXLINE];
	if(signal(SIGALRM,sig_alrm)==SIG_ERR)
		err_sys("signal(SIGALRM) error.\n");
	if(setjmp(env_alrm)!=0){
		printf("read error.\n");
	}
	alarm(10);
	printf("This is the %d times.\n",count);
	if((n=read(STDOUT_FILENO,line,MAXLINE))<0)
		err_sys("read error.\n");
	alarm(0);

	write(STDOUT_FILENO,line,n);
	exit(0);
}

static void sig_alrm(int signo)
{
	count++;
	longjmp(env_alrm,1);
}
