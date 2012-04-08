#include "apue.h"
#include <stdio.h>
#include <time.h>

int main(int argc,char *argv[])
{
	time_t stime;
	char *ct;
	char str;
	int len,i=1;
	if((stime=time(NULL))==-1)
	{
		err_quit("Error read the System time.\n");
	}
	ct=ctime(&stime);
	printf("%s",ct);
	len=sizeof(ct);
	do{
		str=fgetc(&ct);
		if(ct=NULL){
			printf("That is include the null.");
			break;
		}
		i++;
	}while(i<=len);
	exit(0);
}

