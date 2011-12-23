#ifndef __SDS_H_
#define __SDS_H_

#include <sys/types.h>

typedef struct {
	size_t len:31;
	size_t free:1;
	char buf[];
} sds;

char *sdsNew(const char *src, size_t len);
void sdsDel(char *s);
char *sdsDup(const char *s);
size_t sdsAvail(const char *s);
size_t sdsLen(const char *s);


#endif
