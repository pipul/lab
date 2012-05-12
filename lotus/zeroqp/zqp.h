/* zqp.h - A zeroqp event driven network programming framework.
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


#ifndef __ZQP_H_
#define __ZQP_H_

#include <stdint.h>
#include "protobuf.h"

#define INET_ADDRSTRLEN 46  /* for IPv4 dotted-decimal and IPv6 hex string */
#define INET_PORTSTRLEN 6
#define ZQPS_NAMESTRLEN 10  /* including the trailing null bytes to str */

typedef struct {
	int32_t len;
	int8_t data[];
} slice;

typedef void _srv_callback(int32_t fd, int32_t argc, slice **argv);

struct zeroqp_service {
    int8_t inet_addr[INET_ADDRSTRLEN];
	int32_t inet_port;
	int8_t zqp_srv[ZQPS_NAMESTRLEN];
	_srv_callback *zqp_cbfunc;
} ;

/* zeoroqp_open() creates an zeroqp instance,returns a "file descriptor", but
 * this fd can't use by write()/read().. Warnning!
 * zeroqp_close() closes a zeroqp file descriptor, so that it no longer refers
 * to any zeroqp handle and may be reused, the resources associated with the
 * "file descriptor" are freed.
 */
int32_t zeroqp_open();
void zeroqp_close(int32_t zfd);

/* The zeroqp_ctl performs control operations on the zeroqp instance referred
 * to by the "file descriptor" zfd, It request that the operation op be
 * performed for the target "file descriptor", zfd.
 *
 * Valid values for the op argument are:
     ZEROQP_CTL_ADD
	     Install one service on the zeroqp instance by the zs.
	 ZEROQP_CTL_DEL
	     Remove that service from zeroqp instance by zs.
 */
int32_t zeroqp_ctl(int32_t zfd, int32_t op, struct zeroqp_service *zs);
#define ZEROQP_CTL_ADD 0x01
#define ZEROQP_CTL_DEL 0x02


/* The call make the zeroqp instance into working state. zeroqp will open
 * all registered service and listen on one sockfd, waiting for clients to
 * connect and call the service response callback function.
 */
void zeroqp_wait(int32_t zfd);







#endif
