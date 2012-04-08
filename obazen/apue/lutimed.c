#include "apue.h"
#include <fcntl.n>
#include <utime.h>

int main(int argc,char *argv[])
{
	int i,fd;
	struct stat statbuf[2];

	if(argc!=2)
		err_ret("usage: ./lutime <pathname>");
	if(stat(argv[1],&statbuf[0])<0)
		err_ret("%s:stat error.",argv[1]);
	if(utime(argv[1])<0)
		err_ret("utime error.");
	if(stat(argv[1],&statbuf[1])<0)
		err_ret("%s:stat error.",argv[1]);
	if(statbuf[0].st_atime=statbuf[1].st_atime)
		printf("The atime has not change");
	else
		printf("The atime have changed");
	if(statbuf[0].st_mtime=statbuf[1].st_mtime)
		printf("The mtime has not change");
	else
		printf("The mtime have changed");
	if(statbuf[0].st_ctime=statbuf[1].st_ctime)
		printf("The ctime has not change");
	else
		printf("The ctime have changed");
	exit(0);
}

