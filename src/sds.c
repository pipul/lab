#include <stdio.h>
#include "sds.h"

#define malloc cmalloc
#define free cfree


/**
 * if l = 0 , copy the whole buffer.
 * if l > 0 , copy the specific length of buffer.
 * if l < 0 , the wrong length.
 */

char * sdsNew(const char *s, size_t l)
{
	sds *str;

	if (l<=0)
		l = (s==NULL) ? 0 : strlen(s);

	if ((str = cmalloc(sizeof(sds)+l+1)) == NULL)
		return(NULL);
	str->len = l;
	str->free = 0;
	if (l) {
		if (s)
			memcpy(str->buf, s, l);
		else
			memset(str->buf, 0, l);
	}
	str->buf[l] = '\0';
	return (char *)str->buf;
}

size_t sdsLen(const char *s)
{
	sds *str = (sds*)(s-(sizeof(sds)));
	return(str->len);
}

size_t sdsAvail(const char *s)
{
	sds *str = (sds*)(s-(sizeof(sds)));
	return(str->free);
}

void sdsFree(char *s)
{
	if (s==NULL)
		return;
	cfree(s-sizeof(sds));
}

char *sdsDup(const char *s)
{
	return sdsNew(s,sdsLen(s));
}


