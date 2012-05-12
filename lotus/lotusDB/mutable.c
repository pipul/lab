/* mutable.c - A memory table implementation
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
#include <fcntl.h>
#include "lotus.h"

static int _random_lev()
{
    int lev = 1;
    while ((rand()&0xFFFF) < (_SKIPLIST_P * 0xFFFF))
        lev += 1;
    return(lev < _LEVEL_MAX) ? lev : _LEVEL_MAX;
}


melem_t *melem_new(int lev, slice_t key, slice_t value)
{
    melem_t *me;

    if (lev == _LEVEL_RANDOM)
        lev = _random_lev();
    me = calloc(1,sizeof(melem_t) + lev*sizeof(melem_t *));
    if (me) {
        me->level = lev;
        me->key = slice_dup(key);
        me->value = slice_dup(value);
    }
    return(me);
}

void melem_free(melem_t *me)
{
    if (me) {
        slice_free(me->key);
        slice_free(me->value);
    }
    free(me);
}

int melem_cmp(melem_t *se, melem_t *de)
{
    if (!se || !de) {
        if (se)
            return(1);
        if (de)
            return(-1);
    }
    return(slice_cmp(se->key,de->key));
}



mut_t *mutable_new()
{
    mut_t *mt;

    if (!(mt = calloc(1,sizeof(mut_t))))
        return(NULL);
    if (!(mt->header = melem_new(_LEVEL_MAX,NULL,NULL)))
        return(NULL);
    mt->melem_cmp = melem_cmp;
    return(mt);
}

void mutable_free(mut_t *mt)
{
    melem_t *me, *next;

    if (mt) {
        me = mt->header;
        while (me) {
            next = me->forward[0];
            melem_free(me);
            me = next;
        }
        free(mt);
    }
}

melem_t *mutable_lookup(mut_t *mt, slice_t key)
{
    int i;
    melem_t *curkv, tmpkv;
    
    if (!mt || !key)
        return(NULL);
    tmpkv.key = key;
    curkv = mt->header;
    for (i = mt->maxlevel - 1; i >= 0; i--)
        while (curkv->forward[i] != NULL &&
         mt->melem_cmp(curkv->forward[i],&tmpkv) < 0)
            curkv = curkv->forward[i];
    curkv = curkv->forward[0];
    if (mt->melem_cmp(curkv,&tmpkv) == 0)
        return(curkv);
    return(NULL);
}



int mutable_add(mut_t *mt, melem_t *kv)
{
    int i;
    melem_t *curkv, *update[_LEVEL_MAX - 1];

    if (!mt || !kv)
        return(-1);

    curkv = mt->header;
    for (i = mt->maxlevel - 1; i >= 0; i--) {
        while (curkv->forward[i] != NULL &&
         mt->melem_cmp(curkv->forward[i],kv) < 0)
            curkv = curkv->forward[i];
        update[i] = curkv;
    }
    if (kv->level > mt->maxlevel) {
        for (i = kv->level - 1; i > mt->maxlevel - 1; i--)
            update[i] = mt->header;
        mt->maxlevel = kv->level;
    }
    
    for (i = kv->level - 1; i >= 0; i--) {
        kv->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = kv;
    }
    mt->size++;
    mt->a_time = time(NULL);
    return(0);
}

int mutable_del(mut_t *mt, melem_t *kv)
{
    int i,klen;
    melem_t *curkv, *update[_LEVEL_MAX - 1];

    if (!mt || !kv)
        return(-1);

    curkv = mt->header;
    for (i = mt->maxlevel - 1; i >= 0; i--) {
        while (curkv->forward[i] != NULL &&
         mt->melem_cmp(curkv->forward[i],kv) < 0)
            curkv = curkv->forward[i];
        update[i] = curkv;
    }
    curkv = curkv->forward[0];
    if (curkv != kv)
        return(-1);
    for (i = kv->level - 1; i >= 0; i--)
        update[i]->forward[i] = curkv->forward[i];
    if (kv->level >= mt->maxlevel) {
        for (i = kv->level - 1; i >= 0; i--)
            if (mt->header->forward[i] != NULL)
                break;
        mt->maxlevel = i + 1;
    }
	mt->size--;
	return(0);
}

int mutable_dump(mut_t *mt, const char *sst_name)
{
    uint32_t hashv;
    int fd,blocksize;
    FILE *sstfp;
    melem_t *kv;
    smeta_t *meta;
    sidx_t *index;
    sbloom_t *bloom;
    sstat_t sst_stat = {0,0,0,0,0,0,0,0,0,0,0};

    if (!mt || !sst_name)
        return(-1);
    /* this sstable does exist */
    if ((fd = open(sst_name,O_WRONLY|O_CREAT|O_EXCL,0644)) < 0)
        return(-1);
    sstfp = fdopen(fd,"a");
    index = sidx_new();
    bloom = sbloom_new(mt->size,_BLOOM_P);
    if (!index || !bloom) {
        sidx_free(index);
        sbloom_free(bloom);
        fclose(sstfp);
        return(-1);
    }

    for (kv = mt->header->forward[0]; kv;) {
        blocksize = _BLOCK_SIZE;
        if (!(meta = smeta_new(_LEVEL_RANDOM)))
            continue;
        meta->key = kv->key;
        meta->boffset = ftello(sstfp);
        while (kv && blocksize > 0) {

            hashv = md5_encode(kv->key,slice_len(kv->key));
            sbloom_set(bloom,kv->key);
            sst_stat.selemnum++;
            /* the last one key, save the sk and ek off into stat */
            if (!kv->forward[0]) {
                sst_stat.skoff = 0 + 2*sizeof(int32_t);
                sst_stat.ekoff = ftello(sstfp) + 2*sizeof(int32_t);
            }

            fwrite(&hashv,sizeof(uint32_t),1,sstfp);
            fwrite(_SLICE_HP(kv->key),_SLICE_HL(kv->key),1,sstfp);
            fwrite(_SLICE_HP(kv->value),_SLICE_HL(kv->value),1,sstfp);
            blocksize -= _SLICE_HL(kv->key) +
                _SLICE_HL(kv->value) + sizeof(uint32_t);
            kv = kv->forward[0];
        }
        meta->bsize = _BLOCK_SIZE - blocksize;
        sidx_add(index,meta);
        sst_stat.smetanum++;
    }

    sst_stat.bloomoff = ftello(sstfp);
    fwrite(&(bloom->size),sizeof(int32_t),1,sstfp);
    fwrite(&(bloom->mbits),sizeof(int32_t),1,sstfp);
    fwrite(&(bloom->hashn),sizeof(int32_t),1,sstfp);
    fwrite(bloom->s,bloom->size,1,sstfp);
    sst_stat.bloomsize = ftello(sstfp) - sst_stat.bloomoff;

    sst_stat.idxoff = ftello(sstfp);
    for (meta = index->header->forward[0]; meta; meta = meta->forward[0]) {
        fwrite(_SLICE_HP(meta->key),_SLICE_HL(meta->key),1,sstfp);
        meta->key = NULL;
        fwrite(&(meta->boffset),sizeof(int32_t),1,sstfp);
        fwrite(&(meta->bsize),sizeof(int32_t),1,sstfp);
    }
    sst_stat.idxsize = ftello(sstfp) - sst_stat.idxoff;
    sst_stat.timestamp = time(NULL);

    fwrite(&sst_stat,sizeof(sstat_t),1,sstfp);
    fflush(sstfp);
    sidx_free(index);
    sbloom_free(bloom);
    if (-1 == fsync(fd))
        remove(sst_name);
    close(fd);
    fclose(sstfp);
    return(0);
}


int mutable_cat(mut_t *sm, mut_t *dm)
{
    melem_t *me;

    if (!sm || !dm)
        return(-1);
    while (me = dm->header->forward[0]) {
        mutable_del(dm,me);
        mutable_add(sm,me);
    }
    mutable_free(dm);
    return(0);
}
