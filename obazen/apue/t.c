#include <stdio.h>

int main(void)
{
	int c;
	while((c=fgetc(stdin))!=10){
		printf("%d\n",c);
	}
	if(c=10) printf("EOF");
}
