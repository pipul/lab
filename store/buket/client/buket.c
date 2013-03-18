#include "net.h"
#include "buket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>

#define MAXLINE 1024


BFILE *b_open(const char *host, const char *port, const char *file)
{
	int sockfd,len,n;
	char requestHeader[MAXLINE],filesize[MAXLINE];
	char nodeHost[MAXLINE],nodePort[MAXLINE],recvline[MAXLINE];
	char *ptr;
	BFILE *fp;

	if ((sockfd = tcpConnect(host,port)) < 0)
		return(NULL);

	snprintf(requestHeader,MAXLINE,"GET %s\r\n",file);

	len = strlen(requestHeader);
	if ((n = write(sockfd,requestHeader,len)) != len) {
		close(sockfd);
		return(NULL);
	}
	
	if ((n = read(sockfd,recvline,MAXLINE)) > 0)
		recvline[n] = '\0';
	else {
		close(sockfd);
		return(NULL);
	}
	
	ptr = strstr(recvline,"HOST ");
	snprintf(nodeHost,strstr(ptr,"\r\n")-ptr-4,"%s",ptr+5);
	ptr = strstr(recvline,"PORT ");
	snprintf(nodePort,strstr(ptr,"\r\n")-ptr-4,"%s",ptr+5);
	
	close(sockfd);
	
	if ((sockfd = tcpConnect(nodeHost,nodePort)) < 0)
		return(NULL);
	
	snprintf(requestHeader,MAXLINE,"FILE %s\r\n");
	len = strlen(requestHeader);
	if ((n = write(sockfd,requestHeader,len)) != len) {
		close(sockfd);
		return(NULL);
	}
	
	if ((n = read(sockfd,recvline,MAXLINE)) > 0)
		recvline[n] = '\0';
	else {
		close(sockfd);
		return(NULL);
	}
	
	if ((ptr = strstr(recvline,"FILE OK\r\n")) == NULL) {
		close(sockfd);
		return(NULL);
	}
	
	ptr = strstr(recvline,"SIZE ");
	snprintf(filesize,strstr(ptr,"\r\n")-ptr-4,"%s",ptr+5);

	if ((fp = malloc(sizeof(BFILE)+strlen(file)+1)) == NULL) {
		close(sockfd);
		return(NULL);
	}
	
	fp->fd = sockfd;
	fp->offset = 0;
	fp->size = atoi(filesize);
	strcpy(fp->name,file);

	return(fp);
}


int b_read(BFILE *fp, void *buf, size_t size,...)
{
	int len,n;
	size_t offset;
	char requestHeader[MAXLINE];
	char *ptr = requestHeader;
	va_list ap;

	va_start(ap,size);
	offset = va_arg(ap,int);
	va_end(ap);

	if (!buf || size == 0)
		return(-1);

	snprintf(requestHeader,MAXLINE,
		"GET %s\r\nOFFSET %d\r\nSIZE %d\r\n",fp->name,offset,size);

	len = strlen(requestHeader);
	if ((n = write(fp->fd,requestHeader,len)) != len)
		return(-1);
	
	return(read(fp->fd,buf,size));
}

int b_close(BFILE *fp)
{
	if (!fp)
		return(-1);
	close(fp->fd);
	free(fp);
}
