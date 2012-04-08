#include "apue.h"
#include <setjmp.h>

int main(void)
{
	sigset_t newmask;
	if(sigprocmask(0,NULL,&newmask)<0)
		err_sys("sigprocmask error.");
	sigemptyset(&newmask);
	pr_mask("The front sigset: ");
	sigaddset(&newmask,SIGQUIT);
	sigprocmask(SIG_SETMASK,&newmask,NULL);
	pr_mask("The follow sigset: ");
}
