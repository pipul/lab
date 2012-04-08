#include "apue.h"

static void sig_quit(int);

int main(void)
{
	sigset_t newmask,oldmask,pendmask;

	if(signal(SIGQUIT,sig_quit)==SIG_ERR)
		err_sys("can't catch SIGQUIT.\n");

	sigemptyset(&newmask);
	sigaddset(&newmask,SIGQUIT);
	sigemptyset(&oldmask);
	sigaddset(&oldmask,SIGALRM);
	if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0)
		err_sys("SIG_BLOCK error.\n");
	if(sigprocmask(SIG_BLOCK,&newmask,&pendmask)<0)
		err_sys("pendmask error.\n");

	if(sigismember(&pendmask,SIGQUIT) && sigismember(&pendmask,SIGALRM))
		printf("newmask is the new .\n");
	else if(sigismember(&oldmask,SIGQUIT) && sigismember(&oldmask,SIGALRM))
		printf("oldmask is the new .\n");
	else printf("oldmask and newmask not the new.\n");


	sleep(5);

	if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0)
		err_sys("SIG_BLOCK error.\n");
	if(sigismember(&pendmask,SIGQUIT))
		printf("\nSIGQUIT pending.\n");

	if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0)
		err_sys("SIG_SETMASK error.\n");
	printf("SIGQUIT unblocked.\n");
	sleep(5);
	exit(0);
}

static void sig_quit(int signo)
{
	printf("caught SIGQUIT\n");
	if(signal(SIGQUIT,SIG_DFL)==SIG_ERR)
		err_sys("can't reset SIGQUIT");
}
