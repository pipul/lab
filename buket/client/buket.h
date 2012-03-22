#ifndef __BUKET_H_
#define __BUKET_H_

#include <stddef.h>

typedef struct FILE_t {
	int fd;
	size_t offset;
	size_t size;
	char name[];
} BFILE;

BFILE *b_open(const char *host, const char *port, const char *file);
BFILE *b_creat(const char *host, const char *port, const char *file);
int b_lseek(BFILE *fp, off_t offset, int whence);
int b_read(BFILE *fp, void *buf, size_t size,...);
int b_write(BFILE *fp, void *buf, size_t size);
int b_close(BFILE *fp);




#endif
