#include "anet.h"
#include <stdio.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>

#define LISTENQ 1024

typedef struct sockaddr SA;


int
tcp_listen(const char *host, const char *serv, socklen_t *addrlen)
{
	int listenfd, n;
	const int on = 1;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("tcp_listen error for %s, %s: %s",
				host, serv,gai_strerror(n));
	ressave = res;

	do {
		listenfd = socket(res->ai_family, 
				res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue;	/* error, try the next one. */
		Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;		/* success */

		/* bind error. close and try the next one. */
		Close(listenfd);
	} while ( (res = res->ai_next) != NULL);

	/* error from final socket() or bind(). */
	if (res == NULL)
		err_sys("tcp_listen error for %s, %s", host, serv);

	Listen(listenfd, LISTENQ);

	if (addrlen)
		/* return size of protocol address. */
		*addrlen = res->ai_addrlen;
	freeaddrinfo(ressave);
	return(listenfd);
}
