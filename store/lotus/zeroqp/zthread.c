/* zthread.c - A zeroqp working thread implementation
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zthread.h"
#include "anet.h"

static zqp_service_t 
 *zeroqp_service_create(int8_t *name, _srv_callback *cb)
{
	zqp_service_t *zs;

	if (!name || !cb)
		return(NULL);
	zs = malloc(sizeof(zqp_service_t));
	if (zs) {
		memset(zs,0,sizeof(zqp_service_t));
		snprintf(zs->sign,ZQPS_NAMESTRLEN,"%s",name);
		zs->cbfunc = cb;
	}
	return(zs);
}

static void zeroqp_service_free(zqp_service_t *zs)
{
	free(zs);
}




static zqp_worker_t
 *zeroqp_worker_create(int8_t *addr, int32_t port)
{
	zqp_worker_t *zw;

	if (!addr || port < 0)
		return(NULL);
	if ((zw = malloc(sizeof(zqp_worker_t))) == NULL)
		return(NULL);
	memset(zw,0,sizeof(zqp_worker_t));
	snprintf(zw->inet_addr,INET_ADDRSTRLEN,"%s",addr);
	snprintf(zw->inet_port,INET_PORTSTRLEN,"%d",port);
	zw->listenfd = tcp_listen(zw->inet_addr,zw->inet_port,NULL);
	if (zw->listenfd < 0) {
		free(zw);
		return(NULL);
	}
	return(zw);
}

static void zeroqp_worker_free(zqp_worker_t *zw)
{
	zqp_service_t *zs;

	if (!zw)
		return;
	close(zw->listenfd);
	while (zw->serv_size) {
		zs = zw->serv_list->si_next;
		zeroqp_service_free(zw->serv_list);
		zw->serv_list = zs;
		zw->serv_size--;
	}
	free(zw);
}



#define ZWORKER_CTL_ADD 0x01
#define ZWORKER_CTL_DEL 0x02

static int32_t
 zeroqp_worker_ctl(
				   zqp_worker_t *zw, 
				   int32_t op, 
				   int8_t *sign, 
				   _srv_callback *cb )
{
	zqp_service_t *zqp_srv,*zqp_srv_prev;

	if (!zw || !sign || !cb)
		return(-1);
	zqp_srv_prev = NULL;
	zqp_srv = zw->serv_list;
	while (zqp_srv) {
		if (strcmp(zqp_srv->sign,sign) == 0)
			break;
		zqp_srv_prev = zqp_srv;
		zqp_srv = zqp_srv->si_next;
	}
	if (op & ZWORKER_CTL_ADD) {
		if (zqp_srv)
			return(-1);
		if ((zqp_srv = zeroqp_service_create(sign,cb)) == NULL)
			return(-1);
		if (zw->serv_list)
			zqp_srv->si_next = zw->serv_list;
		zw->serv_list = zqp_srv;
		zw->serv_size++;
	} else if (op & ZWORKER_CTL_DEL) {
		if (!zqp_srv)
			return(-1);
		if (!zqp_srv_prev)
			zw->serv_list = zqp_srv->si_next;
		else
			zqp_srv_prev->si_next = zqp_srv->si_next;
		zeroqp_service_free(zqp_srv);
		zw->serv_size--;
	}
	return(0);
}


zqp_thread_t *zeroqp_thread_create()
{
	zqp_thread_t *zt;

	zt = malloc(sizeof(zqp_thread_t));
	if (zt)
		memset(zt,0,sizeof(zqp_thread_t));
	return(zt);
}

void zeroqp_thread_free(zqp_thread_t *zt)
{
	zqp_worker_t *zw;

	if (!zt)
		return;
	while (zt->workers) {
		zw = zt->workers->wo_next;
		zeroqp_worker_free(zt->workers);
		zt->workers = zw;
	}
	free(zt);
}

int32_t zeroqp_thread_ctl(
						  zqp_thread_t *zt, 
						  int32_t op, 
						  int8_t *inet_addr, 
						  int32_t inet_port, 
						  int8_t *sign, 
						  _srv_callback *cb )
{
	zqp_worker_t *zw,*zw_prev;
	zqp_service_t *zs;

	if (!zt || !inet_addr || inet_port < 0 || !sign || !cb)
		return(-1);

	zw_prev = NULL;
	zw = zt->workers;

	while (zw) {
		if (inet_port == atoi(zw->inet_port) &&
			strcmp(zw->inet_addr,inet_addr) == 0)
			break;
		zw_prev = zw;
		zw = zw->wo_next;
	}
	if (op & ZTHREAD_CTL_ADD) {
		if (!zw) {
			if ((zw = zeroqp_worker_create(inet_addr,inet_port)) == NULL)
				return(-1);
			zw->wo_next = zt->workers;
			zt->workers = zw;
		}
		return(zeroqp_worker_ctl(zw,ZTHREAD_CTL_ADD,sign,cb));
	} else if (op & ZTHREAD_CTL_DEL) {
		if (!zw)
			/* not found the worker, so this service must not exist */
			return(-1);
		if (-1 == zeroqp_worker_ctl(zw,ZTHREAD_CTL_DEL,sign,cb))
			return(-1);
		if (zw->serv_size == 0) {
			if (!zw_prev)
				zt->workers = zw->wo_next;
			else
				zw_prev->wo_next = zw->wo_next;
			zeroqp_worker_free(zw);
		}
		return(0);
	}
}
