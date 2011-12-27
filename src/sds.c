#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sds.h"

sds sdsNew(const char *src, size_t len)
{
	sdshdr *sh;

	if (len <= 0)
		len = (src == NULL) ? 0 : strlen(src);

	if ((sh = malloc(sizeof(sdshdr)+len+1)) == NULL)
		return(NULL);
	sh->len = len;
	sh->free = 0;
	if (len) {
		if (src)
			memcpy(sh->buf, src, len);
		else
			memset(sh->buf, 0, len);
	}
	sh->buf[len] = '\0';
	return (char *)sh->buf;
}

size_t sdsLen(const sds src)
{
	sdshdr *str = (sdshdr *)(src-(sizeof(sdshdr)));
	return(str->len);
}

size_t sdsAvail(const sds src)
{
	sdshdr *str = (sdshdr *)(src-(sizeof(sdshdr)));
	return(str->free);
}

void sdsDel(sds src)
{
	if (src == NULL)
		return;
	free(src-sizeof(sdshdr));
}

sds sdsDup(const sds src)
{
	return sdsNew(src,sdsLen(src));
}

sds sdsSet(sds s,int c, size_t n)
{
	size_t len = (n > sdsLen(s)) ? sdsLen(s) : n;
	return memset(s,c,len);
}

/*
 * Test Case 1 for sds
 *

int main(int argc, char **argv)
{
	if (argc <= 2) {
		printf("Usage:...\n");
		return(0);
	}

	char **s;
	int i;
	s = calloc(argc,sizeof(char *));

	for (i = 0;i < argc;i++) {
		s[i] = sdsNew(argv[i],strlen(argv[i]));
	}

	printf("String List is avail or not :\n");
	for (i = 0;i < argc;i++) {
		printf("%s : %d\n",s[i],sdsAvail(s[i]));
	}

	printf("String List's len :\n");
	for (i = 0;i < argc;i++) {
		printf("%s : %d\n",s[i],sdsLen(s[i]));
	}

	printf("Delete all the string list :\n");
	for (i = 0;i < argc;i++) {
		sdsDel(s[i]);
	}
	
	return(0);
}
 */

/*
 * Test Case 2 for sds
 *

typedef struct {
	sds key;
	sds value;
} te;

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage:...\n");
		return(0);
	}
	
	te t;
	t.key = sdsNew(argv[0],strlen(argv[0]));
	t.value = sdsNew(argv[1],strlen(argv[1]));
	
	printf("te:\n");
	printf("key:%s %d\n",t.key,sdsLen(t.key));
	printf("value:%s %d\n",t.value,sdsLen(t.value));
	
	sdsDel(t.key);
	sdsDel(t.value);
	
	printf("te:\n");
	printf("key:%s %d\n",t.key,sdsLen(t.key));
	printf("value:%s %d\n",t.value,sdsLen(t.value));
	
	return(0);
}

 */

