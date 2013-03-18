#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "ae.h"

EL *el_open() {
	EL *el;
	int i;

	if (!(el = malloc(sizeof(EL))))
		return(NULL);
	/* 1024 is just an hint for the kernel */
	el->efd = epoll_create(1024);
	el->stop = 0;

	/*
	 * Events with mask == AE_NONE are not set.
	 * So let's initialize the vector with it.
	 */
	for (i = 0;i < AE_SETSIZE;i++)
		el->ev[i].mask = AE_NONE;

	return(el);
}

int el_close(EL *el)
{
	close(el->efd);
	free(el);

	return(AE_OK);
}

int el_stop(EL *el) {
	el->stop = 1;
	return(AE_OK);
}

int el_start(EL *el, int flags) {
	int readyEventsNum;
	el->stop = 0;
	while (!el->stop) {
		struct timeval tv, *tvp;

		/*
		 * Calculate the time missing for the epoll_wait().
		 * if we have to check for events but need to return
		 * because of AE_NO_BLOCK. so we can set the timeout
		 * to zero.
		 */
		if (flags & AE_NO_BLOCK) {
			tv.tv_sec = tv.tv_usec = 0;
			tvp = &tv;
		} else {
			tvp = NULL;
		}

		readyEventsNum = epoll_wait(
			el->efd, el->rd, AE_SETSIZE, tvp ? (
				tvp->tv_sec*1000 + tvp->tv_usec/1000): -1);
		readyEventsNum = readyEventsNum > 0 ? readyEventsNum : 0;
		
		int j;
		for (j = 0;j < readyEventsNum;j++) {
			struct epoll_event *e = el->rd + j;
			int mask;
			efile_t *fe = &el->ev[e->data.fd];

			if (e->events & EPOLLIN)
				mask |= AE_READABLE;
			if (e->events & EPOLLOUT)
				mask |= AE_WRITABLE;

			/*
			 * Note the fe->mask & mask &... code:
			 * Maybe an already processed event removed an
			 * element that fired and we still didn't precessed
			 * , so we check if the event is still valid.
			 */
			int rf = 0;
			if (fe->mask & mask & AE_READABLE) {
				rf = 1;
				fe->rProc(e->data.fd,fe->data,mask);
			}
			if (fe->mask & mask & AE_WRITABLE) {
				if(!rf || fe->wProc != fe->rProc)
					fe->wProc(e->data.fd,fe->data,mask);
			}
		}
	}

	return(AE_OK);
}


int el_addFileEvent(el, fd, mask, proc, data)

EL *el;
int fd;
int mask;
eProc *proc;
void *data;

{
	if (fd >= AE_SETSIZE)
		return(AE_ERR);
	efile_t *fe = &el->ev[fd];

	/*
	 * If the fd was already monitored for some event, we need a
	 * Mod operation. Otherwise we need a Add operation.
	 */
	int op = (fe->mask == AE_NONE) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

	/*
	 * Merge the old events. 
	 */
	fe->mask |= mask;

	struct epoll_event ee;
	ee.events = 0;

	if (fe->mask != AE_NONE) {
		if (fe->mask & AE_READABLE)
			ee.events |= EPOLLIN;
		if (fe->mask & AE_WRITABLE)
			ee.events |= EPOLLOUT;
	}

	/*
	 * Avoid valgrind warning.
	 */
	ee.data.u64 = 0;
	ee.data.fd = fd;

	if (epoll_ctl(el->efd,op,fd,&ee) == -1)
		return(AE_ERR);

	/*
	 * Add the process function to the EL.
	 */
	fe->rProc = (mask & AE_READABLE) ? proc : NULL;
	fe->wProc = (mask & AE_READABLE) ? proc : NULL;
	
	fe->data = data;

	return(AE_OK);
}

int el_delFileEvent(EL *el, int fd, int mask)
{
	if (fd >= AE_SETSIZE)
		return(AE_ERR);
	efile_t *fe = &el->ev[fd];

	if (!(fe->mask & mask))
		return(AE_ERR);
	/*
	 * Delete the event from el.
	 */
	fe->mask = fe->mask & (~mask);

	/*
	 * Note that if the fd was still monitor other event, we need a
	 * Mod operation. Otherwise we need a DEL operation.
	 */
	int op = (fe->mask == AE_NONE) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

	struct epoll_event ee;
	ee.events = 0;

	if (fe->mask != AE_NONE) {
		if (fe->mask & AE_READABLE)
			ee.events |= EPOLLIN;
		if (fe->mask & AE_WRITABLE)
			ee.events |= EPOLLOUT;
	}

	/*
	 * Avoid valgrind warning
	 */
	ee.data.u64 = 0;
	ee.data.fd = fd;

	if (epoll_ctl(el->efd,op,fd,&ee) == -1)
		return(AE_ERR);

	fe->rProc = (mask & AE_READABLE) ? NULL : fe->rProc;
	fe->wProc = (mask & AE_WRITABLE) ? NULL : fe->wProc;
	fe->data = (fe->mask == AE_NONE) ? NULL : fe->data;

	return(AE_OK);
}
