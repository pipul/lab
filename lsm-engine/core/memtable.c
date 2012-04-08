/* memtable.c - A memory table implementation
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
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "memtable.h"

MET *memTableCreate(int32_t flag, ...)
{
    MET *list;
    va_list ap;

    if ((list = malloc(sizeof(MET))) == NULL)
        return(NULL);
    if (!(flag & AOF_ON) && !(flag & AOF_OFF))
        return(NULL);
    memset(list,0,sizeof(MET));
    if ((list->_head = entryCreateByLevel(_LEVEL_MAX)) == NULL) {
        free(list);
        return(NULL);
    }
    list->compareEntry = entryCompare;
    list->aof_flag = flag;
    if (flag & AOF_ON) {
        va_start(ap,flag);
        list->aof_fd = va_arg(ap,int32_t);
        va_end(ap);
    }
    
    list->aof_buf.free = AOF_BUFFERSIZE;
    do {
        list->aof_buf.data = malloc(AOF_BUFFERSIZE);
    } while (list->aof_buf.data == NULL);

    return(list);
}

static void memTableFree(MET *list)
{
    free(list->aof_buf.data);
    free(list);
}

void memTableDestroy(MET *list)
{
    entry_t *node, *next;

    if (!list)
        return;
    node = list->_head->forward[0];
    entryDestroy(list->_head);
    while (node) {
        next = node->forward[0];
        entryDestroy(node);
        node = next;
    }
    memTableFree(list);
}

int32_t memTableControl(MET *list, int32_t flag)
{
    int32_t aof_recentflag = list->aof_flag;

    if (flag & AOF_ON) {
        list->aof_flag = list->aof_flag & (~AOF_OFF);
        list->aof_flag = list->aof_flag | AOF_ON;
        list->aof_buf.len = 0;
        list->aof_buf.free = AOF_BUFFERSIZE;
        if (flag & AOF_SYNC)
            list->aof_flag = list->aof_flag | AOF_SYNC;
    } else if (flag & AOF_OFF) {
        list->aof_flag = list->aof_flag & (~AOF_ON);
        list->aof_flag = list->aof_flag | AOF_OFF;
    }
    return(aof_recentflag);
}



entry_t *memTableHeader(MET *list)
{
    if (!list)
        return(NULL);
    return(list->_head->forward[0]);
}

entry_t *memTableTailer(MET *list)
{
    int32_t i;
    entry_t *node = list->_head;

    if (!list)
        return(NULL);
    for (i = list->maxlevel; i >= 0; i--)
        while (node->forward[0] != NULL)
            node = node->forward[0];
    if (node == list->_head)
        return(NULL);
    return(node);
}

entry_t *memTableFind(MET *list, sds key)
{
    int32_t i;
    entry_t *curNode, tmpNode;
    
    if (!list || !key)
        return(NULL);
    tmpNode.key = key;
    curNode = list->_head;
    for (i = list->maxlevel - 1; i >= 0; i--)
        while (curNode->forward[i] != NULL &&
         list->compareEntry(curNode->forward[i],&tmpNode) < 0)
            curNode = curNode->forward[i];
    curNode = curNode->forward[0];
    if (list->compareEntry(curNode,&tmpNode) == 0)
        return(curNode);
    return(NULL);
}


int32_t memTableInsertEntry(MET *list, entry_t *node)
{
    int32_t i,klen,vlen;
    entry_t *curNode, *update[_LEVEL_MAX - 1];

    if (!list || !node)
        return(-1);

    if (list->aof_flag & AOF_ON) {
        klen = sdslen(node->key) + sizeof(int32_t);
        vlen = sdslen(node->value) + sizeof(int32_t);
        if (list->aof_buf.free >= entrySize(node)) {
            memcpy(list->aof_buf.data,node->key - sizeof(sdshdr),klen);
            memcpy(list->aof_buf.data + klen,node->value - sizeof(sdshdr),vlen);
            write(list->aof_fd,list->aof_buf.data,klen + vlen);
        } else {
            write(list->aof_fd,node->key - sizeof(sdshdr),klen);
            write(list->aof_fd,node->value - sizeof(sdshdr),vlen);
        }
        if (list->aof_flag & AOF_SYNC)
            fsync(list->aof_fd);
    }


    curNode = list->_head;
    for (i = list->maxlevel - 1; i >= 0; i--) {
        while (curNode->forward[i] != NULL &&
         list->compareEntry(curNode->forward[i],node) < 0)
            curNode = curNode->forward[i];
        update[i] = curNode;
    }
    if (node->level > list->maxlevel) {
        for (i = node->level - 1; i > list->maxlevel - 1; i--)
            update[i] = list->_head;
        list->maxlevel = node->level;
    }
    
    for (i = node->level - 1; i >= 0; i--) {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }
    list->items++;
    list->timestamp = time(NULL);
    return(0);
}

int32_t memTableDeleteEntry(MET *list, entry_t *node)
{
    int32_t i,klen;
    entry_t *curNode, *update[_LEVEL_MAX - 1];

    if (!list || !node)
        return(-1);
    
    if (list->aof_flag & AOF_ON) {
        klen = sdslen(node->key) + sizeof(int32_t);
        if (list->aof_buf.free >= klen) {
            memcpy(list->aof_buf.data,node->key - sizeof(sdshdr),klen);
            write(list->aof_fd,list->aof_buf.data,klen);
        } else
            write(list->aof_fd,node->key - sizeof(sdshdr),klen);
        if (list->aof_flag & AOF_SYNC)
            fsync(list->aof_fd);
    }
    
    curNode = list->_head;
    for (i = list->maxlevel - 1; i >= 0; i--) {
        while (curNode->forward[i] != NULL &&
         list->compareEntry(curNode->forward[i],node) < 0)
            curNode = curNode->forward[i];
        update[i] = curNode;
    }
    curNode = curNode->forward[0];
    if (curNode != node)
        return(-1);
    for (i = node->level - 1; i >= 0; i--)
        update[i]->forward[i] = curNode->forward[i];
    if (node->level >= list->maxlevel) {
        for (i = node->level - 1; i >= 0; i--)
            if (list->_head->forward[i] != NULL)
                break;
        list->maxlevel = i + 1;
    }
	list->items--;
//    list->timestamp = time(NULL);
	return(0);
}


int32_t memTableDumpToSStable(MET *list, SST *sst)
{
    int32_t sstfd,aof_flag,naof_fd;
    meta_t *curMeta;
    entry_t *curEntry;
    int8_t aof_fileName[_PATH_MAX], aof_fileLink[_PATH_MAX];

    if (!list || list->items == 0 || !sst)
        return(-1);
    /* Save the origin aof_flag and turn off the AOF function. Because during
     * the dumping process memTableDeleteEntry() operate will cause an AOF 
     * persistence transaction. after that, it recovers the aof_flag.
     */
    aof_flag = memTableControl(list,AOF_OFF);

    if (sst->trailer == NULL)
        sst->trailer = sstTrailerCreate();
    if (sst->fileinfo == NULL)
        sst->fileinfo = sstInfoCreate();
    if (sst->metas == NULL)
        sst->metas = sstIndexCreate();
    if (sst->bloom == NULL)
        sst->bloom = sstBloomCreate(list->items,BLOOM_P);

    if (!sst->trailer || !sst->fileinfo || !sst->metas || !sst->bloom)
        return(-1);

    for (curEntry = memTableHeader(list);
     curEntry != NULL; curEntry = entryNext(curEntry))
        sstBloomInsertKey(sst->bloom,curEntry->key);
    sst->fileinfo->entrycount = list->items;
    sst->fileinfo->lastkey = sdsdup((memTableTailer(list))->key);
    sst->trailer->timestamp = time(NULL);
    
    int32_t blocksize;
    do {
        /* Each Data block contains at less one entries or more. */
        blocksize = _BLOCK_SIZE;
        curEntry = memTableHeader(list);
        if ((curMeta = metaCreate()) == NULL)
            continue;
        if ((curMeta->block = sstBlockCreate()) == NULL) {
            metaFree(curMeta);
            continue;
        }
        
        blocksize = blocksize - sizeof(uint64_t) - entrySize(curEntry);
        memTableDeleteEntry(list,curEntry);
        sstBlockInsertEntry(curMeta->block,curEntry);
        
        curEntry = memTableHeader(list);
        while (curEntry && entrySize(curEntry) < blocksize) {
            memTableDeleteEntry(list,curEntry);
            sstBlockInsertEntry(curMeta->block,curEntry);
            blocksize = blocksize - entrySize(curEntry);
            curEntry = memTableHeader(list);
        }
        curMeta->key = sdsdup((sstBlockHeader(curMeta->block))->key);
        curMeta->state = IN_CACHE;
        sstIndexInsertMeta(sst->metas,curMeta);
        sst->fileinfo->blockcount++;
    } while (curEntry != NULL);

    do {
        sstfd = open(sst->s_name,O_WRONLY|O_APPEND);
    } while (sstfd < 0);

    for (curMeta = sstIndexHeader(sst->metas);
     curMeta != NULL; curMeta = metaNext(curMeta)) {
        curMeta->offset = lseek(sstfd,0,SEEK_CUR);
        curMeta->blocksize =
         sstBlockDumpIntoSStable(sstfd,curMeta->offset,curMeta->block);
    }

    sst->trailer->indexoffset = lseek(sstfd,0,SEEK_CUR);
    sst->trailer->indexsize =
     sstIndexDumpIntoSStable(sstfd,sst->trailer->indexoffset,sst->metas);
    sst->trailer->bloomoffset = lseek(sstfd,0,SEEK_CUR);
    sst->trailer->bloomsize =
     sstBloomDumpIntoSStable(sstfd,sst->trailer->bloomoffset,sst->bloom);
    sst->trailer->infooffset = lseek(sstfd,0,SEEK_CUR);
    sst->trailer->infosize =
     sstInfoDumpIntoSStable(sstfd,sst->trailer->infooffset,sst->fileinfo);
    sstTrailerDumpIntoSStable(sstfd,lseek(sstfd,0,SEEK_CUR),sst->trailer);
    close(sstfd);

    /* When a memTable was dumped on disk successfully, the aof file will be
     * truncated to a zero size. as you see, that is new aof persistence file.
     */
    ftruncate(list->aof_fd,0);
    lseek(list->aof_fd,0,SEEK_SET);
    memTableControl(list,aof_flag);

    return(0);
}

int32_t aofileLoadToMemtable(MET *list, int32_t aof_fd)
{
    entry_t *node;
    int32_t aof_flag;
    struct stat aof_stat;
    int8_t *aof_ptr, *buffer;

    fstat(aof_fd,&aof_stat);
    if ((buffer = mmap(NULL,
     aof_stat.st_size,PROT_READ,MAP_PRIVATE,aof_fd,0)) == NULL)
        return(-1);
    else
        aof_ptr = buffer;
    aof_flag = memTableControl(list,AOF_OFF);
    while (aof_ptr - buffer < aof_stat.st_size) {
        do {
            node = entryCreate();
        } while (node == NULL);
        node->key = sdsdup(aof_ptr + sizeof(int32_t));
        aof_ptr = aof_ptr + sdslen(node->key) + sizeof(int32_t);
        node->value = sdsdup(aof_ptr + sizeof(int32_t));
        aof_ptr = aof_ptr + sdslen(node->value) + sizeof(int32_t);
        memTableInsertEntry(list,node);
    }
    munmap(buffer,aof_stat.st_size);
    list->aof_fd = aof_fd;
    list->aof_flag = AOF_ON;
    lseek(list->aof_fd,0,SEEK_END);
    memTableControl(list,aof_flag);
    return(0);
}
