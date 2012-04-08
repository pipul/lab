#include <stdio.h>

int foo(int *p)
{
	int a=35;
	p=&a;
	return(0);
}

int main(void)
{
	int m=2222335;
	int *t=&m;
	printf("%p\n",t);
	foo(t);
	printf("%p\n",t);
	return(0);
}
