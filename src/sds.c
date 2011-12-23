#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sds.h"

char * sdsNew(const char *src, size_t len)
{
	sds *sN;

	if (len <= 0)
		len = (src == NULL) ? 0 : strlen(src);

	if ((sN = malloc(sizeof(sds)+len+1)) == NULL)
		return(NULL);
	sN->len = len;
	sN->free = 0;
	if (len) {
		if (src)
			memcpy(sN->buf, src, len);
		else
			memset(sN->buf, 0, len);
	}
	sN->buf[len] = '\0';
	return (char *)sN->buf;
}

size_t sdsLen(const char *src)
{
	sds *str = (sds*)(src-(sizeof(sds)));
	return(str->len);
}

size_t sdsAvail(const char *src)
{
	sds *str = (sds*)(src-(sizeof(sds)));
	return(str->free);
}

void sdsDel(char *src)
{
	if (src == NULL)
		return;
	free(src-sizeof(sds));
}

char *sdsDup(const char *src)
{
	return sdsNew(src,sdsLen(src));
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
	char *key;
	char *value;
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

