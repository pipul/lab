#ifndef __ANET_H__
#define __ANET_H__

#include <sys/socket.h>

int tcp_listen(const char *host, const char *serv, socklen_t *addrlen);

#endif
