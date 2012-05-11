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

int32_t zeroqp_open();
void zeroqp_close(int32_t zfd);

int32_t zeroqp_ctl(int32_t zfd, int op, struct zeroqp_service *zs);
#define ZEROQP_CTL_ADD 0x01
#define ZEROQP_CTL_DEL 0x02

void zeroqp_wait(int32_t zfd);







#endif
