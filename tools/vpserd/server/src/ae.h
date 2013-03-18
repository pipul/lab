#ifndef __AE_H__
#define __AE_H__

/* Max number of fd supported */
#define AE_SETSIZE (1024*10)

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_NO_BLOCK 4

#define AE_NOMORE -1

/* internal data struction. */
#include <sys/epoll.h>

typedef struct epoll {
	int efd;
	struct epoll_event events[AE_SETSIZE];
} epoll_t;

/* Ending internal. */


struct eloop;

/* File event structure */
typedef struct efile {
	/* one of AE_(READABLE|WRITABLE) */
	int mask;
	
	void *(*rProc)(struct eloop *el, int fd, void *data, int op);
	void *(*wProc)(struct eloop *el, int fd, void *data, int op);
	void *data;
} efile_t;

/* A fired event */
typedef struct efire {
	int fd;
	int mask;
} efire_t;

/* State of an event based program */
typedef struct eloop {
	int maxfd;
	int stop;
	
	efile_t ev[AE_SETSIZE];
	efire_t rd[AE_SETSIZE];
	
	epoll_t *ep;
	
} EL;

/* Prototypes */
EL *el_open(void);
int el_close(EL *el);

int el_stop(EL *el);
int el_start(EL *el, int flags);

int el_addFileEvent(EL *el, int fd, int mask, void *proc, void *data);
int el_delFileEvent(EL *el, int fd, int mask);

#endif
