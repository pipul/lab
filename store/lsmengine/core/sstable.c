/* sstable.c - A persistant storage file implementation
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
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include "sstable.h"
#include "utils.h"

static int32_t slRandomLevel(void)
{
    int level = 1;
    while ((rand()&0xFFFF) < (_SKIPLIST_P * 0xFFFF))
        level += 1;
    return (level < _LEVEL_MAX) ? level : _LEVEL_MAX;
}



/* The Data Block segment ------->
 * To store key/value pairs, may be compressed. each block is the
 * unit of transfer to and from persistent storage. The default
 * block size is approximately 1mb uncompressed bytes.
 */

int32_t entryCompare(entry_t *aNode, entry_t *bNode)
{
    if (!aNode || !bNode) {
        if (aNode)
            return(1);
        if (bNode)
            return(-1);
    }
    return(sdscmp(aNode->key,bNode->key));
}

entry_t *entryCreateByLevel(int32_t level)
{
    entry_t *node;
    if ((node = malloc(sizeof(entry_t) + level*sizeof(entry_t *))) == NULL)
        return(NULL);
    memset(node,0,sizeof(entry_t) + level*sizeof(entry_t *));
    node->level = level;
    return(node);
}

entry_t *entryCreate(void)
{
    return(entryCreateByLevel(slRandomLevel()));
}

void entryFree(entry_t *node)
{
    free(node);
}

void entryDestroy(entry_t *node)
{
    if (node == NULL)
        return;
    sdsdel(node->key);
    sdsdel(node->value);
    entryFree(node);
}

entry_t *entryNext(entry_t *node)
{
    if (node)
        return(node->forward[0]);
    else
        return(NULL);
}


int32_t entrySize(entry_t *node)
{
    if (!node)
        return(-1);
    return(2*sizeof(int32_t) + sdslen(node->key) + sdslen(node->value));
}

int32_t entryValid(entry_t *node)
{
    if (!node)
        return(-1);
    if (sdslen(node->value) == 0)
        return(-1);
    return(0);
}


sstblock_t *sstBlockCreate(void)
{
    sstblock_t *list;

    if ((list = malloc(sizeof(sstblock_t))) == NULL)
        return(NULL);
    memset(list,0,sizeof(sstblock_t));
    if ((list->_head = entryCreateByLevel(_LEVEL_MAX)) == NULL) {
        sstBlockFree(list);
        return(NULL);
    }
    list->compareEntry = entryCompare;
    return(list);
}

void sstBlockFree(sstblock_t *list)
{
	free(list);
}

void sstBlockDestroy(sstblock_t *list)
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
    sstBlockFree(list);
}

int32_t sstBlockSize(sstblock_t *list)
{
    entry_t *node;
    int32_t bsize;

    if (!list)
        return(-1);
    bsize = sizeof(uint64_t);
    for (node = sstBlockHeader(list); node != NULL; node = entryNext(node))
        bsize = bsize + entrySize(node);
    return(bsize);
}

entry_t *sstBlockHeader(sstblock_t *list)
{
    if (!list)
        return(NULL);
    return(list->_head->forward[0]);
}

entry_t *sstBlockTailer(sstblock_t *list)
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


entry_t *sstBlockPickupMin(sstblock_t *list)
{
    entry_t *min;
    
    if (!list)
        return(NULL);
    min = sstBlockHeader(list);
    sstBlockDeleteEntry(list,min);
    return(min);
}

entry_t *sstBlockPickupMax(sstblock_t *list)
{
    entry_t *max;
    
    if (!list)
        return(NULL);
    max = sstBlockTailer(list);
    sstBlockDeleteEntry(list,max);
    return(max);
}


int32_t sstBlockInsertEntry(sstblock_t *list, entry_t *node)
{
    entry_t *curNode, *update[_LEVEL_MAX - 1];
    int32_t i;

    if (!list || !node)
        return(-1);
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
    
    list->_count++;
    return(0);
}

int32_t sstBlockDeleteEntry(sstblock_t *list, entry_t *node)
{
    entry_t *curNode, *update[_LEVEL_MAX - 1];
    int32_t i;

    if (!list || !node)
        return(-1);
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
    list->_count--;
	return(0);
}

entry_t *sstBlockFind(sstblock_t *list, sds key)
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

sstblock_t *sstBlockLoadFromSStable(int32_t fd, int32_t offset, int32_t size)
{
    int32_t len;
    uint64_t magic;
    sstblock_t *list;
    entry_t *node;
    int8_t *buffer, *ptr;

    if (fd < 0 || offset < 0 || size < 0)
        return(NULL);
    if ((list = sstBlockCreate()) == NULL)
        return(NULL);
    if ((buffer = malloc(size)) == NULL) {
        sstBlockFree(list);
        return(NULL);
    }
    do {
        lseek(fd,offset,SEEK_SET);
    } while (size != read(fd,buffer,size));

    ptr = buffer;
    magic = *(uint64_t *)ptr;
    ptr = ptr + sizeof(uint64_t);
    if (magic != crc32_encode(ptr,size - sizeof(uint64_t))) {
        sstBlockFree(list);
        free(buffer);
        return(NULL);
    }

    while (ptr - buffer < size) {
        do {
            node = entryCreate();
        } while (node == NULL);
        len = *(int32_t *)ptr;
        ptr = ptr + sizeof(int32_t);
        node->key = sdsnnew(ptr,len);
        ptr = ptr + len;

        len = *(int32_t *)ptr;
        ptr = ptr + sizeof(int32_t);
        node->value = sdsnnew(ptr,len);
        ptr = ptr + len;

        if (node->key && node->value) {
            sstBlockInsertEntry(list,node);
            continue;
        }
        if (node->key)
            sdsdel(node->key);
        if (node->value)
            sdsdel(node->value);
    }

    free(buffer);
    return(list);
}


int32_t sstBlockDumpIntoSStable(int32_t fd, int32_t offset, sstblock_t *list)
{
    entry_t *node;
    int32_t len,size;
    int8_t *buffer, *ptr;

    if (fd < 0 || offset < 0 || !list)
        return(-1);
    if (-1 == fcntl(fd,F_SETFL,O_APPEND))
        return(-1);
    size = sstBlockSize(list);
    if ((buffer = malloc(size)) == NULL)
        return(-1);
    else
        ptr = buffer + sizeof(uint64_t);

    for (node = sstBlockHeader(list); node != NULL; node = entryNext(node)) {
        len = sdslen(node->key);
        memcpy(ptr,&len,sizeof(int32_t)); 
        ptr = ptr + sizeof(int32_t);
        memcpy(ptr,node->key,len);
        ptr = ptr + len;

        len = sdslen(node->value);
        memcpy(ptr,&len,sizeof(int32_t));
        ptr = ptr + sizeof(int32_t);
        memcpy(ptr,node->value,len);
        ptr = ptr + len;
    }

    ptr = buffer + sizeof(uint64_t);
    *(uint64_t *)buffer = crc32_encode(ptr,size - sizeof(uint64_t));

    do {
        lseek(fd,offset,SEEK_SET);
    } while (size != write(fd,buffer,size));

    free(buffer);
    return(size);
}







/* The Data Block Index segment ------->
 * Indexes the data block offset in the HFile. The key of each meta
 * is the key of first key/value pair in the block. as you see, index
 * is the list of meta node.
 */


int32_t metaCompare(meta_t *aNode, meta_t *bNode)
{
    if (!aNode || !bNode) {
        if (!aNode && !bNode)
            return(0);
        if (!aNode)
            return(-1);
        if (!bNode)
            return(-1);
    }
    return(sdscmp(aNode->key,bNode->key));
}

meta_t *metaCreateByLevel(int32_t level)
{
    meta_t *node;

    if ((node = malloc(sizeof(meta_t) + level*sizeof(meta_t *))) == NULL)
        return(NULL);
    memset(node,0,sizeof(meta_t) + level*sizeof(meta_t *));
    node->level = level;
    return(node);
}

meta_t *metaCreate(void)
{
    return(metaCreateByLevel(slRandomLevel()));
}

void metaFree(meta_t *node)
{
	free(node);
}

void metaDestroy(meta_t *node)
{
    if (node == NULL)
        return;
    sdsdel(node->key);
    if (node->state == IN_CACHE)
        sstBlockDestroy(node->block);
    metaFree(node);
}

meta_t *metaNext(meta_t *node)
{
    if (!node)
        return(NULL);
    return(node->forward[0]);
}



sstindex_t *sstIndexCreate(void)
{
    sstindex_t *list;

    if ((list = malloc(sizeof(sstindex_t))) == NULL)
        return(NULL);
    memset(list,0,sizeof(sstindex_t));
    if ((list->_head = metaCreateByLevel(_LEVEL_MAX)) == NULL) {
        sstIndexFree(list);
        return(NULL);
    }
    list->compareMeta = metaCompare;
    return(list);
}

void sstIndexFree(sstindex_t *list)
{
    free(list);
}

void sstIndexDestroy(sstindex_t *list)
{
    meta_t *curNode, *next;

    if (!list)
        return;
    curNode = list->_head->forward[0];
    metaDestroy(list->_head);
    while (curNode) {
        next = curNode->forward[0];
        metaDestroy(curNode);
        curNode = next;
    }
    sstIndexFree(list);
}

meta_t *sstIndexHeader(sstindex_t *list)
{
    if (!list)
        return(NULL);
    return(list->_head->forward[0]);
}

meta_t *sstIndexTailer(sstindex_t *list)
{
    int32_t i;
    meta_t *node = list->_head;

    if (!list)
        return(NULL);
    for (i = list->maxlevel; i >= 0; i--)
        while (node->forward[0] != NULL)
            node = node->forward[0];
    if (node == list->_head)
        return(NULL);
    return(node);
}

int32_t sstIndexInsertMeta(sstindex_t *list, meta_t *node)
{
    int32_t i;
    meta_t *curNode, *update[_LEVEL_MAX - 1];
    
    if (!list || !node)
        return(-1);

    curNode = list->_head;
    for (i = list->maxlevel - 1; i >= 0; i--) {
        while (curNode->forward[i] != NULL &&
         list->compareMeta(curNode->forward[i],node) < 0)
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
    return(0);
}

int32_t sstIndexDeleteMeta(sstindex_t *list, meta_t *node)
{
    int32_t i;
    meta_t *curNode, *update[_LEVEL_MAX - 1];

    if (!list || !node)
        return(-1);
    curNode = list->_head;
    for (i = list->maxlevel - 1; i >= 0; i--) {
        while (curNode->forward[i] != NULL &&
         list->compareMeta(curNode->forward[i],node) < 0)
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
	return(0);
}

meta_t *sstIndexFind(sstindex_t *list, sds key)
{
    int32_t i;
    meta_t *curNode, tmpNode;

    if (!list || !key)
        return(NULL);

    curNode = list->_head;
    tmpNode.key = key;
    for (i = list->maxlevel - 1; i >= 0; i--)
        while (curNode->forward[i] != NULL &&
         list->compareMeta(curNode->forward[i],&tmpNode) <= 0)
            curNode = curNode->forward[i];
    if (curNode != list->_head)
        return(curNode);
    return(NULL);
}


sstindex_t *sstIndexLoadFromSStable(int32_t fd, int32_t offset, int32_t size)
{
    int32_t len;
    uint64_t magic;
    meta_t *node;
    sstindex_t *list;
    int8_t *buffer, *ptr;

    if (fd < 0 || offset < 0 || size < 0)
        return(NULL);
    if ((list = sstIndexCreate()) == NULL)
        return(NULL);
    if ((buffer = malloc(size)) == NULL) {
        sstIndexFree(list);
        return(NULL);
    }
    do {
        lseek(fd,offset,SEEK_SET);
    } while (size != read(fd,buffer,size));

    ptr = buffer;
    magic = *(uint64_t *)ptr;
    ptr = ptr + sizeof(uint64_t);
    if (magic != crc32_encode(ptr,size - sizeof(uint64_t))) {
        sstIndexFree(list);
        free(buffer);
        return(NULL);
    }

    while (ptr - buffer < size) {
        if ((node = metaCreate()) == NULL)
            break;
        node->offset = *(int32_t *)ptr;
        ptr = ptr + sizeof(int32_t);
		node->blocksize = *(int32_t *)ptr;
		ptr = ptr + sizeof(int32_t);

        len = *(int32_t *)ptr;
        ptr = ptr + sizeof(int32_t);
        node->key = sdsnnew(ptr,len);
        ptr = ptr + len;

        if (node->key == NULL) {
            metaFree(node);
            continue;
		}
        sstIndexInsertMeta(list,node);
    }

    free(buffer);
    return(list);
}




int32_t sstIndexDumpIntoSStable(int32_t fd, int32_t offset, sstindex_t *list)
{
    meta_t *node;
    int32_t len,size;
    int8_t *buffer, *ptr;

    if (fd < 0 || !list)
        return(-1);
    if (-1 == fcntl(fd,F_SETFL,O_APPEND))
        return(-1);
    size = sizeof(uint64_t);
    for (node = sstIndexHeader(list); node != NULL; node = metaNext(node))
        size = size + 3*sizeof(int32_t) + sdslen(node->key);
    if ((buffer = malloc(size)) == NULL)
        return(-1);
    else
        ptr = buffer + sizeof(uint64_t);

    for (node = sstIndexHeader(list); node != NULL; node = metaNext(node)) {
        memcpy(ptr,&node->offset,sizeof(int32_t));
        ptr = ptr + sizeof(int32_t);

        memcpy(ptr,&node->blocksize,sizeof(int32_t));
        ptr = ptr + sizeof(int32_t);

        len = sdslen(node->key);
        memcpy(ptr,&len,sizeof(int32_t));
        ptr = ptr + sizeof(int32_t);
        memcpy(ptr,node->key,len);
        ptr = ptr + len;
    }

    ptr = buffer + sizeof(uint64_t);
    *(uint64_t *)buffer = crc32_encode(ptr,size - sizeof(uint64_t));

    do {
        lseek(fd,offset,SEEK_SET);
    } while (size != write(fd,buffer,size));

    free(buffer);
    return(size);
}









/* The Bloom-filter imprementation  ------->
 * Bloom-filter is a space-efficient probabilistic data structure
 * that is used to test whether an element is a member of a set.
 */



#define SETBIT(s,n) (s[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(s,n) (s[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))

#define _ALIGN 8
#define _S_round_up(__bytes)\
     (((__bytes) + (uint32_t) _ALIGN-1) & ~((uint32_t) _ALIGN - 1))



sstbloom_t *sstBloomCreate(uint32_t n, float p)
{
    sstbloom_t *bloom;

    if (!(bloom = malloc(sizeof(sstbloom_t))))
        return(NULL);

    bloom->mbits = ceil(n*log(p)/(-log(2)*log(2)));
    bloom->size = _S_round_up(bloom->mbits)/CHAR_BIT;

    if (!(bloom->s = calloc(bloom->size,sizeof(int8_t)))) {
        free(bloom);
        return(NULL);
    }

    bloom->hashn = floor(0.7*(bloom->mbits)/n);
    bloom->hash = md532_encode;

    return(bloom);
}

void sstBloomDestroy(sstbloom_t *bloom)
{
    if (bloom == NULL)
        return;
    free(bloom->s);
    free(bloom);
}

int32_t sstBloomInsertKey(sstbloom_t *bloom, const sds key)
{
    int32_t n;
    int32_t slen;
    int8_t buf[_KEY_MAX + 1];

    slen = sdslen(key);
    memcpy(buf,key,slen);
    buf[slen+1] = '\0';

    for (n = 0; n < bloom->hashn; n++) {
        buf[slen] = n + 1;
        SETBIT(bloom->s, (*bloom->hash)(buf,slen)%bloom->mbits);
    }

    return(0);
}

int32_t sstBloomCheckKey(sstbloom_t *bloom, const sds key)
{
    int32_t n;
    int32_t slen;
    int8_t buf[_KEY_MAX + 1];

    slen = sdslen(key);
    memcpy(buf,key,slen);
    buf[slen+1] = '\0';

    for (n = 0; n < bloom->hashn; n++) {
        buf[slen] = n + 1;
        if (!(GETBIT(bloom->s, (*bloom->hash)(buf,slen)%bloom->mbits)))
            return(-1);
    }

    return(0);
}

int32_t sstBloomInsertKeySets(sstbloom_t *bloom, sstblock_t *list)
{
    entry_t *node;

    for (node = sstBlockHeader(list); node != NULL; node = entryNext(node))
        sstBloomInsertKey(bloom,node->key);
        
    return(0);
}

sstbloom_t *sstBloomLoadFromSStable(int32_t fd, int32_t offset, int32_t size)
{
    sstbloom_t *bloom;
    int8_t *buffer, *ptr;

    if (fd < 0 || offset < 0 || size < 0)
        return(NULL);
    if ((buffer = malloc(size)) == NULL)
        return(NULL);
    if ((bloom = malloc(sizeof(sstbloom_t))) == NULL) {
        free(buffer);
        return(NULL);
    }
    memset(bloom,0,sizeof(sstbloom_t));
    do {
        lseek(fd,offset,SEEK_SET);
	} while (size != read(fd,buffer,size));
    memcpy(bloom,buffer,3*sizeof(int32_t));
    ptr = buffer + 3*sizeof(int32_t);
    if ((bloom->s = malloc(bloom->size)) == NULL) {
        sstBloomDestroy(bloom);
        free(buffer);
        return(NULL);
    }

    memcpy(bloom->s,ptr,bloom->size);
    bloom->hash = &md532_encode;

    free(buffer);
    return(bloom);
}


int32_t sstBloomDumpIntoSStable(int32_t fd, int32_t offset, sstbloom_t *bloom)
{
    int32_t size;
    int8_t *buffer, *ptr;
	
    if (fd < 0 || offset < 0 || !bloom)
        return(-1);
    size = 3*sizeof(int32_t) + bloom->size;
    if ((buffer = malloc(size)) == NULL)
        return(-1);
    memset(buffer,0,size);
    memcpy(buffer,bloom,3*sizeof(int32_t));
    ptr = buffer + 3*sizeof(int32_t);
    memcpy(ptr,bloom->s,bloom->size);

    do {
        lseek(fd,offset,SEEK_SET);
    } while (size != write(fd,buffer,size));

    free(buffer);
    return(size);
}







/* The SStable info segment  ------->
 * It is a small metadata of the storage File, without compression.
 * developer can add user defined small metadata (name/value) here.
 * and rebuild the project.
 */


sstinfo_t *sstInfoCreate(void)
{
    sstinfo_t *ssi;

    if ((ssi = malloc(sizeof(sstinfo_t))) == NULL)
        return(NULL);
    memset(ssi,0,sizeof(sstinfo_t));
    return(ssi);
}

void sstInfoFree(sstinfo_t *ssi)
{
	free(ssi);
}

void sstInfoDestroy(sstinfo_t *ssi)
{
    if (ssi == NULL)
        return;
    sdsdel(ssi->lastkey);
    sstInfoFree(ssi);
}

sstinfo_t *sstInfoLoadFromSStable(int32_t fd, int32_t offset, int32_t size)
{
    uint64_t magic;
    sstinfo_t *ssi;
    int8_t *buffer, *ptr;

    if (fd < 0 || offset < 0 || size < 0)
        return(NULL);
    if ((ssi = sstInfoCreate()) == NULL)
        return(NULL);
    if ((buffer = malloc(size)) == NULL) {
        sstInfoFree(ssi);
        return(NULL);
    }
    do {
        lseek(fd,offset,SEEK_SET);
    } while (size != read(fd,buffer,size));
    
    ptr = buffer;
    magic = *(int64_t *)ptr;
    ptr = ptr + sizeof(uint64_t);
    if (magic != crc32_encode(ptr,size - sizeof(uint64_t))) {
        free(buffer);
        sstInfoFree(ssi);
        return(NULL);
    }
    ssi->blockcount = *(int32_t *)ptr;
    ptr = ptr + sizeof(int32_t);
    ssi->entrycount = *(int32_t *)ptr;
    ptr = ptr + sizeof(int32_t);
    ssi->lastkey = sdsnnew(ptr + sizeof(int32_t),*(int32_t *)ptr);

    free(buffer);
    return(ssi);
}


int32_t sstInfoDumpIntoSStable(int32_t fd, int32_t offset, sstinfo_t *ssi)
{
    int32_t size;
    int8_t *buffer, *ptr;

    if (fd < 0 || offset < 0 || !ssi)
        return(-1);
    if (-1 == fcntl(fd,F_SETFL,O_APPEND))
        return(-1);
    size = sizeof(uint64_t) + 3*sizeof(int32_t) + sdslen(ssi->lastkey);
    if ((buffer = malloc(size)) == NULL)
        return(-1);
    ptr = buffer + sizeof(uint64_t);
    memcpy(ptr,&ssi->blockcount,sizeof(int32_t));
    ptr = ptr + sizeof(int32_t);
    memcpy(ptr,&ssi->entrycount,sizeof(int32_t));
    ptr = ptr + sizeof(int32_t);
    memcpy(ptr,
     ssi->lastkey - sizeof(sdshdr), sdslen(ssi->lastkey) + sizeof(int32_t));

    ptr = buffer + sizeof(uint64_t);
    *(uint64_t *)buffer = crc32_encode(ptr,size - sizeof(uint64_t));

    do {
        lseek(fd,offset,SEEK_SET);
    } while (size != write(fd,buffer,size));

    free(buffer);
    return(size);
}







/* The SStable Trailer segment  ------->
 * The fix sized metadata. To hold the offset and size of each segment,
 * etc. To read an HFile, we should always read the Trailer firstly.
 */


ssttrailer_t *sstTrailerCreate()
{
    ssttrailer_t *ssr;
    if ((ssr = malloc(sizeof(ssttrailer_t))) == NULL)
        return(NULL);
    memset(ssr,0,sizeof(ssttrailer_t));
    ssr->version = CUR_VERSION;
    return(ssr);
}

void sstTrailerFree(ssttrailer_t *ssr)
{
    free(ssr);
}

ssttrailer_t *sstTrailerLoadFromSStable(int32_t fd, int32_t offset, int32_t size)
{
    ssttrailer_t *ssr;

    if (fd < 0 || size < 0)
        return(NULL);
    if ((ssr = sstTrailerCreate()) == NULL)
        return(NULL);
    do {
        lseek(fd,offset,SEEK_END);
    } while (size != read(fd,ssr,size));
    return(ssr);
}

int32_t sstTrailerDumpIntoSStable(int32_t fd, int32_t offset, ssttrailer_t *ssr)
{
    int32_t size;
    if (fd < 0 || offset < 0 || !ssr)
        return(-1);
    if (-1 == fcntl(fd,F_SETFL,O_APPEND))
        return(-1);
    size = sizeof(ssttrailer_t);
    do {
        lseek(fd,offset,SEEK_SET);
    } while (size != write(fd,ssr,size));
    return(0);
}





/* The SStable Handler  ------->
 * When open a HFILE, will create an instance of a HFILE_T, HFILE_T
 * maintains a pointer to point to each segment of the storage file.
 */


SST *ssTableCreate(void)
{
    SST *sst;
    if ((sst = malloc(sizeof(SST))) == NULL)
        return(NULL);
    memset(sst,0,sizeof(SST));
    return(sst);
}

void ssTableFree(SST *sst)
{
    free(sst);
}

void ssTableDestroy(SST *sst)
{
    if (!sst)
        return;
    sstBloomDestroy(sst->bloom);
    sstIndexDestroy(sst->metas);
    sstInfoDestroy(sst->fileinfo);
    sstTrailerFree(sst->trailer);
    ssTableFree(sst);
}


SST *ssTableOpen(const int8_t *sstname, int32_t flags, ...)
{
    SST *sst;
    va_list ap;
    int32_t sstfd, mode;

    if (!sstname || strlen(sstname) == 0)
        return(NULL);
    if ((sst = ssTableCreate()) == NULL)
        return(NULL);
    memcpy(sst->s_name,sstname,strlen(sstname));

    if (flags & SST_OPEN) {
        if ((sstfd = open(sst->s_name,O_RDONLY)) < 0) {
            ssTableFree(sst);
            return(NULL);
        }
        sst->trailer = sstTrailerLoadFromSStable(
         sstfd,-sizeof(ssttrailer_t),sizeof(ssttrailer_t));
        if (sst->trailer == NULL) {
            ssTableFree(sst);
            close(sstfd);
            return(NULL);
        }
        sst->fileinfo = sstInfoLoadFromSStable(
         sstfd,sst->trailer->infooffset,sst->trailer->infosize);
        if (sst->fileinfo == NULL) {
            ssTableDestroy(sst);
            close(sstfd);
            return(NULL);
        }
        sst->metas = sstIndexLoadFromSStable(
         sstfd,sst->trailer->indexoffset,sst->trailer->indexsize);
        if (sst->metas == NULL) {
            ssTableDestroy(sst);
            close(sstfd);
            return(NULL);
        }
        sst->bloom = sstBloomLoadFromSStable(
         sstfd,sst->trailer->bloomoffset,sst->trailer->bloomsize);
        if (sst->bloom == NULL) {
	        ssTableDestroy(sst);
	        close(sstfd);
	        return(NULL);
	    }
        close(sstfd);
    } else if (flags & SST_CREAT) {
        va_start(ap,flags);
        mode = va_arg(ap,int32_t);
        va_end(ap);
        if ((sstfd = open(sst->s_name,O_RDONLY|O_CREAT|O_EXCL,mode)) < 0) {
            ssTableFree(sst);
            return(NULL);
        }
        close(sstfd);
    }

    return(sst);
}

void ssTableClose(SST *sst)
{
    ssTableDestroy(sst);
}

entry_t *ssTableFind(SST *sst, sds key)
{
    int32_t sstfd;
    meta_t *curMeta;

    if (!sst || !key)
        return(NULL);
    if (-1 == sstBloomCheckKey(sst->bloom,key))
        return(NULL);
    if (sdscmp(key,(sstIndexHeader(sst->metas))->key) < 0)
        return(NULL);
    if (sdscmp(key,sst->fileinfo->lastkey) > 0)
        return(NULL);
    curMeta = sstIndexFind(sst->metas,key);
    if (curMeta->state != IN_CACHE) {
        if ((sstfd = open(sst->s_name,O_RDONLY)) < 0)
            return(NULL);
        curMeta->block =
         sstBlockLoadFromSStable(sstfd,curMeta->offset,curMeta->blocksize);
        curMeta->state = IN_CACHE;
        close(sstfd);
    }
    return(sstBlockFind(curMeta->block,key));
}


int32_t ssTableMerge(SST *sst, SST *ssa, SST *ssb)
{
    int32_t sstfd,nsstfd;
    sstblock_t *bufferBlock;
    entry_t *aMin, *bMin, *curMin;
    meta_t *Ameta, *Bmeta, *bufferMeta;

    if (!ssa || !ssb || !sst)
        return(-1);
    if (!sst->metas && (sst->metas = sstIndexCreate()) == NULL)
        return(-1);
    if (!sst->trailer && (sst->trailer = sstTrailerCreate()) == NULL)
        return(-1);
    if (!sst->fileinfo && (sst->fileinfo = sstInfoCreate()) == NULL)
        return(-1);
    if (!sst->bloom && (sst->bloom = sstBloomCreate(
     ssa->fileinfo->entrycount + ssb->fileinfo->entrycount, BLOOM_P)) == NULL)
        return(-1);

    do {
        nsstfd = open(sst->s_name,O_RDWR|O_CREAT|O_APPEND,0644);
    } while (nsstfd < 0);

    /* The Base merge algorithm
     *
     * Suppose there are two SStable A and B, being be merged into a new large 
     * SStable C, each time to remove an entry from A and B and save the entries
     * pointer to n and m, and then take the loop operation:
            1. if n == NULL, pick up a min entry from A. as well as B to m.
            2. compare key(n) and key(m) :
                2.1 if key(n) < key(m)
                    insert n into the new sstable C and then make n = NULL.
                2.2 if key(n) > key(m)
                    insert m into the new sstable C and then make m = NULL.
                2.3 if key(n) == key(m), compare time(n) and time(m)
                    2.3.1 if time(n) < time(m)
                        this shows that n was expired. n will be given up and
                        insert m into C.
                    2.3.2 if time(n) > time(m)
                        It is similar to above, give up m and insert n into C
                    2.3.3 time(n) == time(m) ?
                        Impossible, the typical logic error. 
            3. continue loop.
     */

    
    Ameta = sstIndexHeader(ssa->metas);
    Bmeta = sstIndexHeader(ssb->metas);
    aMin = bMin = NULL;

    while (Ameta || Bmeta) {
        bufferMeta = metaCreate();
        bufferBlock = sstBlockCreate();
        if (!bufferMeta || !bufferBlock) {
            if (bufferMeta)
                metaFree(bufferMeta);
            if (bufferBlock)
                sstBlockFree(bufferBlock);
            ssTableDestroy(sst);
            return(-1);
        }

        while (sstBlockSize(bufferBlock) < _BLOCK_SIZE) {
            do {
                if (!Ameta)
                    break;
                if (Ameta->state != IN_CACHE) {
                    sstfd = open(ssa->s_name,O_RDONLY);
                    Ameta->block = sstBlockLoadFromSStable(
                     sstfd, Ameta->offset, Ameta->blocksize);
                    Ameta->state = IN_CACHE;
                    close(sstfd);
                }
                if (!aMin) {
                    if ((aMin = sstBlockPickupMin(Ameta->block)) == NULL) {
                        Ameta->state = IN_DISK;
                        sstBlockDestroy(Ameta->block);
                        Ameta = metaNext(Ameta);
                    }
                }
            } while (!aMin);

            do {
                if (!Bmeta)
                    break;
                if (Bmeta->state != IN_CACHE) {
                    sstfd = open(ssb->s_name,O_RDONLY);
                    Bmeta->block = sstBlockLoadFromSStable(
                     sstfd, Bmeta->offset, Bmeta->blocksize);
                    Bmeta->state = IN_CACHE;
                    close(sstfd);
                }
                if (!bMin) {
                    if ((bMin = sstBlockPickupMin(Bmeta->block)) == NULL) {
                        Bmeta->state = IN_DISK;
                        sstBlockDestroy(Bmeta->block);
                        Bmeta = metaNext(Bmeta);
                    }
                }
            } while (!bMin);

            if (!aMin && !bMin)
                break;
            else if (aMin && bMin) {
                if (entryCompare(aMin,bMin) < 0) {
                    curMin = aMin;
                    aMin = NULL;
                } else if (entryCompare(aMin,bMin) > 0) {
                    curMin = bMin;
                    bMin = NULL;
                } else {
                    if (ssa->trailer->timestamp > ssb->trailer->timestamp) {
                        curMin = aMin;
                        entryDestroy(bMin);
                        aMin = bMin = NULL;
                    } else {
                        curMin = bMin;
                        entryDestroy(aMin);
                        aMin = bMin = NULL;
                    }
                }
            } else {
                if (!aMin) {
                    curMin = bMin;
                    bMin = NULL;
                } else {
                    curMin = aMin;
                    aMin = NULL;
                }
            }
            sstBlockInsertEntry(bufferBlock,curMin);
        }
        if (bufferBlock->_count > 0) {
            bufferMeta->key = sdsdup((sstBlockHeader(bufferBlock))->key);
            bufferMeta->blocksize = sstBlockSize(bufferBlock);
            sstIndexInsertMeta(sst->metas,bufferMeta);
            sstBloomInsertKeySets(sst->bloom,bufferBlock);
            do {
                bufferMeta->offset = lseek(nsstfd,0,SEEK_END);
            } while (bufferMeta->blocksize !=
             sstBlockDumpIntoSStable(nsstfd,bufferMeta->offset,bufferBlock));
            sst->fileinfo->blockcount++;
            sst->fileinfo->entrycount = sst->fileinfo->entrycount + bufferBlock->_count;
            /* free the sstable block cache data */
            sstBlockDestroy(bufferBlock);
        } else {
            metaDestroy(bufferMeta);
            sstBlockDestroy(bufferBlock);
        }
    }
    
    bufferMeta = sstIndexTailer(sst->metas);
    if (bufferMeta->state != IN_CACHE)
        bufferMeta->block = sstBlockLoadFromSStable(nsstfd,bufferMeta->offset,bufferMeta->blocksize);
    sst->fileinfo->lastkey = sdsdup((sstBlockTailer(bufferMeta->block))->key);
    sstBlockDestroy(bufferMeta->block);
    bufferMeta->block = NULL;
    
    sst->trailer->bloomoffset = lseek(nsstfd,0,SEEK_CUR);
    sst->trailer->bloomsize = sstBloomDumpIntoSStable(nsstfd,sst->trailer->bloomoffset,sst->bloom);
    sst->trailer->indexoffset = lseek(nsstfd,0,SEEK_CUR);
    sst->trailer->indexsize = sstIndexDumpIntoSStable(nsstfd,sst->trailer->indexoffset,sst->metas);
    sst->trailer->infooffset = lseek(nsstfd,0,SEEK_CUR);
    sst->trailer->infosize = sstInfoDumpIntoSStable(nsstfd,sst->trailer->infooffset,sst->fileinfo);
    sstTrailerDumpIntoSStable(nsstfd,lseek(nsstfd,0,SEEK_CUR),sst->trailer);

    close(nsstfd);
    close(sstfd);
    return(0);
}

int32_t ssTableSplit(SST *sst, SST *ssa, SST *ssb)
{
    meta_t *curMeta;
    int32_t i,sstfd,ssafd,ssbfd;

    if (!sst || !ssa || !ssb)
        return(-1);
    if (!ssa->metas)
        ssa->metas = sstIndexCreate();
    if (!ssa->trailer)
        ssa->trailer = sstTrailerCreate();
    if (!ssa->fileinfo)
        ssa->fileinfo = sstInfoCreate();
    if (!ssa->bloom)
        ssa->bloom = sstBloomCreate(sst->fileinfo->entrycount,BLOOM_P);
    if (!ssa->metas || !ssa->trailer || !ssa->fileinfo || !ssa->bloom) {
        sstIndexFree(ssa->metas);
        sstTrailerFree(ssa->trailer);
        sstInfoFree(ssa->fileinfo);
        sstBloomDestroy(ssa->bloom);
        return(-1);
    }

    if (!ssb->metas)
        ssb->metas = sstIndexCreate();
    if (!ssb->trailer)
        ssb->trailer = sstTrailerCreate();
    if (!ssb->fileinfo)
        ssb->fileinfo = sstInfoCreate();
    if (!ssb->bloom)
        ssb->bloom = sstBloomCreate(sst->fileinfo->entrycount,BLOOM_P);
    if (!ssb->metas || !ssb->trailer || !ssb->fileinfo || !ssb->bloom) {
        sstIndexFree(ssb->metas);
        sstTrailerFree(ssb->trailer);
        sstInfoFree(ssb->fileinfo);
        sstBloomDestroy(ssb->bloom);
        return(-1);
    }

    /* SStable file split operaton, do not involve data processing, because
     * the sequence of key/value pairs in the sstable are stored in sorted
     * order and partitioned into a sequence of data blocks. It just similar
     * to cut a watermelon in half.
     */


    sstfd = open(sst->s_name,O_RDONLY);

    ssafd = open(ssa->s_name,O_RDWR|O_CREAT,0644);
    for (i = 0; i < (sst->fileinfo->blockcount) >> 1; i++) {
        curMeta = sstIndexHeader(sst->metas);
        sstIndexDeleteMeta(sst->metas,curMeta);        
        sendfile(sstfd,
         curMeta->offset,ssafd,lseek(ssafd,0,SEEK_CUR),curMeta->blocksize);
        curMeta->offset = lseek(ssafd,0,SEEK_CUR) - curMeta->blocksize;
        sstIndexInsertMeta(ssa->metas,curMeta);
    }
    for (curMeta = sstIndexHeader(ssa->metas);
     curMeta != NULL; curMeta = metaNext(curMeta)) {
        if (curMeta->state != IN_CACHE)
            curMeta->block =
             sstBlockLoadFromSStable(ssafd,curMeta->offset,curMeta->blocksize);
        sstBloomInsertKeySets(ssa->bloom,curMeta->block);
        ssa->fileinfo->blockcount++;
        ssa->fileinfo->entrycount =
         ssa->fileinfo->entrycount + curMeta->block->_count;
        if (NULL == metaNext(curMeta))
            ssa->fileinfo->lastkey =
             sdsdup((sstBlockTailer(curMeta->block))->key);
        sstBlockDestroy(curMeta->block);
        curMeta->state = IN_DISK;
        curMeta->block = NULL;
    }
    ssa->trailer->indexoffset = lseek(ssafd,0,SEEK_CUR);
    ssa->trailer->indexsize =
     sstIndexDumpIntoSStable(ssafd,ssa->trailer->indexoffset,ssa->metas);
    ssa->trailer->bloomoffset = lseek(ssafd,0,SEEK_CUR);
    ssa->trailer->bloomsize =
     sstBloomDumpIntoSStable(ssafd,ssa->trailer->bloomoffset,ssa->bloom);
    ssa->trailer->infooffset = lseek(ssafd,0,SEEK_CUR);
    ssa->trailer->infosize =
     sstInfoDumpIntoSStable(ssafd,ssa->trailer->infooffset,ssa->fileinfo);
    sstTrailerDumpIntoSStable(ssafd,lseek(ssafd,0,SEEK_CUR),ssa->trailer);
    close(ssafd);
    
    ssbfd = open(ssb->s_name,O_RDWR|O_CREAT,0644);
    for ( ; i < sst->fileinfo->blockcount; i++) {
        curMeta = sstIndexHeader(sst->metas);
        sstIndexDeleteMeta(sst->metas,curMeta);
        sendfile(sstfd,
         curMeta->offset,ssbfd,lseek(ssbfd,0,SEEK_CUR),curMeta->blocksize);
        curMeta->offset = lseek(ssbfd,0,SEEK_CUR) - curMeta->blocksize;
        sstIndexInsertMeta(ssb->metas,curMeta);
    }
    for (curMeta = sstIndexHeader(ssb->metas);
     curMeta != NULL; curMeta = metaNext(curMeta)) {
        if (curMeta->state != IN_CACHE)
            curMeta->block =
             sstBlockLoadFromSStable(ssbfd,curMeta->offset,curMeta->blocksize);
        sstBloomInsertKeySets(ssb->bloom,curMeta->block);
        ssb->fileinfo->blockcount++;
        ssb->fileinfo->entrycount =
         ssb->fileinfo->entrycount + curMeta->block->_count;
        if (NULL == metaNext(curMeta))
            ssb->fileinfo->lastkey =
             sdsdup((sstBlockTailer(curMeta->block))->key);
        sstBlockDestroy(curMeta->block);
        curMeta->state = IN_DISK;
        curMeta->block = NULL;
    }
    ssb->trailer->indexoffset = lseek(ssbfd,0,SEEK_CUR);
    ssb->trailer->indexsize =
     sstIndexDumpIntoSStable(ssbfd,ssb->trailer->indexoffset,ssb->metas);
    ssb->trailer->bloomoffset = lseek(ssbfd,0,SEEK_CUR);
    ssb->trailer->bloomsize =
     sstBloomDumpIntoSStable(ssbfd,ssb->trailer->bloomoffset,ssb->bloom);
    ssb->trailer->infooffset = lseek(ssbfd,0,SEEK_CUR);
    ssb->trailer->infosize =
     sstInfoDumpIntoSStable(ssbfd,ssb->trailer->infooffset,ssb->fileinfo);
    sstTrailerDumpIntoSStable(ssbfd,lseek(ssbfd,0,SEEK_CUR),ssb->trailer);
    close(ssbfd);
    return(0);
}




