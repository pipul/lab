#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ae.h"



/* internal interface prototypes. */

static epoll_t *epollCreate() {
	epoll_t *ep = malloc(sizeof(epoll_t));
	
	if (!ep)
		return(NULL);
	/* 1024 is just an hint for the kernel */
	ep->efd = epoll_create(1024); 
	if (ep->efd == -1) {
		free(ep);
		return(NULL);
	}

	return(ep);
}

static int epollFree(epoll_t *ep) {
	close(ep->efd);
	free(ep);
	
	return(0);
}

/* If mask == AE_NONE than op should be EPOLL_CTL_DEL.
 * if mask != AE_NONE than op should be EPOLL_CTL_MOD
 * or EPOLL_CTL_ADD.
 */

static int epollSetEvent(epoll_t *ep, int fd, int mask, int op) {
	struct epoll_event ee;
	ee.events = 0;

	if (mask != AE_NONE) {
		if (mask & AE_READABLE)
			ee.events |= EPOLLIN;
		if (mask & AE_WRITABLE)
			ee.events |= EPOLLOUT;
	}
	
	/* avoid valgrind warning */
	ee.data.u64 = 0; 
	ee.data.fd = fd;
	
	if (epoll_ctl(ep->efd,op,fd,&ee) == -1)
		return(-1);
		
	return(0);
}

static int epollWait(epoll_t *ep, struct timeval *tvp) {
	int numevents ;
	numevents = epoll_wait(ep->efd,ep->events,AE_SETSIZE,
			tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);
			
	if(numevents > 0)
		return(numevents);
	else
		return(0);
}

/* End the internal data struction and interface. */


EL *el_open() {
	EL *el;
	int i;
	
	el = malloc(sizeof(EL));
	
	if (!el)
		return(NULL);
		
	el->stop = 0;
	el->maxfd = -1;
	el->ep = epollCreate();
	
	if (el->ep == NULL) {
		free(el);
		return(NULL);
	}

    /* 
     * Events with mask == AE_NONE are not set. 
     * So let's initialize the vector with it.
     */
	for (i = 0;i < AE_SETSIZE;i++)
		el->ev[i].mask = AE_NONE;
	
	return(el);
}

int el_close(EL *el) {
	epollFree(el->ep);
	free(el);
	
	return(0);
}

int el_stop(EL *el) {
	el->stop = 1;
	return(0);
}


/*
 * If flags is 0, the function does nothing and returns.
 * if flags has AE_FILE_EVENTS set, file events are processed.
 * the events that's possible to process without to wait are processed.
 *
 * The function returns the number of events processed.
 */

int el_start(EL *el, int flags) {
	int processed = 0, numevents;
	epoll_t *ep = el->ep;
	
	el->stop = 0;
	while(!el->stop) {
		/* Nothing to do, return. */
		if (!(flags & AE_FILE_EVENTS))
			return(0);
			
		if(el->maxfd != -1) {
			int j;
			struct timeval tv, *tvp;
			
			/* Calculate the time missing for the epoll_wait().
			 * if we have to check for events but need to return
			 * because of AE_NO_BLOCK. so we cant set the
			 * timeout to zero.
			 */
			if (flags & AE_NO_BLOCK){
				tv.tv_sec = tv.tv_usec = 0;
				tvp = &tv;
			}else {
				/* Otherwise we can block. wait forever */
				tvp = NULL;
			}
			
			numevents = epollWait(el->ep,tvp);

			for (j=0;j<numevents;j++) {
				struct epoll_event *e = el->ep->events + j;
				
				if (e->events & EPOLLIN)
					el->rd[j].mask |= AE_READABLE;
				if (e->events & EPOLLOUT)
					el->rd[j].mask |= AE_WRITABLE;
					
				el->rd[j].fd = e->data.fd;
			}
			
			for (j=0;j<numevents;j++) {
				efile_t *fe = &el->ev[el->rd[j].fd];
				int mask = el->rd[j].mask;
				int fd = el->rd[j].fd;
				
				/* note the fe->mask & mask &... code: maybe an already
				 * processed event removed an element that fired and we
				 * still didn't processed, so we check if the event is
				 * still valid.
				 */
				int rf = 0;
				
				if (fe->mask & mask & AE_READABLE) {
					rf = 1;
					fe->rProc(el,fd,fe->data,mask);
				}
				
				if (fe->mask & mask & AE_WRITABLE) {
					if(!rf || fe->wProc != fe->rProc)
						fe->wProc(el,fd,fe->data,mask);
				}
				
				processed++;
			}
		}
	}
	
	return(processed);
}
	

int el_addFileEvent(el, fd, mask, proc, data)

EL *el;
int fd;
int mask;
void *proc;
void *data;

{
	if (fd >= AE_SETSIZE)
		return(AE_ERR);
	efile_t *fe = &el->ev[fd];
	
	/* If the fd was already monitored for some event, we need a
	 * MOD operation. Otherwise we need a ADD operation. 
	 */
	int op = el->ev[fd].mask == AE_NONE ?
		EPOLL_CTL_ADD : EPOLL_CTL_MOD;
	
	/* Merge the old events. */
	fe->mask |= mask;
	
	if (epollSetEvent(el->ep,fd,fe->mask,op) == -1)
		return(AE_ERR);

	if (mask & AE_READABLE)
		fe->rProc = proc;
	else
		fe->rProc = NULL;
		
	if (mask & AE_WRITABLE)
		fe->wProc = proc;
	else
		fe->wProc = NULL;
	
	fe->data = data;
	if (fd>el->maxfd)
		el->maxfd = fd;
	
	return(AE_OK);
}
	
int el_delFileEvent(EL *el, int fd, int mask) {
	if (fd >= AE_SETSIZE)
		return(AE_ERR);
	efile_t *fe = &el->ev[fd];
	
	if (!(fe->mask & mask))
		return(AE_ERR);
	fe->mask = fe->mask & (~mask);

	/* If the fd was still monitor other event, we need a
	 * MOD operation. Otherwise we need a DEL operation. 
	 */
	int op = fe->mask == AE_NONE ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
	
	if (fd == el->maxfd && fe->mask == AE_NONE) {
		/* Update the max fd */
		int j;
		
		for (j = el->maxfd -1; j >= 0; j--)
			if (el->ev[j].mask != AE_NONE) break;
		el->maxfd = j;
	}
	
	if (epollSetEvent(el->ep,fd,fe->mask,op) == -1)
		return(AE_ERR);
	if (mask & AE_READABLE)
		fe->rProc = NULL;
	if (mask & AE_WRITABLE)
		fe->wProc = NULL;
	
	return(AE_OK);
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
