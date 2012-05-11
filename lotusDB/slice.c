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
