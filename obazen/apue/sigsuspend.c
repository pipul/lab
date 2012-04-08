#include "apue.h"
#include <setjmp.h>

volatile sig_atomic_t quitflag;

static void sig_int(int signo)
{
	if(signo==SIGUSR1)
		printf("\ninterrupt\n");
	else if(signo==SIGQUIT)
		quitflag=1;
	pr_mask("2: ");
}

int main(void)
{
	sigset_t newmask,oldmask,zeromask;
	if(signal(SIGUSR1,sig_int)==SIG_ERR)
		err_sys("signal(SIGINT) error");
	if(signal(SIGQUIT,sig_int)==SIG_ERR)
		err_sys("signal(SIGQUIT) error");

	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask,SIGQUIT);

	if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0)
		err_sys("SIG_BLOCK error ");
	while(quitflag==0){
		pr_mask("1: ");
		sigsuspend(&zeromask);
	}

	quitflag=0;
	if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0)
		err_sys("SIG_SETMASK error ");

	exit(0);
}
