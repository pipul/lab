#include "net.h"
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define LISTENQ 1024


int tcpConnect(const char *host, const char *serv)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;
	
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if ((n = getaddrinfo(host,serv,&hints,&res)) != 0)
		return(-1);
	ressave = res;
	
	do {
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		
		if (sockfd < 0)
			continue;
		if (connect(sockfd,res->ai_addr,res->ai_addrlen) == 0)
			break;
		close(sockfd);
	} while ((res = res->ai_next) != NULL);
	
	if (res == NULL)
		return(-1);
		
	freeaddrinfo(ressave);
	return(sockfd);
}

int tcpListen(const char *host, const char *serv, socklen_t *addrlen)
{
	int listenfd, n;
	const int on = 1;
	struct addrinfo hints, *res, *ressave;
	
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if ((n = getaddrinfo(host,serv,&hints,&res)) != 0)
		return(-1);
	ressave = res;
	
	do {
		listenfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		
		if (listenfd < 0)
			continue;
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
		if (bind(listenfd,res->ai_addr,res->ai_addrlen) == 0)
			break;
		close(listenfd);
	} while ((res = res->ai_next) != NULL);
	
	if (res == NULL)
		return(-1);
	listen(listenfd,LISTENQ);
	if (addrlen)
		*addrlen = res->ai_addrlen;
	freeaddrinfo(ressave);
	
	return(listenfd);
}


int getExternalIp(char *ipaddr)
{
	int n;
	struct addrinfo hints, *res, *ressave;
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(NULL,"80",&hints,&res)) !=0)
		return(-1);

	ressave = res;

	do {
		sprintf(ipaddr,"%s\n",
		inet_ntoa(((struct sockaddr_in *)(res->ai_addr))->sin_addr));
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		return(-1);
	freeaddrinfo(ressave);
	return(0);
}
		
