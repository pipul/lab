#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#define MAXLINE 1024

int randomstr(char *src,size_t size)
{
	size_t i = 0;
	while (i < size)
		src[i++] = 100 + rand()%10;
}

int main(int argc, char **argv)
{
	int fd,i = 0;
	size_t num,range,size;
	char BUFF[MAXLINE];
	char *ptr;
	
	if (argc != 3) {
		printf("usage:./db-test _size _num\n");
		exit(0);
	}

	range = atoi(argv[1]);
	num = atoi(argv[2]);
	
	srand((unsigned)time(NULL));
	umask(0);
	while (i < num) {
		snprintf(BUFF,MAXLINE,"%d",i++);
		if ((fd = open(BUFF,O_RDWR|O_CREAT,0644)) < 0)
			continue;
		size = rand()%range;
		if ((ptr = malloc(size)) == NULL) {
			close(fd);
			continue;
		}
		randomstr(ptr,size);
		write(fd,ptr,size);
		close(fd);
		free(ptr);
	}
	
	return(0);
}
