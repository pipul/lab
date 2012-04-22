#ifndef __AE_H_
#define __AE_H_

#define AE_SETSIZE (1024*10)	/* Max number of fd supported */
#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#include <sys/epoll.h>

struct eloop;

/* Call back function */
typedef void cbfunc(struct eloop *el, int32_t fd, void *argv, int32_t mask);


typedef struct
 asynchronous_event {
	int32_t mask;
	cbfunc 	*rcb;	/* read operation call back func */
	cbfunc 	*wcb;	/* write operation call back func */
	void 	*data;
} ae_t;

typedef struct eloop {
	int32_t efd;
	int32_t state;
#define AE_RUN  1
#define AE_STOP	2
	ae_t	events[AE_SETSIZE];
	struct epoll_event ready[AE_SETSIZE];
} EL;

EL *el_open(void);
int32_t el_close(EL *el);
int32_t el_stop(EL *el);

#define AE_NO_BLOCK 1
int32_t el_start(EL *el, int32_t flags);

int32_t el_addEvent(EL *el, int32_t fd, int32_t mask, cbfunc *cb, void *data);
int32_t el_delEvent(EL *el, int32_t fd, int32_t mask);




















#endif
