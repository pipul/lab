/* anet.c - A tcp network framework implementation
 *
 * Copyright (c) 2011-2012, FangDong <yp.fangdong@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */



#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "anet.h"

#define __DEBUG(x...)
#define LISTENQ 1024


int32_t tcp_connect(const int8_t *host, const int8_t *serv)
{
    int32_t sockfd, n;
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

int32_t tcp_listen(const int8_t *host, const int8_t *serv, socklen_t *addrlen)
{
    int32_t listenfd, n;
    const int32_t on = 1;
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

int32_t tcp_accept(int32_t listenfd, int8_t *ip, int32_t *port)
{
    int32_t cli_fd;
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);

    if (listenfd < 0)
        return(-1);    
    while(1) {
        cli_fd = accept(listenfd,(struct sockaddr*)&sa,&salen);
        if (cli_fd == -1) {
            if (errno == EINTR)
                continue;
            else
                __DEBUG("accept: %s", strerror(errno));
                break;
        }
        break;
    }
    if (cli_fd) {
        if (ip) strcpy(ip,inet_ntoa(sa.sin_addr));
        if (port) *port = ntohs(sa.sin_port);
    }
    return(cli_fd);
}
