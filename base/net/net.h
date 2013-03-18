#ifndef __NET_H_
#define __NET_H_

#include <sys/socket.h>

int tcpConnect(const char *host, const char *serv);
int tcpListen(const char *host, const char *serv, socklen_t *addrlen);



#endif
