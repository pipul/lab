/* db.c - A database handle implementation
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
#include <sys/types.h>
#include <dirent.h>
#include "lotus.h"




DB *db_open(const char *path, int flags)
{
    int i;
    sst_t *sst;
    DB *db;
    DIR *dp;
    struct dirent *dir;
    char aotfile[_PATH_MAX] = {0}, sstfile[_PATH_MAX] = {0};

    if (!path || !(dp = opendir(path)))
        return(NULL);
    if (!(db = calloc(1,sizeof(DB)))) {
        closedir(dp);
        return(NULL);
    }
    for (i = 0; i < _SLOTTAG_MAX; i++)
        db->db_slot[i] = sloter_new();

    db->db_flags = flags;
    snprintf(db->db_path,_PATH_MAX,"%s",path);
    while ((dir = readdir(dp)) != NULL) {
        if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0)
            continue;
        if (strstr(dir->d_name,_AOT_SUFFIX) != NULL) {
            snprintf(aotfile,_PATH_MAX,"%s/%s",path,dir->d_name);
            db->apponlyfile = aotable_open(aotfile,AOT_OPEN);
            db->memtable = aotable_recover(db->apponlyfile);
            aotable_close(db->apponlyfile);
            if (db->memtable) {
                snprintf(sstfile,_PATH_MAX,
                         "%s/%d%s",path,time(NULL),_SST_SUFFIX);
                mutable_dump(db->memtable,sstfile);
                remove(aotfile);
                if (sst = sstable_open(sstfile,SST_RDONLY))
                    sloter_add(db->db_slot[sst->stat.slottag],sst);
            }
            db->apponlyfile = NULL;
            db->memtable = NULL;
        }
        if (strstr(dir->d_name,_SST_SUFFIX) != NULL) {
            snprintf(sstfile,_PATH_MAX,"%s/%s",path,dir->d_name);
            if (!(sst = sstable_open(sstfile,SST_RDONLY))) {
                __DEBUG();
                continue;
            }
            sloter_add(db->db_slot[sst->stat.slottag],sst);
        }
    }
    snprintf(aotfile,_PATH_MAX,"%s/%d%s",path,time(NULL),_AOT_SUFFIX);
    if (db->db_flags & DB_SYNC)
        db->apponlyfile = aotable_open(aotfile,AOT_CREAT|AOT_SYNC,0644);
    else
        db->apponlyfile = aotable_open(aotfile,AOT_CREAT,0644);

    if (!(db->memtable = mutable_new()))
        __DEBUG();
    db->close = _db_close;
    db->del = _db_del;
    db->put = _db_put;
    db->get = _db_get;
    db->fflush = _db_fflush;

    db->_sst_link = __db_sst_link;
    db->_sst_unlink = __db_sst_unlink;
    closedir(dp);
    return(db);
}

int _db_close(DB *db)
{
    int i;

    if (!db)
        return(-1);
    mutable_free(db->memtable);
    aotable_close(db->apponlyfile);
    for (i = 0; i < _SLOTTAG_MAX; i++)
        sloter_free(db->db_slot[i]);
    free(db);
    return(0);
}

int _db_del(const DB *db, slice_t key)
{
    melem_t *me;
    slice_t __N;

    if (!db || !key)
        return(-1);
    __N = slice_new("",0);
    if (!(me = mutable_lookup(db->memtable,key))) {
        if (!(me = melem_new(_LEVEL_RANDOM,key,__N)))
            return(-1);
        mutable_add(db->memtable,me);
        aotable_append(db->apponlyfile,me->key,me->value);
    } else {
        slice_free(me->value);
        me->value = slice_dup(__N);;
        aotable_append(db->apponlyfile,me->key,me->value);
    }
    slice_free(__N);
    return(0);
}

int _db_put(const DB *db, slice_t key, slice_t value)
{
    melem_t *me;

    if (!db || !key || !value)
        return(-1);
    if (!(me = melem_new(_LEVEL_RANDOM,key,value)))
        return(-1);
    if (0 == mutable_add(db->memtable,me))
        aotable_append(db->apponlyfile,me->key,me->value);
    return(0);
}

slice_t _db_get(const DB *db, slice_t key)
{
    int i,j;
    slice_t value;
    
    for (i = 0; i < _SLOTTAG_MAX; i++) {
        for (j = 0; j < db->db_slot[i]->size; j++) {
            if (value = sstable_lookup(db->db_slot[i]->sstfiles[j],key))
                return(value);
        }
    }
    return(NULL);
}

int _db_fflush(DB *db)
{
    sst_t *sst;
    char filename[_PATH_MAX] = {0};

    if (!db)
        return(-1);
    snprintf(filename,_PATH_MAX,"%s/%d%s",db->db_path,time(NULL),_SST_SUFFIX);
    if (0 == mutable_dump(db->memtable,filename)) {
        if (!(sst = sstable_open(filename,SST_RDONLY))) {
            __DEBUG();
            return(-1);
        }
        sloter_add(db->db_slot[sst->stat.slottag],sst);
        aotable_truncate(db->apponlyfile);
    }
    return(0);
}

int __db_sst_link(DB *db, char *sst_name)
{
    char filename[_PATH_MAX] = {0};
    sst_t *sst;

    if (!db || !sst_name)
        return(-1);
    snprintf(filename,_PATH_MAX,"%s/%s",db->db_path,sst_name);
    if (!(sst = sstable_open(filename,SST_RDONLY)))
        return(-1);
    return(sloter_add(db->db_slot[sst->stat.slottag],sst));
}
int __db_sst_unlink(DB *db, char *sst_name)
{
    int i;
    sst_t *sst = NULL;

    if (!db || !sst_name)
        return(-1);
    for (i = 0; i < _SLOTTAG_MAX; i++)
        if (sst = sloter_get(db->db_slot[i],sst_name))
            break;
    if (sst) {
        sloter_del(db->db_slot[sst->stat.slottag],sst);
        sstable_close(sst);
        remove(sst_name); /* when unlink a sstable file, remove it */
    }
    return(0);
}
