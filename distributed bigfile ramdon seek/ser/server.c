#include "debug.h"
#include "conhash.h"
#include "ae.h"
#include "net.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

#define REPLICAS 20
#define MAXLINE 1024
typedef struct sockaddr SA;

static cluster *cl;


void sendServerIp(int listenfd,void *argv,int mask)
{
	int clientfd, n;
	char BUFF[MAXLINE];
	char RQFILE[MAXLINE];
	char *ptr;
	const char *serIp;

	socklen_t cliaddr_len;
	struct sockaddr_storage cliaddr;
	
	cliaddr_len = sizeof(cliaddr);
	clientfd = accept(listenfd,(SA *)&cliaddr,&cliaddr_len);
	
	if ((n = read(clientfd,BUFF,MAXLINE)) > 0) {
		ptr = strstr(BUFF,"GET ");
		strncpy(RQFILE,ptr+4,strstr(ptr,"\r\n")-ptr-4);
		
		serIp = (char *)clusterGetServer(cl,RQFILE);
		
		if (serIp)
			write(clientfd,serIp,strlen(serIp));
		else
			write(clientfd,"no",2);
	}
	
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
	else
		err_quit("usage:server [HOST] <PORT>\n");
	
	if ((el = el_open()) == NULL)
		err_quit("server error for creating epoll.\n");
	if ((cl = clusterCreate()) == NULL)
		err_quit("server error for creating cluster.\n");
	
	clusterAddServer(cl,"127.0.0.1",REPLICAS);
//	clusterAddServer(cl,"8.8.4.4",REPLICAS);
	
	el_addFileEvent(el,listenfd,AE_READABLE,sendServerIp,NULL);
	
	el_start(el,AE_BLOCK);
	
	el_close(el);
}
