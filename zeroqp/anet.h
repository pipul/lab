#ifndef __NET_H_
#define __NET_H_

#include <sys/socket.h>

int32_t tcp_connect(const int8_t *host, const int8_t *serv);
int32_t tcp_listen(const int8_t *host, const int8_t *serv, socklen_t *addrlen);
int32_t tcp_accept(int32_t listenfd, int8_t *ip, int32_t *port);


#endif
