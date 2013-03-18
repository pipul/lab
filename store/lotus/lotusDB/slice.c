/* slice.c - A slice data structure implementation
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
#include <string.h>
#include <stdlib.h>
#include "lotus.h"


slice_t slice_new(char *src, int32_t len)
{
    slicehdr_t *sh;

    if (!src)
        return(NULL);
    if (!(sh = calloc(1,sizeof(slicehdr_t) + len + 1)))
        return(NULL);
    memcpy(sh->data,src,len);
    sh->len = len;
    return(sh->data);
}

void slice_free(slice_t src)
{
    slicehdr_t *sh;

    if (src) {
        sh = _SLICE_HP(src);
        free(sh);
    }
}

slice_t slice_dup(slice_t src)
{
    return(slice_new(src,slice_len(src)));
}

int32_t slice_len(slice_t src)
{
    slicehdr_t *sh;

    if (!src)
        return(0);
    sh = _SLICE_HP(src);
    return(sh->len);
}

int slice_cmp(slice_t src, slice_t drc)
{
    int len,res;

    if (!src || !drc) {
        if (!src)
            return(1);
        else if (!drc)
            return(-1);
        else
            return(0);
    }
    len = slice_len(src) >= slice_len(drc) ? slice_len(drc) : slice_len(src);
    res = memcmp(src,drc,len);
    if (res != 0)
        return(res);
    if (slice_len(src) > slice_len(drc))
        return(1);
    else if (slice_len(src) < slice_len(drc))
        return(-1);
    else
        return(0);
}
