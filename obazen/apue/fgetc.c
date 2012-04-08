#include "apue.h"
#include <stdio.h>

int main(int argc,char *argv[])
{
	char str;
	while(str!='v')
	{
		str=fgetc(stdin);
		if(str!='\n')
			printf("%c",str);
		else printf(".");
	}
	exit(0);
}
