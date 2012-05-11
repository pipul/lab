#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "lotus.h"


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
            if (!(se = selem_new(_LEVEL_RANDOM)))
                continue;
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
    if (st->flags & SST_RDONLY) {
        sbloom_free(st->bloom);
        sidx_free(st->index);
    }
    free(st);
    return(0);
}


slice_t sstable_lookup(sst_t *st, slice_t key)
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
