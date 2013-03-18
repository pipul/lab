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
