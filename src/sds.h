#ifndef __SDS_H_
#define __SDS_H_

#include <sys/types.h>

typedef char * sds;

typedef struct {
	size_t len:31;
	size_t free:1;
	char buf[];
} sdshdr;

sds sdsNew(const char *src, size_t len);
void sdsDel(sds s);
sds sdsDup(const sds s);
size_t sdsAvail(const sds s);
size_t sdsLen(const sds s);
sds sdsSet(sds s,int c, size_t n);



#endif
