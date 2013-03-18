/* sst.c - A persistant storage file implementation
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
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "lotus.h"

#define __HEAD(s) (s->header->forward[0])
#define __NEXT(s) (s->forward[0])



static int _random_lev()
{
    int lev = 1;
    while ((rand()&0xFFFF) < (_SKIPLIST_P * 0xFFFF))
        lev += 1;
    return(lev < _LEVEL_MAX) ? lev : _LEVEL_MAX;
}


selem_t *selem_new(int lev)
{
    selem_t *se;

    if (lev == _LEVEL_RANDOM)
        lev = _random_lev();
    se = calloc(1,sizeof(selem_t) + lev*sizeof(selem_t *));
    if (se)
        se->level = lev;
    return(se);
}

void selem_free(selem_t *se)
{
    free(se);
}

int selem_cmp(selem_t *se, selem_t *de)
{
    if (!se && !de)
        return(0);
    if (!se)
        return(-1);
    if (!de)
        return(1);
    if (se->khash > de->khash)
        return(1);
    if (se->khash == de->khash)
        return(0);
    else
        return(-1);
}

slist_t *slist_new()
{
    slist_t *sl;

    if (!(sl = calloc(1,sizeof(slist_t))))
        return(NULL);
    if (!(sl->header = selem_new(_LEVEL_MAX))) {
        free(sl);
        return(NULL);
    }
    sl->selem_cmp = selem_cmp;
    return(sl);
}

int slist_free(slist_t *sl)
{
    selem_t *se,*next;

    if (!sl)
        return(-1);
    se = sl->header;
    while (se) {
        next = se->forward[0];
        selem_free(se);
        se = next;
    }
    free(sl);
}

int slist_add(slist_t *sl, selem_t *se)
{
    int i;
    selem_t *curse,*update[_LEVEL_MAX - 1];

    if (!sl || !se)
        return(-1);
    curse = sl->header;
    for (i = sl->maxlevel - 1; i >= 0; i--) {
        while (curse->forward[i] && sl->selem_cmp(curse->forward[i],se) < 0)
            curse = curse->forward[i];
        update[i] = curse;
    }
    if (se->level > sl->maxlevel) {
        for (i = se->level - 1; i > sl->maxlevel - 1; i--)
            update[i] = sl->header;
        sl->maxlevel = se->level;
    }
    for (i = se->level - 1; i >= 0; i--) {
        se->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = se;
    }
    sl->size++;
    return(0);
}


int slist_del(slist_t *sl, selem_t *se)
{
    int i;
    selem_t *curse,*update[_LEVEL_MAX - 1];

    if (!sl || !se)
        return(-1);
    curse = sl->header;
    for (i = sl->maxlevel - 1; i >=0; i--) {
        while (curse->forward[i] && sl->selem_cmp(curse->forward[i],se) < 0)
            curse = curse->forward[i];
        update[i] = curse;
    }
    if (!(curse = curse->forward[0]))
        return(-1);
    for (i = se->level - 1; i >= 0; i--)
        update[i]->forward[i] = curse->forward[i];
    if (se->level = sl->maxlevel) {
        for (i = se->level - 1; i >= 0; i--)
            if (sl->header->forward[i])
                break;
        sl->maxlevel = i + 1;
    }
    sl->size--;
    return(0);
}


selem_t *slist_get(slist_t *sl, uint32_t kh, int *sc)
{
    int i,selemc = 0;
    selem_t *curse,*_se,tmpse;

    if (!sl)
        return(NULL);
    tmpse.khash = kh;
    curse = sl->header;
    for (i = sl->maxlevel - 1; i >= 0; i--)
        while (curse->forward[i] && sl->selem_cmp(curse->forward[i],&tmpse) < 0)
            curse = curse->forward[i];
    _se = curse->forward[0];
    if (sl->selem_cmp(_se,&tmpse) != 0)
        return(NULL);
    curse = _se->forward[0];
    selemc++;
    while (sl->selem_cmp(curse,&tmpse) == 0) {
        selemc++;
        curse = curse->forward[0];
    }
    if (sc)
        *sc = selemc;
    return(_se);
}

smeta_t *smeta_new(int lev)
{
    smeta_t *sm;

    if (lev == _LEVEL_RANDOM)
        lev = _random_lev();
    if (!(sm = calloc(1,sizeof(smeta_t) + lev*sizeof(smeta_t *))))
        return(NULL);
    if (!(sm->list = slist_new())) {
        free(sm);
        return(NULL);
    }
    sm->level = lev;
    return(sm);
}

int smeta_free(smeta_t *sm)
{
    if (!sm)
        return(-1);
    slice_free(sm->key);
    slist_free(sm->list);
    free(sm);
    return(0);
}

int smeta_cmp(smeta_t *sm, smeta_t *dm)
{
    if (!sm && !dm)
        return(0);
    if (!sm) return(-1);
    if (!dm) return(1);
    return(slice_cmp(sm->key,dm->key));
}

sidx_t *sidx_new()
{
    sidx_t *si;

    if (!(si = calloc(1,sizeof(sidx_t))))
        return(NULL);
    if (!(si->header = smeta_new(_LEVEL_MAX))) {
        free(si);
        return(NULL);
    }
    si->smeta_cmp = smeta_cmp;
    return(si);
}

int sidx_free(sidx_t *si)
{
    smeta_t *cursm,*next;

    if (!si)
        return(-1);
    cursm = si->header;
    while (cursm) {
        next = cursm->forward[0];
        smeta_free(cursm);
        cursm = next;
    }
    free(si);
}

int sidx_add(sidx_t *si, smeta_t *sm)
{
    int i;
    smeta_t *cursm, *update[_LEVEL_MAX - 1];
    
    if (!si || !sm)
        return(-1);

    cursm = si->header;
    for (i = si->maxlevel - 1; i >= 0; i--) {
        while (cursm->forward[i] && si->smeta_cmp(cursm->forward[i],sm) < 0)
            cursm = cursm->forward[i];
        update[i] = cursm;
    }
    if (sm->level > si->maxlevel) {
        for (i = sm->level - 1; i > si->maxlevel - 1; i--)
            update[i] = si->header;
        si->maxlevel = sm->level;
    }
    for (i = sm->level - 1; i >= 0; i--) {
        sm->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = sm;
    }
    si->size++;
    return(0);
}

int sidx_del(sidx_t *si, smeta_t *sm)
{
    int i;
    smeta_t *cursm, *update[_LEVEL_MAX - 1];

    if (!si || !sm)
        return(-1);
    cursm = si->header;
    for (i = si->maxlevel - 1; i >= 0; i--) {
        while (cursm->forward[i] && si->smeta_cmp(cursm->forward[i],sm) < 0)
            cursm = cursm->forward[i];
        update[i] = cursm;
    }
    cursm = cursm->forward[0];
    if (cursm != sm)
        return(-1);
    for (i = sm->level - 1; i >= 0; i--)
        update[i]->forward[i] = cursm->forward[i];
    if (sm->level >= si->maxlevel) {
        for (i = sm->level - 1; i >= 0; i--)
            if (si->header->forward[i] != NULL)
                break;
        si->maxlevel = i + 1;
    }
    si->size--;
	return(0);
}

smeta_t *sidx_get(sidx_t *si, slice_t key)
{
    int i;
    smeta_t *cursm, tmpsm;

    if (!si || !key)
        return(NULL);

    cursm = si->header;
    tmpsm.key = key;
    for (i = si->maxlevel - 1; i >= 0; i--)
        while (cursm->forward[i] &&
               si->smeta_cmp(cursm->forward[i],&tmpsm) <= 0)
            cursm = cursm->forward[i];
    if (cursm == si->header) cursm = NULL;
    return(cursm);   
}

sstat_t *sstat_new()
{
    return(calloc(1,sizeof(sstat_t)));
}

void sstat_free(sstat_t *ss)
{
    free(ss);
}



static int
 sstable_path(const sst_t *st, char *path, int len)
{
    int __len;
    char __path[_PATH_MAX] = {0};

    if (!st || !path)
        return(-1);
    snprintf(__path,_PATH_MAX,"%s",st->sst_name);
    __len = strlen(__path);
    while (__len > 0) {
        if (__path[__len - 1] == '/') {
            __path[__len - 1] = '\0';
            break;
        }
        __len--;
    }
    if (!__len)
        __path[__len] = '\0';
    snprintf(path,len,"%s",__path);
    return(0);
}


sst_t *sstable_open(const char *sst_name, int flags)
{
    void *sstptr;
    int fd;
    sst_t *st;
    selem_t *se;
    slist_t *list;
    smeta_t *meta;
    struct stat sst_stat;

    if (!sst_name || (!(flags & SST_CPONLY) && !(flags & SST_RDONLY)))
        return(NULL);
    if (!(st = calloc(1,sizeof(sst_t))))
        return(NULL);

    snprintf(st->sst_name,_PATH_MAX,"%s",sst_name);
    if ((fd = open(sst_name,O_RDONLY|O_EXCL)) < 0) {
        free(st);
        return(NULL);
    }
    fstat(fd,&sst_stat);
    st->mmaptr = mmap(NULL,sst_stat.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    if (!st->mmaptr) {
        close(fd);
        free(st);
        return(NULL);
    }
    st->flags = flags;

    /* Closing the file descriptor dose not unmap the region */
    close(fd);

    /* Reading the sstable stat infomation from file */
    sstptr = st->mmaptr + sst_stat.st_size - sizeof(sstat_t);
    memcpy(&st->stat,sstptr,sizeof(sstat_t));

    /* hashv(4B) + klen(4B) + kdata + vlen(4B) + vdata */
    st->sk = slice_dup(st->mmaptr + st->stat.skoff);
    st->ek = slice_dup(st->mmaptr + st->stat.ekoff);

    if (st->flags & SST_CPONLY)
        return(st);


    /* Creating index and bloomfilter from sstable stat. */
    st->index = sidx_new();
    st->bloom = sbloom_new(st->stat.selemnum,_BLOOM_P);

    /* Reading the bloomfilter data from sstable file */
    sstptr = st->mmaptr + st->stat.bloomoff;
    memcpy(st->bloom,sstptr,3*sizeof(int32_t)); /* size + mbits + hashn */
    memcpy(st->bloom->s,sstptr + 3*sizeof(int32_t),st->bloom->size);

    /* Reading the index data from sstable file */
    sstptr = st->mmaptr + st->stat.idxoff;
    while (sstptr < st->mmaptr + st->stat.idxsize + st->stat.idxoff) {
        if (!(meta = smeta_new(_LEVEL_RANDOM)))
            continue;
        meta->key = slice_dup(sstptr + sizeof(int32_t));
        sstptr += _SLICE_HL(meta->key);
        /* boffset + bsize */
        memcpy(&meta->boffset,sstptr,2*sizeof(int32_t));
        sstptr += 2*sizeof(int32_t);
        sidx_add(st->index,meta);
    }

    /* Reading the entry hash from sstable file */
    for (meta = st->index->header->forward[0]; meta; meta = meta->forward[0]) {
        sstptr = st->mmaptr + meta->boffset;
        while (sstptr < st->mmaptr + meta->boffset + meta->bsize) {
            se = selem_new(_LEVEL_RANDOM);
            se->khash = *(uint32_t *)sstptr;
            sstptr += sizeof(uint32_t);
            se->klen = *(int32_t *)sstptr;
            se->kptr = sstptr + sizeof(int32_t);
            sstptr = se->kptr + se->klen;
            se->vlen = *(int32_t *)sstptr;
            se->vptr = sstptr + sizeof(int32_t);
            sstptr = se->vptr + se->vlen;
            slist_add(meta->list,se);
        }
    }
    
    return(st);
}

int sstable_close(sst_t *st)
{
    struct stat sst_stat;

    if (!st)
        return(-1);
    stat(st->sst_name,&sst_stat);
    munmap(st->mmaptr,sst_stat.st_size);
    slice_free(st->sk);
    slice_free(st->ek);
    sbloom_free(st->bloom);
    sidx_free(st->index);
    free(st);
    return(0);
}


slice_t sstable_lookup(const sst_t *st, slice_t key)
{
    uint32_t hashv;
    int elemc;
    selem_t *se;
    slice_t __v = NULL;
    smeta_t *meta;

    if (!st || !key || (st->flags & SST_CPONLY))
        return(NULL);
    if (slice_cmp(st->sk,key) > 0 || slice_cmp(st->ek,key) < 0)
        return(NULL);
    if (!(meta = sidx_get(st->index,key)))
        return(NULL);
    hashv = md5_encode(key,slice_len(key));
    if (!(se = slist_get(meta->list,hashv,&elemc)))
        return(NULL);
    while (se->khash == hashv) {
        if (se->klen != slice_len(key)) {
            se = se->forward[0];
            continue;
        }
        if (slice_cmp(se->kptr,key) == 0) {
            __v = se->vptr;
            break;
        }
        se = se->forward[0];
    }
    return(__v);
}





typedef struct lane {
    void **sk;
    int size,*selemnum;
    int32_t *stimestamp;
} lane_t;

static lane_t *__lane_new(int size, sst_t **sts);
static void __lane_free(lane_t *la);
static slice_t __lane_popmink(lane_t *la);

static lane_t *__lane_new(int size, sst_t **sts)
{
    int i;
    lane_t *la;

    if (!sts)
        return(NULL);
    if (!(la = calloc(1,sizeof(lane_t))))
        return(NULL);
    la->size = size;
    la->sk = calloc(la->size + 1,sizeof(void *));
    la->selemnum = calloc(la->size + 1,sizeof(int));
    la->stimestamp = calloc(la->size + 1,sizeof(int32_t));
    for (i = 0; i < la->size; i++) {
        la->sk[i] = sts[i]->mmaptr;
        la->selemnum[i] = sts[i]->stat.selemnum;
        la->stimestamp[i] = sts[i]->stat.timestamp;
    }
    return(la);
}

static void __lane_free(lane_t *la)
{
    if (la) {
        free(la->sk);
        free(la->selemnum);
        free(la->stimestamp);
        free(la);
    }
}
        /* sstable kv format 
         * ------------------------------------------------
         *  hashv(4B) klen(4B) kdata(..) vlen(4B) vdata(..)
         * ------------------------------------------------ */
static void *__skip(void *sk)
{
    int32_t klen,vlen;
    slice_t kptr,vptr;

    if (!sk)
        return(NULL);
    kptr = sk + 2*sizeof(int32_t);
    vptr = kptr + slice_len(kptr) + sizeof(int32_t);
    return(vptr + slice_len(vptr));
}

static slice_t __lane_popmink(lane_t *la)
{
    int i,j,cr,__tmpen,__movelen;
    void *__tmpsk,*__popsk;
    int32_t __tmptimestamp;

    if (!la || !la->size)
        return(NULL);
    for (i = la->size - 1; i > 0; i--) {
        cr = slice_cmp(la->sk[i-1] +
                       2*sizeof(int32_t),la->sk[i] + 2*sizeof(int32_t));
        if (cr > 0) {
            __tmpsk = la->sk[i-1];
            la->sk[i-1] = la->sk[i];
            la->sk[i] = __tmpsk;
            __tmpen = la->selemnum[i-1];
            la->selemnum[i-1] = la->selemnum[i];
            la->selemnum[i] = __tmpen;
            __tmptimestamp = la->stimestamp[i-1];
            la->stimestamp[i-1] = la->stimestamp[i];
            la->stimestamp[i] = __tmptimestamp;
        } else if (cr == 0) {
            j = la->stimestamp[i-1] < la->stimestamp[i] ? i-1 : i;
            la->selemnum[j]--;
            if (la->selemnum[j]) {
                la->sk[j] = __skip(la->sk[j]);
                i++;
                continue;
            } else {
                __movelen = (la->size-j)*sizeof(void *);
                memmove(&la->sk[j],&la->sk[j+1],__movelen);
                memmove(&la->selemnum[j],&la->selemnum[j+1],__movelen);
                memmove(&la->stimestamp[j],&la->stimestamp[j+1],__movelen);
                la->size--;
            }
        }
    }
    __popsk = la->sk[0];
    la->sk[0] = __skip(la->sk[0]);
    la->selemnum[0]--;
    if (la->selemnum[0] == 0 && la->size > 0) {
        __movelen = (la->size - 0)*sizeof(void *);
        memmove(&la->sk[0],&la->sk[1],__movelen);
        memmove(&la->selemnum[0],&la->selemnum[1],__movelen);
        memmove(&la->stimestamp[0],&la->stimestamp[1],__movelen);
        la->size--;
    }
    return(__popsk);
}

int sstable_merge(int stc, sst_t **sts, sst_t **dt)
{
    lane_t *la;
    int i,fd,rlen,blocksize;
    FILE *fp;
    smeta_t *meta;
    void *__cursk,*__nexsk;
    char path[_PATH_MAX] = {0};

    if (!sts || !dt)
        return(-1);
    if (!(*dt = calloc(1,sizeof(sst_t))))
        return(-1);
    sstable_path(*sts,path,_PATH_MAX);
    if (!strlen(path))
        /* current path */
        snprintf(path,_PATH_MAX,"%s","./");
    do {
        snprintf((*dt)->sst_name,
                 _PATH_MAX,"%s/%d%s",path,time(NULL),_SST_SUFFIX);
        fd = open((*dt)->sst_name,O_WRONLY|O_CREAT|O_EXCL,0644);
    } while (fd < 0);
    fp = fdopen(fd,"a");
    (*dt)->index = sidx_new();
    for (i = 0; i < stc; i++) {
        if (sts[i]->stat.timestamp >= (*dt)->stat.timestamp)
            (*dt)->stat.timestamp = sts[i]->stat.timestamp;
        if (sts[i]->stat.slottag >= (*dt)->stat.slottag)
            (*dt)->stat.slottag = sts[i]->stat.slottag;
    }

    la = __lane_new(stc,sts);
    while (__cursk = __lane_popmink(la))
        (*dt)->stat.selemnum++;
    __lane_free(la);
    la = __lane_new(stc,sts);

    (*dt)->bloom = sbloom_new((*dt)->stat.selemnum,_BLOOM_P);
    (*dt)->stat.selemnum = 0;
    if (!(*dt)->index || !(*dt)->bloom) {
        sstable_close(*dt);
        close(fd);
        fclose(fp);
        return(-1);
    }
    __cursk = __nexsk = NULL;
    for (;;) {
        blocksize = _BLOCK_SIZE;
        if (!__cursk && !(__cursk = __lane_popmink(la)))
            break;
        if (!(meta = smeta_new(_LEVEL_RANDOM)))
            continue;
        meta->key = __cursk + 2*sizeof(int32_t);
        meta->boffset = ftello(fp);
        while (__cursk && blocksize > 0) {
            rlen = __skip(__cursk) - __cursk;
            blocksize -= rlen;
            sbloom_set((*dt)->bloom,__cursk + 2*sizeof(int32_t));
            __nexsk = __lane_popmink(la);
            if (!__nexsk) {
                (*dt)->stat.skoff = 0 + 2*sizeof(int32_t);
                (*dt)->stat.ekoff = ftello(fp) + 2*sizeof(int32_t);
            }
            fwrite(__cursk,rlen,1,fp);
            (*dt)->stat.selemnum++;
            __cursk = __nexsk;
        }
        meta->bsize = _BLOCK_SIZE - blocksize;
        sidx_add((*dt)->index,meta);
        (*dt)->stat.smetanum++;
    }
    (*dt)->stat.bloomoff = ftello(fp);
    fwrite(&(*dt)->bloom->size,sizeof(int32_t),1,fp);
    fwrite(&(*dt)->bloom->mbits,sizeof(int32_t),1,fp);
    fwrite(&(*dt)->bloom->hashn,sizeof(int32_t),1,fp);
    fwrite((*dt)->bloom->s,(*dt)->bloom->size,1,fp);
    (*dt)->stat.bloomsize = ftello(fp) - (*dt)->stat.bloomoff;

    (*dt)->stat.idxoff = ftello(fp);
    for (meta=(*dt)->index->header->forward[0];meta;meta=meta->forward[0]) {
        fwrite(_SLICE_HP(meta->key),_SLICE_HL(meta->key),1,fp);
        meta->key = NULL;
        fwrite(&(meta->boffset),sizeof(int32_t),1,fp);
        fwrite(&(meta->bsize),sizeof(int32_t),1,fp);
    }
    (*dt)->stat.idxsize = ftello(fp) - (*dt)->stat.idxoff;
    fwrite(&(*dt)->stat,sizeof(sstat_t),1,fp);
    fflush(fp);
    fclose(fp);
    fsync(fd);
    close(fd);
    snprintf(path,_PATH_MAX,"%s",(*dt)->sst_name);
    sstable_close(*dt);
    /* open the new sstable file and return handle */
    *dt = sstable_open(path,SST_CPONLY);
    __lane_free(la);
    return(0);
}


int sstable_split(sst_t *st, int dtc, sst_t ***dts)
{
    lane_t *la;
    int i,rlen,blocksize,elemn;
    int *fds;
    FILE **fps;
    smeta_t *meta;
    void *__cursk,*__nexsk;
    char path[_PATH_MAX] = {0};

    if (!st || !dts)
        return(-1);
    la = __lane_new(1,&st);
    fps = calloc(dtc,sizeof(FILE *));
    fds = calloc(dtc,sizeof(int *));
    *dts = calloc(dtc,sizeof(sst_t *));
    for (i = 0; i < dtc; i++) {
        sstable_path(st,path,_PATH_MAX);
        if (!strlen(path))
            /* current path */
            snprintf(path,_PATH_MAX,"%s","./");
        (*dts)[i] = calloc(1,sizeof(sst_t));
        do {
            snprintf((*dts)[i]->sst_name,
                     _PATH_MAX,"%s/%d%s",path,time(NULL),_SST_SUFFIX);
            fds[i] = open((*dts)[i]->sst_name,O_WRONLY|O_CREAT|O_EXCL,0644);
            fps[i] = fdopen(fds[i],"a");
        } while (!fps[i]);
        (*dts)[i]->index = sidx_new();
        (*dts)[i]->stat.timestamp = st->stat.timestamp;
        (*dts)[i]->stat.slottag = st->stat.slottag;
        if (i < dtc - 1)
            (*dts)[i]->stat.selemnum = ((st->stat.selemnum)/dtc);
        else
            (*dts)[i]->stat.selemnum =
                st->stat.selemnum - i*((st->stat.selemnum)/dtc);
        (*dts)[i]->bloom = sbloom_new((*dts)[i]->stat.selemnum,_BLOOM_P);

        __cursk = __nexsk = NULL;
        elemn = (*dts)[i]->stat.selemnum;
        while (elemn > 0) {
            blocksize = _BLOCK_SIZE;
            __cursk = __lane_popmink(la);
            meta = smeta_new(_LEVEL_RANDOM);
            meta->key = __cursk + 2*sizeof(int32_t);
            meta->boffset = ftello(fps[i]);
            while (__cursk) {
                rlen = __skip(__cursk) - __cursk;
                blocksize -= rlen;
                sbloom_set((*dts)[i]->bloom,__cursk + 2*sizeof(int32_t));
                elemn--;
                if (!elemn) {
                    (*dts)[i]->stat.skoff = 0 + 2*sizeof(int32_t);
                    (*dts)[i]->stat.ekoff = ftello(fps[i]) + 2*sizeof(int32_t);
                }
                fwrite(__cursk,rlen,1,fps[i]);
                if (elemn == 0 || blocksize < 0)
                    break;
                __cursk = __lane_popmink(la);
            }
            meta->bsize = _BLOCK_SIZE - blocksize;
            sidx_add((*dts)[i]->index,meta);
            (*dts)[i]->stat.smetanum++;
        }
        (*dts)[i]->stat.bloomoff = ftello(fps[i]);
        fwrite(&(*dts)[i]->bloom->size,sizeof(int32_t),1,fps[i]);
        fwrite(&(*dts)[i]->bloom->mbits,sizeof(int32_t),1,fps[i]);
        fwrite(&(*dts)[i]->bloom->hashn,sizeof(int32_t),1,fps[i]);
        fwrite((*dts)[i]->bloom->s,(*dts)[i]->bloom->size,1,fps[i]);
        (*dts)[i]->stat.bloomsize = ftello(fps[i]) - (*dts)[i]->stat.bloomoff;
        (*dts)[i]->stat.idxoff = ftello(fps[i]);
        for (meta = __HEAD((*dts)[i]->index); meta; meta = __NEXT(meta)) {
            fwrite(_SLICE_HP(meta->key),_SLICE_HL(meta->key),1,fps[i]);
            meta->key = NULL;
            fwrite(&(meta->boffset),sizeof(int32_t),1,fps[i]);
            fwrite(&(meta->bsize),sizeof(int32_t),1,fps[i]);
        }
        (*dts)[i]->stat.idxsize = ftello(fps[i]) - (*dts)[i]->stat.idxoff;
        fwrite(&(*dts)[i]->stat,sizeof(sstat_t),1,fps[i]);
        fflush(fps[i]);
        fclose(fps[i]);
        fsync(fds[i]);
        close(fds[i]);
        snprintf(path,_PATH_MAX,"%s",(*dts)[i]->sst_name);
        sstable_close((*dts)[i]);
        (*dts)[i] = sstable_open(path,SST_CPONLY);
    }
    __lane_free(la);
    free(fds);
    free(fps);
    return(0);
}
