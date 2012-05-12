/* bloom.c - A bloomfilter implementation
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
