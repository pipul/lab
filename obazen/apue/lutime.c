#include "apue.h"
#include <fcntl.h>
#include <utime.h>

int main(int argc,char *argv[])
{
	int i,fd;
	struct stat statbuf[2];
	struct utimbuf timebuf;

	if(argc!=3)
	{
		err_ret("usage: ./lutime <pathname> <pathname>\n");
		return 0;
	}
	if(stat(argv[1],&statbuf[0])<0)
	{
		err_ret("%s:stat error.\n",argv[1]);
		return 0;
	}
	if(open(argv[1],O_RDWR|O_TRUNC)<0)
	{
		err_ret("%s:file open error",argv[1]);
		return 0;
	}
	if(stat(argv[1],&statbuf[1])<0)
	{
		err_ret("%s:stat error.\n",argv[2]);
		return 0;
	}
	
	timebuf.actime=statbuf[1].st_atime;
	timebuf.modtime=statbuf[1].st_mtime;


	if(utime(argv[2],&timebuf)<0)
		err_ret("utime error.\n");
	
	if(stat(argv[2],&statbuf[1])<0)
		err_ret("Restat %s error.",argv[2]);

	if(statbuf[0].st_atime=statbuf[1].st_atime)
		printf("The atime has not change\n");
	else
		printf("The atime have changed\n");
	if(statbuf[0].st_mtime=statbuf[1].st_mtime)
		printf("The mtime has not change\n");
	else
		printf("The mtime have changed\n");
	if(statbuf[0].st_ctime=statbuf[1].st_ctime)
		printf("The ctime has not change\n");
	else
		printf("The ctime have changed\n");
	exit(0);
}
