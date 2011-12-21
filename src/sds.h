#ifndef __SDS_H_
#define __SDS_H_

#include <sys/types.h>

typedef struct sds {
	int len;
	int free;
	char buf[];
};


/**
 * if l = 0 , copy the whole char buffer.
 * if l > 0 , cope the specific length of char buffer.
 * if l < 0 , the wrong length.
 */

char *sdsNew(const char *s, size_t l);
void sdsFree(char *s);
char *sdsDup(const char *s);


size_t sdsAvail(const char *s);
size_t sdsLen(const char *s);
