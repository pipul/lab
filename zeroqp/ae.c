#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ae.h"

EL *el_open()
{
    EL *el;
    int32_t i;

    if ((el = malloc(sizeof(EL))) == NULL)
        return(NULL);
    el->efd = epoll_create(1024);
    el->state = AE_STOP;
    for (i = 0; i < AE_SETSIZE; i++)
        el->events[i].mask = AE_NONE;
    return(el);
}

int32_t el_close(EL *el)
{
    close(el->efd);
    free(el);
    return(AE_OK);
}

int32_t el_stop(EL *el) {
    el->state = AE_STOP;
    return(AE_OK);
}

int32_t el_start(EL *el, int32_t flag)
{
    ae_t *e;
    int32_t n,i;
    int32_t mask,rf;
    struct timeval tv, *tvp;
    struct epoll_event *ee;

    el->state = AE_RUN;    
    while (el->state != AE_STOP) {
        if (flag & AE_NO_BLOCK) {
            tv.tv_sec = tv.tv_usec = 0;
            tvp = &tv;
        } else {
            tvp = NULL;
        }
        n = epoll_wait(el->efd, el->ready, AE_SETSIZE,
         tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);
        n = n > 0 ? n : 0;
        
        for (i = 0; i < n; i++) {
            ee = el->ready + i;
            e = &el->events[ee->data.fd];
            if (ee->events & EPOLLIN)
                mask = mask | AE_READABLE;
            if (ee->events & EPOLLOUT)
                mask = mask | AE_WRITABLE;
            if (e->mask & mask & AE_READABLE) {
                rf = 1;
                e->rcb(el,ee->data.fd,e->data,mask);
            }
            if (e->mask & mask & AE_WRITABLE)
                if (rf != 1 || e->rcb != e->wcb)
                    e->wcb(el,ee->data.fd,e->data,mask);
        }
    }
    return(AE_OK);
}

int32_t el_addEvent(
    EL *el, int32_t fd, int32_t mask, cbfunc *cb, void *data)
{
    ae_t *e;
    int32_t op;
    struct epoll_event ee;

    if (fd >= AE_SETSIZE)
        return(AE_ERR);
    e = &el->events[fd];
    op = (e->mask == AE_NONE) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    e->mask = e->mask | mask;
    ee.events = 0;
    
    if (e->mask != AE_NONE) {
        if (e->mask & AE_READABLE)
            ee.events |= EPOLLIN;
        if (e->mask & AE_WRITABLE)
            ee.events |= EPOLLOUT;
    }
    
    ee.data.u64 = 0;
    ee.data.fd = fd;
    
    if (-1 == epoll_ctl(el->efd,op,fd,&ee))
        return(AE_ERR);
    e->rcb = (mask & AE_READABLE) ? cb : NULL;
    e->wcb = (mask & AE_WRITABLE) ? cb : NULL;
    e->data = data;
    
    return(AE_OK);
}


int32_t el_delEvent(EL *el, int32_t fd, int32_t mask)
{
    ae_t *e;
    int32_t op;
    struct epoll_event ee;

    if (fd >= AE_SETSIZE)
        return(AE_ERR);
    e = &el->events[fd];
    if (!(e->mask & mask))
        return(AE_ERR);
    e->mask = e->mask & (~mask);
    op = (e->mask == AE_NONE) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
    ee.events = 0;
    
    if (e->mask != AE_NONE) {
        if (e->mask & AE_READABLE)
            ee.events |= EPOLLIN;
        if (e->mask & AE_WRITABLE);
            ee.events |= EPOLLOUT;
    }
    
    ee.data.u64 = 0;
    ee.data.fd = fd;
    
    if (-1 == epoll_ctl(el->efd,op,fd,&ee))
        return(AE_ERR);
    e->rcb = (mask & AE_READABLE) ? NULL : e->rcb;
    e->wcb = (mask & AE_WRITABLE) ? NULL : e->wcb;
    e->data = (e->mask == AE_NONE) ? NULL : e->data;
    
    return(AE_OK);
}









