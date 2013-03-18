#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zqp.h"
#include "zthread.h"
#include "protobuf.h"
#include "ae.h"

typedef struct {
	EL *zqp_el;
	zqp_thread_t *zthr;
} zqp_t;

int32_t zeroqp_open()
{
	zqp_t *zqp;

	zqp = calloc(1,sizeof(zqp_t));
	zqp->zqp_el = el_open();
	zqp->zthr = zeroqp_thread_create();
	return((int32_t)zqp);
}

void zeroqp_close(int32_t zfd)
{
	zqp_t *zqp = (zqp_t *)zfd;
	if (zqp) {
		el_close(zqp->zqp_el);
		zeroqp_thread_free(zqp->zthr);
	}
	free(zqp);
}

int32_t zeroqp_ctl(int32_t zfd, int32_t op, struct zeroqp_service *zs)
{
	zqp_t *zqp = (zqp_t *)zfd;

	if (!zqp || !zs)
		return(-1);
	if (op & ZEROQP_CTL_ADD)
		zeroqp_thread_ctl(zqp->zthr,ZTHREAD_CTL_ADD,zs->inet_addr,
						  zs->inet_port,zs->zqp_srv,zs->zqp_cbfunc);
	else if (op & ZEROQP_CTL_DEL)
		zeroqp_thread_ctl(zqp->zthr,ZTHREAD_CTL_DEL,zs->inet_addr,
						  zs->inet_port,zs->zqp_srv,zs->zqp_cbfunc);
	return(0);
}



typedef struct {
	zqp_service_t *serv_list;
	ptb_t *ptb_context;
} zqp_session_t;

static zqp_session_t *zeroqp_session_create()
{
	zqp_session_t *zqp_ss;

	zqp_ss = calloc(1,sizeof(zqp_session_t));
	if (zqp_ss)
		zqp_ss->ptb_context = protobuf_create();
	if (zqp_ss && zqp_ss->ptb_context)
		return(zqp_ss);
	else
		return(NULL);
}

static void
 zeroqp_session_free(zqp_session_t *zqp_ss)
{
	if (zqp_ss)
		protobuf_free(zqp_ss->ptb_context);
	free(zqp_ss);
}

static void zeroqp_handle_exec(EL *el, 
							   int32_t clifd, 
							   void *argv, 
							   int32_t mask )
{
	int32_t nread,state;
	int8_t ptb_buf[PROTOBUF_SIZE] = {0};
	zqp_service_t *zs;
	zqp_session_t *zqp_ss;

	if (!el || clifd < 0 || !argv || !(mask & AE_READABLE))
		return;
	zqp_ss = (zqp_session_t *)argv;
	zs = zqp_ss->serv_list;
	nread = read(clifd,ptb_buf,PROTOBUF_SIZE);
	if (nread <= 0) {
		zeroqp_session_free(zqp_ss);
		el_delEvent(el,clifd,AE_READABLE);
		close(clifd);
	} else {
		protobuf_append(zqp_ss->ptb_context,ptb_buf,nread);
		state = protobuf_parse(zqp_ss->ptb_context);
		if (state == PROTOBUF_STATE_OK) {
			while (zs) {
				if (strcmp(zs->sign,
				 zqp_ss->ptb_context->ptb_slice_v[0]->slice_data) == 0)
					break;
				zs = zs->si_next;
			}
			if (zs)
				zs->cbfunc(clifd,zqp_ss->ptb_context->ptb_slice_c - 1,
						   (slice **)&zqp_ss->ptb_context->ptb_slice_v[1]);
			protobuf_clean(zqp_ss->ptb_context);
		} else if (state == PROTOBUF_STATE_ERR) {
			zeroqp_session_free(zqp_ss);
			el_delEvent(el,clifd,AE_READABLE);
			close(clifd);
		}
	}
}

static void zeroqp_handle_accept(EL *el, 
								 int32_t listenfd, 
								 void *argv, 
								 int32_t mask )
{
	int32_t cli_fd,cli_port;
	int8_t cli_ip[INET_ADDRSTRLEN];
	zqp_session_t *zqp_ss;

	if (!el || listenfd < 0)
		return;
	if ((zqp_ss = zeroqp_session_create()) == NULL)
		return;
	if ((cli_fd = tcp_accept(listenfd,cli_ip,&cli_port)) < 0) {
		zeroqp_session_free(zqp_ss);
		return;
	}
	zqp_ss->serv_list = ((zqp_worker_t *)argv)->serv_list;
	fprintf(stderr,"client:%s connected from %d\n",cli_ip,cli_port);
	el_addEvent(el,cli_fd,AE_READABLE,zeroqp_handle_exec,zqp_ss);
}

void zeroqp_wait(int32_t zfd)
{
	zqp_t *zqp = (zqp_t *)zfd;
	zqp_worker_t *zw = zqp->zthr->workers;

	if (!zqp)
		return;
	while (zw) {
		el_addEvent(zqp->zqp_el,zw->listenfd,AE_READABLE,zeroqp_handle_accept,zw);
		zw = zw->wo_next;
	}
	el_start(zqp->zqp_el,AE_NO_BLOCK);
}
