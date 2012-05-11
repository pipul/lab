#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "lotus.h"

aot_t *aotable_open(const char *filename, int flags, ...)
{
    aot_t *at;
    va_list ap;
    int mode;

    if (!filename)
        return(NULL);
    va_start(ap,flags);
    at = calloc(1,sizeof(aot_t));
    if (at) {
        if (flags & AOT_OPEN) {
            at->fd = open(filename,O_RDWR|O_APPEND);
            if (at->fd > 0) {
                snprintf(at->aot_name,_PATH_MAX,"%s",filename);
                if (flags & AOT_SYNC)
                    at->flags = at->flags | AOT_SYNC;
            }
        }
        if (at->fd <= 0 && flags & AOT_CREAT) {
            mode = va_arg(ap,int);
            at->fd = open(filename,O_RDWR|O_APPEND|O_CREAT|O_EXCL,mode);
            if (at->fd > 0) {
                snprintf(at->aot_name,_PATH_MAX,"%s",filename);
                if (flags & AOT_SYNC)
                    at->flags = at->flags | AOT_SYNC;
            }
        }
    }
    va_end(ap);
    if (at && at->fd < 0) {
        free(at);
        at == NULL;
    }
    return(at);
}

void aotable_close(aot_t *at)
{
    if (at) {
        close(at->fd);
        free(at);
    }
}

int aotable_truncate(aot_t *at)
{
    if (at) {
        ftruncate(at->fd,0);
    }
    return(0);
}

mut_t *aotable_recover(aot_t *at)
{
    melem_t *me;
    mut_t *mt;
    int klen,vlen;
    char *aotfp,*pos,*kptr,*vptr;
    struct stat aot_stat;

    if (!at)
        return(NULL);
    fstat(at->fd,&aot_stat);
    if (aot_stat.st_size == 0)
        return(NULL);
    if (!(mt = mutable_new()))
        return(NULL);
    if (!(aotfp = mmap(NULL,aot_stat.st_size,PROT_READ,MAP_PRIVATE,at->fd,0))) {
        free(mt);
        return(NULL);
    } else
        pos = aotfp;
    while (pos - aotfp < aot_stat.st_size - 2*sizeof(int32_t)) {
        klen = *(int32_t *)pos;
        kptr = pos + sizeof(int32_t);
        pos += sizeof(int32_t) + klen;
        vlen = *(int32_t *)pos;
        vptr = pos + sizeof(int32_t);
        pos += sizeof(int32_t) + vlen;
        if (pos - aotfp > aot_stat.st_size)
            break; /* Invalid keyvalue, the data have been damaged */

        if (!(me = melem_new(_LEVEL_RANDOM,kptr,vptr)))
            continue;
        mutable_add(mt,me);
    }
    munmap(aotfp,aot_stat.st_size);
    return(mt);
}

int aotable_append(aot_t *at, slice_t key, slice_t value)
{
    if (!at || !key || !value)
        return(-1);
    if (_SLICE_HL(key) + _SLICE_HL(value) > _PAGE_SIZE) {
        write(at->fd,_SLICE_HP(key),_SLICE_HL(key));
        write(at->fd,_SLICE_HP(value),_SLICE_HL(value));
    } else {
        memcpy(at->aot_buf,_SLICE_HP(key),_SLICE_HL(key));
        memcpy(at->aot_buf + _SLICE_HL(key),_SLICE_HP(value),_SLICE_HL(value));
        write(at->fd,at->aot_buf,_SLICE_HL(key) + _SLICE_HL(value));    
    }
    if (at->flags & AOT_SYNC) fsync(at->fd);
    return(0);
}

