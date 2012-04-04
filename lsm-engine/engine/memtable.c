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


#include "memtable.h"


MET *memTableCreate(void)
{
    MET *list;
   
    if ((list = malloc(sizeof(MET))) == NULL)
        return(NULL);
    memset(list,0,sizeof(MET));
    if ((list->_head = entryCreateByLevel(LEVEL_MAX)) == NULL) {
        memTableFree(list);
        return(NULL);
    }
    list->compareEntry = entryCompare;
    return(list);
}

void memTableFree(MET *list)
{
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
        while (curNode->forward[i] != NULL && list->compareEntry(curNode->forward[i],&tmpNode) < 0)
            curNode = curNode->forward[i];
    curNode = curNode->forward[0];
    if (list->compareEntry(curNode,&tmpNode) == 0)
        return(curNode);
    return(NULL);
}

int32_t memTableInsertEntry(MET *list, entry_t *node)
{
    entry_t *curNode, *update[LEVEL_MAX - 1];
    int32_t i;

    if (!list || !node)
        return(-1);
    curNode = list->_head;
    for (i = list->maxlevel - 1; i >= 0; i--) {
        while (curNode->forward[i] != NULL && list->compareEntry(curNode->forward[i],node) < 0)
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
    return(0);
}

int32_t memTableDeleteEntry(MET *list, entry_t *node)
{
    int32_t i;
    entry_t *curNode, *update[LEVEL_MAX - 1];

    if (!list || !node)
        return(-1);
    curNode = list->_head;
    for (i = list->maxlevel - 1; i >= 0; i--) {
        while (curNode->forward[i] != NULL && list->compareEntry(curNode->forward[i],node) < 0)
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
	return(0);
}

#define DB_PATH "dbase"


SST *memTableDumpToSStable(MET *list)
{
    SST *sst;
    int32_t sstfd;
    meta_t *curMeta;
    entry_t *curEntry;

    if (!list || list->items == 0)
        return(NULL);
    if ((sst = ssTableCreate()) == NULL)
        return(NULL);
    sst->trailer = sstTrailerCreate();
    sst->fileinfo = sstInfoCreate();
    sst->metas = sstIndexCreate();
    sst->bloom = sstBloomCreate(list->items,BLOOM_P);
    if (!sst->trailer || !sst->fileinfo || !sst->metas || !sst->bloom) {
        ssTableDestroy(sst);
        return(NULL);
    }
    for (curEntry =
     memTableHeader(list); curEntry != NULL; curEntry = entryNext(curEntry))
        sstBloomInsertKey(sst->bloom,curEntry->key);
    sst->fileinfo->entrycount = list->items;
    sst->fileinfo->lastkey = sdsdup((memTableTailer(list))->key);
    
    
    int32_t blocksize;
    do {
        blocksize = BLOCK_SIZE;
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
    } while (curEntry != NULL);
    
    sst->trailer->timestamp = time(NULL);
    do {
        snprintf(sst->s_name,PATH_MAX,"%s/%ld.sst",DB_PATH,time(NULL));
        sstfd = open(sst->s_name,O_WRONLY|O_APPEND|O_CREAT|O_EXCL,0644);
    } while (sstfd < 0);
    for (curMeta = sstIndexHeader(sst->metas); curMeta != NULL; curMeta = metaNext(curMeta)) {
        curMeta->offset = lseek(sstfd,0,SEEK_CUR);
        curMeta->blocksize = sstBlockDumpIntoSStable(sstfd,curMeta->offset,curMeta->block);
    }
    sst->trailer->indexoffset = lseek(sstfd,0,SEEK_CUR);
    sst->trailer->indexsize = sstIndexDumpIntoSStable(sstfd,sst->trailer->indexoffset,sst->metas);
    sst->trailer->bloomoffset = lseek(sstfd,0,SEEK_CUR);
    sst->trailer->bloomsize = sstBloomDumpIntoSStable(sstfd,sst->trailer->bloomoffset,sst->bloom);
    sst->trailer->infooffset = lseek(sstfd,0,SEEK_CUR);
    sst->trailer->infosize = sstInfoDumpIntoSStable(sstfd,sst->trailer->infooffset,sst->fileinfo);
    sstTrailerDumpIntoSStable(sstfd,lseek(sstfd,0,SEEK_CUR),sst->trailer);
    close(sstfd);
    return(sst);
}
