#include "apue.h"

int main(int argc,char *argv[])
{
	int status;

	if(argc!=2)
		err_quit("command-line argument required.\n");

	if((status=system(argv[1]))<0)
		err_sys("system() error.\n");
	pr_exit(status);

	exit(0);
}
