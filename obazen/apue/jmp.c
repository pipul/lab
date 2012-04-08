#include "apue.h"
#include <setjmp.h>
static jmp_buf env;

int main(void)
{
	int count=1;
	setjmp(env);

	sleep(1);
	printf("This is the %d time.\n",count);
	count++;

	longjmp(env,1);
	exit(0);
}
