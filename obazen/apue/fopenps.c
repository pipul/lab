#include "apue.h"
#include <stdio.h>

int main(int argc,char *argv[])
{
	struct FILE *fileps;
	if(argc!=2)
	{
		err_ret("usage:./fileps <pathname>");
		return 0;
	}
	if(fopen(argv[1],"w")==NULL)
		err_ret("%s file open error!",argv[1]);
	exit(0);
}
