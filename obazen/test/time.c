#include "anet.h"
#include <time.h>

int main(int argc, char **argv)
{
	int listenfd,connfd;
	socklen_t len;
	char buff[1024];
	time_t ticks;
	struct sockaddr_storage cliaddr;
	
	listenfd = tcp_listen(NULL, "2223", NULL);
	for (;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd,(SA *)&cliaddr,&len);
		ticks = time(NULL);
		write(connfd,buff,strlen(buff));
		close(connfd);
	}
	return(0);
}
