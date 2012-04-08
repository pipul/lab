#include "apue.h"
#include <setjmp.h>
static void sig_alrm(int signo)
{
	//printf("Catch the signal.\n");
	//exit(0);
} 

int main(void)
{
	static jmp_buf env;
	int n=1;

	setjmp(env);

	if(signal(SIGALRM,sig_alrm)==SIG_ERR)
		err_sys("signal error.\n");
	
	alarm(4);
	printf("This is the %d times.\n",n++);
	printf("This is the %d times.\n",n++);
	
	pause();
	longjmp(env,1);
}
	
