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
