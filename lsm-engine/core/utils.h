/* utils.h - A common functions set implementation
 *
 * Copyright (c) 2011-2012, FangDong <yp.fangdong@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */



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
#define PATH_MAX 1024
#define KEY_MAX 1023


typedef int8_t * sds;

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


int32_t sendfile(int fd_in, int32_t off_in, int fd_out, int32_t off_out, size_t len);



#endif
