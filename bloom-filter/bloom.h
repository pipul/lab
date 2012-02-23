#ifndef __BLOOM_H_
#define __BLOOM_H_

#include <stdio.h>

/*
 * The default bloom filter error rate.
 */
#define BLOOM_ERROR_P 0.01

typedef struct {
	size_t mbits;
	unsigned char *s;
	size_t len;
	int hashn;			/* hash function number */
	size_t (*hash)(const char *);
} BLOOM;

BLOOM *bloom_create(size_t n, float p);
void bloom_destroy(BLOOM *bloom);
int bloom_add(BLOOM *bloom, const char *s);
int bloom_check(BLOOM *bloom, const char *s);

#endif	
