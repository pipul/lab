/* sloter.c - A sloter implementation
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
#include "lotus.h"

sloter_t *sloter_new()
{
    sloter_t *st;

    st = calloc(1,sizeof(sloter_t));
    if (st) {
        st->cap = _SLOTFILE_HINT;
        st->sstfiles = calloc(st->cap,sizeof(sst_t *));
        if (!st->sstfiles) {
            free(st);
            st = NULL;
        }
    }
    return(st);
}


void sloter_free(sloter_t *st)
{
    sst_t *sst;

    if (!st)
        return;
    while (st->size) {
        sstable_close(st->sstfiles[st->size - 1]);
        st->size--;
    }
    free(st->sstfiles);
    free(st);
}

int sloter_add(sloter_t *st, sst_t *sst)
{
    int i;
    sst_t *__tmpsstfile;

    if (!st || !sst)
        return(-1);
    if (st->size > st->cap)
        st->sstfiles = realloc(st->sstfiles,st->cap = 2*st->cap);
    st->sstfiles[st->size++] = sst;
    for (i = st->size - 1; i > 0; i--) {
        if (_SST_TIMESTAMP(st->sstfiles[i]) > _SST_TIMESTAMP(st->sstfiles[i-1])) {
            __tmpsstfile = st->sstfiles[i];
            st->sstfiles[i] = st->sstfiles[i-1];
            st->sstfiles[i-1] = __tmpsstfile;
        } else break;
    }
    return(0);
}

int sloter_del(sloter_t *st, sst_t *sst)
{
    int i;

    if (!st || !sst)
        return(-1);
    for (i = 0; i < st->size; i++)
        if (st->sstfiles[i] == sst)
            break;
    if (i < st->size) {
        st->sstfiles[i] = NULL;
        memmove(&st->sstfiles[i],
                &st->sstfiles[i+1],(st->size-i)*sizeof(sst_t *));
    }
    st->size--;
    return(0);
}

sst_t *sloter_get(sloter_t *st, const char *sst_name)
{
    int i;

    if (!st || !sst_name)
        return(NULL);
    for (i = 0; i < st->size; i++)
        if (strcmp(st->sstfiles[i]->sst_name,sst_name) == 0)
            return(st->sstfiles[i]);
    return(NULL);
}
