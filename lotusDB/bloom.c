#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "lotus.h"
#include "md5.h"


/* The Bloom-filter imprementation  ------->
 * Bloom-filter is a space-efficient probabilistic data structure
 * that is used to test whether an element is a member of a set.
 */


#define SETBIT(s,n) (s[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(s,n) (s[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))

#define _ALIGN 8
#define _S_round_up(__bytes)\
     (((__bytes) + (uint32_t) _ALIGN-1) & ~((uint32_t) _ALIGN - 1))

sbloom_t *sbloom_new(uint32_t n, float p)
{
    sbloom_t *sb;

    if (!(sb = calloc(1,sizeof(sbloom_t))))
        return(NULL);
    sb->mbits = ceil(n*log(p)/(-log(2)*log(2)));
    sb->size = _S_round_up(sb->mbits)/CHAR_BIT;
    if (!(sb->s = calloc(sb->size,sizeof(char)))) {
        free(sb);
        return(NULL);
    }
    sb->hashn = floor(0.7*(sb->mbits)/n);
    sb->hash = md5_encode;

    return(sb);
}

void sbloom_free(sbloom_t *sb)
{
    if (sb) {
        free(sb->s);
        free(sb);
    }
}

int sbloom_set(sbloom_t *sb, const slice_t key)
{
    int n,slen;
    char buffer[_KEY_MAX + 1] = {0};

    if (!sb || !key)
        return(-1);
    slen = slice_len(key);
    memcpy(buffer,key,slen);
    for (n = 0; n < sb->hashn; n++) {
        buffer[slen] = n + 1;
        SETBIT(sb->s,(*sb->hash)(buffer,slen)%sb->mbits);
    }
    return(0);
}

int sbloom_get(sbloom_t *sb, const slice_t key)
{
    int n,slen;
    char buffer[_KEY_MAX + 1] = {0};

    if (!sb || !key)
        return(-1);
    slen = slice_len(key);
    memcpy(buffer,key,slen);
    for (n = 0; n < sb->hashn; n++) {
        buffer[slen] = n + 1;
        if (!(GETBIT(sb->s,(*sb->hash)(buffer,slen)%sb->mbits)))
            return(-1);
    }
    return(0);
}
