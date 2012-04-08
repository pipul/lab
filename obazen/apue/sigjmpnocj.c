#include "apue.h"
#include <setjmp.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

static void sig_usr1(int),sig_alrm(int);
static sigjmp_buf jmpbuf;
void pr_mask(const char *);

int main(void)
{
	if(signal(SIGUSR1,sig_usr1)==SIG_ERR)
	  	err_sys("signal(SIGUSR1) error.\n");
	if(signal(SIGALRM,sig_alrm)==SIG_ERR)
		err_sys("signal(SIGALRM) error.\n");

	pr_mask("starting main: ");

	if(sigsetjmp(jmpbuf,1)){
		pr_mask("ending main: ");
		exit(0);
	}

	for(;;)
		pause();
}

static void sig_usr1(int signo)
{
	time_t starttime;

	pr_mask("starting sig_usr1: ");
	alarm(3);
	starttime=time(NULL);
	for(;;)
		if(time(NULL)>starttime+5)
			break;
	pr_mask("finishing sig_usr1: ");

	siglongjmp(jmpbuf,1);
}

static void sig_alrm(int signo)
{
	pr_mask("in sig_alrm: ");
}

void pr_mask(const char *str)
{
	sigset_t sigset;
	int errno_save;

	errno_save=errno;
	if(sigprocmask(0,NULL,&sigset)<0)
		err_sys("sigprocmask error.\n");

	printf("%s .",str);

	if(sigismember(&sigset,SIGABRT)) printf("SIGABRT ");
	if(sigismember(&sigset,SIGALRM)) printf("SIGALRM ");
	if(sigismember(&sigset,SIGCHLD)) printf("SIGCHLD ");
	if(sigismember(&sigset,SIGCONT)) printf("SIGCONT ");
	if(sigismember(&sigset,SIGFPE)) printf("SIGFPE ");
	if(sigismember(&sigset,SIGHUP)) printf("SIGHUP ");
	if(sigismember(&sigset,SIGILL)) printf("SIGILL ");
	if(sigismember(&sigset,SIGINT)) printf("SIGINT ");
	if(sigismember(&sigset,SIGKILL)) printf("SIGKILL ");
	if(sigismember(&sigset,SIGPIPE)) printf("SIGPIPE ");
	if(sigismember(&sigset,SIGQUIT)) printf("SIGQUIT ");
	if(sigismember(&sigset,SIGSEGV)) printf("SIGSEGV ");
	if(sigismember(&sigset,SIGSTOP)) printf("SIGSTOP ");
	if(sigismember(&sigset,SIGTERM)) printf("SIGTERM ");
	if(sigismember(&sigset,SIGTRAP)) printf("SIGTRAP ");
	if(sigismember(&sigset,SIGTSTP)) printf("SIGTSTP ");
	if(sigismember(&sigset,SIGTTIN)) printf("SIGTTIN ");
	if(sigismember(&sigset,SIGTTOU)) printf("SIGTTOU ");

	printf("\n");
	errno=errno_save;
}
