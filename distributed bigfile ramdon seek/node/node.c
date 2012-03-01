#include "net.h"
#include "ae.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

#define DB_PATH "db/"
#define MAXLINE 1024
typedef struct sockaddr SA;

void sendRequesData(int listenfd, void *argv, int mask)
{
	int clientfd, fd, n;
	char RQFILE[MAXLINE] = "";
	char BUFF[MAXLINE] = "";
	char recvline[MAXLINE] = "";
	struct stat rqstat;

	char *ptr,*rqfptr;
	int offset;
	int size,len;
	
	socklen_t cliaddr_len;
	struct sockaddr_storage cliaddr;
	
	cliaddr_len = sizeof(cliaddr);
	clientfd = accept(listenfd,(SA *)&cliaddr,&cliaddr_len);
	
	strcpy(RQFILE,DB_PATH);
	rqfptr = RQFILE + strlen(RQFILE);

	if ((n = read(clientfd,recvline,MAXLINE)) > 0) {
		ptr = strstr(recvline,"GET ");
		strncpy(rqfptr,ptr+4,strstr(ptr,"\r\n")-ptr-4);

		ptr = strstr(recvline,"OFFSET ");
		strncpy(BUFF,ptr+7,strstr(ptr,"\r\n")-ptr-7);
		offset = atoi(BUFF);
		
		ptr = strstr(recvline,"SIZE ");
		strncpy(BUFF,ptr+5,strstr(ptr,"\r\n")-ptr-5);
		size = atoi(BUFF);
		
		stat(RQFILE,&rqstat);
		
		if (rqstat.st_size < offset) {
			close(clientfd);
			return;
		}
		
		len = rqstat.st_size - 
			offset > size ? size : rqstat.st_size - offset;

		if ((fd = open(RQFILE,O_RDONLY)) < 0) {
			close(clientfd);
			return;
		}

		lseek(fd,offset,SEEK_SET);
		
		if ((n = read(fd,BUFF,len)) != len) {
			close(fd);
			close(clientfd);
			return;
		}
		
		write(clientfd,BUFF,len);
	}
	close(fd);
	close(clientfd);
}


int main(int argc, char **argv)
{
	int listenfd, connfd;
	EL *el;
	
	socklen_t addrlen;
	char BUFF[MAXLINE];
	struct sockaddr_storage cliaddr;
	
	if (argc == 2)
		listenfd = tcpListen(NULL,argv[1],&addrlen);
	else if (argc == 3)
		listenfd = tcpListen(argv[1],argv[2],&addrlen);
	else {
		printf("usage:node [HOST] <PORT>\n");
		exit(0);
	}
	
	if ((el = el_open()) == NULL) {
		printf("node error for creating epoll.\n");
		exit(0);
	}

	el_addFileEvent(el,listenfd,AE_READABLE,sendRequesData,NULL);
	
	el_start(el,AE_BLOCK);

	el_close(el);
}	
	
