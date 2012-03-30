#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdint.h>
#include <fcntl.h>

uint32_t crc32_encode(const int8_t  *buf, int32_t len);
uint32_t md532_encode(const int8_t  *buf, int32_t len);


FILE *stdlog;
FILE *stdevt;

#define EVT_FILE "HPDB.evt"	/* event file log */
#define LOG_FILE "HPDB.log"	/* recover log data */
#define BLOCK_SIZE 40960	/* default blocksize = 1mb */


#define KEY_MAX 1023
typedef int8_t  * sds;

typedef struct {
	int32_t len;
	int8_t  data[];
} sdshdr;

sds sdsnew(int8_t  *src);
sds sdsnnew(int8_t *src, int32_t len);
void sdsdel(sds s);
sds sdsdup(sds s);
int32_t sdslen(sds s);
int32_t sdscmp(sds s, sds h);
sds sdscat(sds s, int8_t *h);
sds sdsncat(sds s, int8_t *h, int32_t len);
sds sdsset(sds s, int32_t c);
sds sdsnset(sds s, int32_t c, int32_t len);
sds sdsnul();
sds sdsful();





#define __ERROR_LOG(o) do {\
	fprintf(stdlog, "[%d] %s(line:%d) \n", \
		(int)getpid(), __FUNCTION__, __LINE__);\
	goto o;\
} while(0)

#define __EVENT_LOG(x...) do {\
	fprintf(stdevt, ##x);\
} while(0)



int32_t natoi(const int8_t *src, int32_t len);



#endif
