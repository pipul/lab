#include <stdlib.h>
#include <pthread.h>

#define NHASH 29
#define HASH(fp) (((unsigned long)fp)%NHASH)
struct foo *fh[NHASH];

pthread_mutex_t hashlock=PTHREAD_MUTEX_INITIALIZER;

struct foo{
	int f_count;
	pthread_mutex_t f_lock;
	struct foo *f_next;
	int f_id;
};

struct foo * foo_alloc(void)
{
	struct foo *fp;
	int idx;

	if((fp=malloc(sizeof(struct foo)))!=NULL){
		fp->f_count=1;
		if(pthread_mutex_init(&fp->f_lock,NULL)!=0){
			free(fp);
			return(NULL);
		}
	
		idx=HASH(fp);
		pthread_mutex_lock(&hashlock);
		fp->f_next=fh[idx];
		fh[idx]=fp->f_next;
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_unlock(&fp->f_lock);
	}
	return(fp);
}

void foo_hold(struct foo *fp)
{
	pthread_mutex_lock(&fp

