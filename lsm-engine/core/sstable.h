/* sst.h - A persistant storage file implementation
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


#ifndef __SSTABLE_H_
#define __SSTABLE_H_

#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "utils.h"

#define LEVEL_MAX 15
#define SKIPLIST_P 0.25


/* The Data Block segment ------->
 * To store key/value pairs, may be compressed. each block is the
 * unit of transfer to and from persistent storage. The default
 * block size is approximately 1mb uncompressed bytes. Applications
 * that mostly do bulk scans over the contents of the database may
 * wish to increase this size. Applications that do a lot of point
 * reads of small values may wish to switch to a smaller block size
 * if performance measurements indicate an improvement. There isn't
 * much benefit in using blocks smaller than one kilobyte, or larger
 * than a few megabytes. Also note that compression will be more
 * effective with larger block sizes.
 */



typedef struct entry_s entry_t;
typedef struct block_s sstblock_t;

struct entry_s {
	sds key;
	sds value;
	int32_t level;
	entry_t *forward[];
} ;

struct block_s {
	entry_t *_head;
	int32_t _count;
	int32_t maxlevel;
	int32_t (*compareEntry)(entry_t *, entry_t *);
} ;

entry_t *entryCreate(void);
entry_t *entryCreateByLevel(int32_t level);
void entryFree(entry_t *node);
void entryDestroy(entry_t *node);
entry_t *entryNext(entry_t *node);
int32_t entrySize(entry_t *node);
int32_t entryValid(entry_t *node);
int32_t entryCompare(entry_t *aNode, entry_t *bNode);

sstblock_t *sstBlockCreate(void);
void sstBlockFree(sstblock_t *list);
void sstBlockDestroy(sstblock_t *list);

int32_t sstBlockSize(sstblock_t *list);
entry_t *sstBlockHeader(sstblock_t *list);
entry_t *sstBlockTailer(sstblock_t *list);
entry_t *sstBlockPickupMin(sstblock_t *list);
entry_t *sstBlockPickupMax(sstblock_t *list);
entry_t *sstBlockFind(sstblock_t *list, sds key);
int32_t sstBlockInsertEntry(sstblock_t *list, entry_t *node);
int32_t sstBlockDeleteEntry(sstblock_t *list, entry_t *node);



sstblock_t *sstBlockLoadFromSStable(int32_t fd, int32_t offset, int32_t size);
int32_t sstBlockDumpIntoSStable(int32_t fd, int32_t offset, sstblock_t *list);





/* The SStable Index segment ------->
 * Indexes the data block offset in the SStable. The key of each meta
 * is the key of first key/value pair in the block. as you see, index
 * is the list of meta node.
 */


typedef struct meta_s meta_t;
typedef struct sstindex_s sstindex_t;

struct meta_s {
	sds key;
	int32_t state;		/* IN_CACHE or IN_DISK */
#define IN_DISK 	0
#define IN_CACHE	1
	sstblock_t *block;
	int32_t offset;
	int32_t blocksize;
	int32_t level;
	meta_t *forward[];
} ;

struct sstindex_s {
	meta_t *_head;
	int32_t maxlevel;
	int (*compareMeta)(meta_t *, meta_t *);
} ;

meta_t *metaCreate(void);
meta_t *metaCreateByLevel(int32_t level);
void metaFree(meta_t *node);
void metaDestroy(meta_t *node);
meta_t *metaNext(meta_t *node);
int32_t metaCompare(meta_t *aNode, meta_t *bNode);

sstindex_t *sstIndexCreate(void);
void sstIndexFree(sstindex_t *list);
void sstIndexDestroy(sstindex_t *list);
meta_t *sstIndexHeader(sstindex_t *list);
meta_t *sstIndexTailer(sstindex_t *list);
meta_t *sstIndexFind(sstindex_t *list, sds key);
int32_t sstIndexInsertMeta(sstindex_t *list, meta_t *node);
int32_t sstIndexDeleteMeta(sstindex_t *list, meta_t *node);

sstindex_t *sstIndexLoadFromSStable(int32_t fd, int32_t offset, int32_t size);
int32_t sstIndexDumpIntoSStable(int32_t fd, int32_t offset, sstindex_t *list);







/* The Bloom-filter imprementation  ------->
 * Bloom-filter is a space-efficient probabilistic data structure
 * that is used to test whether an element is a member of a set.
 */

#define BLOOM_P 0.01

typedef struct {
	int32_t size;
	int32_t mbits;
	int32_t hashn;
	uint8_t *s;
	uint32_t (*hash)(const int8_t  *, int32_t );
} sstbloom_t;

sstbloom_t *sstBloomCreate(uint32_t n, float p);
void sstBloomDestroy(sstbloom_t *bloom);
int32_t sstBloomInsertKey(sstbloom_t *bloom, const sds key);
int32_t sstBloomCheckKey(sstbloom_t *bloom, const sds key);
int32_t sstBloomInsertKeySets(sstbloom_t *bloom, sstblock_t *list);

sstbloom_t *sstBloomLoadFromSStable(int32_t fd, int32_t offset, int32_t size);
int32_t sstBloomDumpIntoSStable(int32_t fd, int32_t offset, sstbloom_t *bloom);








/* The SStable info segment  ------->
 * It is a small metadata of the storage File, without compression.
 * developer can add user defined small metadata (name/value) here.
 * and rebuild the project.
 */


typedef struct sstinfo {
    int32_t blockcount;		/* Data block count */
    int32_t entrycount;		/* Entry count */
    sds lastkey;			/* SStable last key */
} sstinfo_t;

sstinfo_t *sstInfoCreate(void);
void sstInfoFree(sstinfo_t *ssi);
void sstInfoDestroy(sstinfo_t *ssi);

sstinfo_t *sstInfoLoadFromSStable(int32_t fd, int32_t offset, int32_t size);
int32_t sstInfoDumpIntoSStable(int32_t fd, int32_t offset, sstinfo_t *ssi);







/* The Trailer segment  ------->
 * The fix sized metadata. To hold the offset and size of each segment,
 * etc. To read an SStable, we should always read the Trailer firstly.
 */

typedef struct ssttrailer {
    int32_t version;        /* SStable Version */
#define CUR_VERSION 0.9
    int32_t timestamp;      /* SStable created time */
    int32_t infooffset;     /* SStable info Segment offset */
    int32_t infosize;       /* SStable info Segment size */
    int32_t indexoffset;    /* SStable Index block offset */
    int32_t indexsize;      /* SStable Index block size */
    int32_t bloomoffset;    /* SStable Bloom filter offset */
    int32_t bloomsize;      /* SStable Bloom filter size */
} ssttrailer_t;

ssttrailer_t *sstTrailerCreate();
void sstTrailerFree(ssttrailer_t *trailer);
ssttrailer_t *sstTrailerLoadFromSStable(int32_t fd, int32_t offset, int32_t size);
int32_t sstTrailerDumpIntoSStable(int32_t fd, int32_t offset, ssttrailer_t *ssr);






/* The SStable Handler  ------->
 * When open a SStable, will create an instance of a SST, SST
 * maintains a pointer to point to each segment of the storage file.
 */

typedef struct SStable {
    ssttrailer_t *trailer;      /* SStable trailer */
    sstinfo_t *fileinfo;        /* SStable info */
    sstindex_t *metas;	        /* SStable meta lists */
    sstbloom_t *bloom;          /* SStable bloom data */
    int8_t s_name[PATH_MAX];    /* SStable name */
} SST;


SST *ssTableCreate(void);
void ssTableFree(SST *sst);
void ssTableDestroy(SST *sst);

SST *ssTableOpen(const int8_t *sstname);
void ssTableClose(SST *sst);
entry_t *ssTableFind(SST *sst, sds key);

int32_t ssTableMerge(SST *sst, SST *ssa, SST *ssb);
int32_t ssTableSplit(SST *sst, SST *ssa, SST *ssb);



#endif
