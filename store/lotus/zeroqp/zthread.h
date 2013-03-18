/* zthread.h - A zeroqp working thread implementation
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


#ifndef __ZTHREAD_H_
#define __ZTHREAD_H_

#include "zqp.h"

typedef struct _zqp_service {
	int8_t sign[ZQPS_NAMESTRLEN];
	_srv_callback *cbfunc;
	struct _zqp_service *si_next;
} zqp_service_t;

typedef struct _zqp_worker {
	int32_t listenfd;
	int8_t inet_port[INET_PORTSTRLEN];
	int8_t inet_addr[INET_ADDRSTRLEN];
	int32_t serv_size; /* the number of service */
	zqp_service_t *serv_list;
	struct _zqp_worker *wo_next;
} zqp_worker_t;


typedef struct {
	zqp_worker_t *workers;   /* a worker list */
} zqp_thread_t;


zqp_thread_t *zeroqp_thread_create();
void zeroqp_thread_free(zqp_thread_t *zt);

int32_t zeroqp_thread_ctl(
						  zqp_thread_t *zt, 
						  int32_t op, 
						  int8_t *inet_addr, 
						  int32_t inet_port, 
						  int8_t *sign, 
						  _srv_callback *cb );

#define ZTHREAD_CTL_ADD 0x01
#define ZTHREAD_CTL_DEL 0x02














#endif
