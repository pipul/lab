/* hfile.h - A persistant storage file implementation
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


#ifndef __HFILE_H_
#define __HFILE_H_

#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "utils.h"

#define LEVEL_MAX 15
#define SKIPLIST_P 0.25

typedef struct HFILE hfile_t;



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



typedef struct datalist_node entry_t;
typedef struct datalist_list block_t;

struct datalist_node {
	sds key;
	sds value;
	int32_t level;
	entry_t *forward[];
} ;

struct datalist_list {
	entry_t *_head;
	int32_t maxlevel;
	int32_t (*cmp)(entry_t *, entry_t *);
	uint64_t magic;
} ;


entry_t *entry_new();
void entry_free(entry_t *e);
void entry_destroy(entry_t *e);
entry_t *entry_next(entry_t *e);
int32_t entry_size(entry_t *e);
int32_t entry_valid(entry_t *e);

block_t *block_new();
void block_free(block_t *l);
void block_destroy(block_t *l);
entry_t *block_head(block_t *l);
entry_t *block_tail(block_t *l);
int32_t block_size(block_t *b);
int32_t block_add(block_t *l, entry_t *e);
int32_t block_del(block_t *l, entry_t *e);

block_t *block_load(int32_t fd, int32_t offset, int32_t size);
int32_t block_swap(int32_t fd, int32_t offset, block_t *l);
entry_t *block_loup(block_t *l, sds key);





/* The Data Block Index segment ------->
 * Indexes the data block offset in the HFile. The key of each meta
 * is the key of first key/value pair in the block. as you see, index
 * is the list of meta node.
 */


typedef struct indexlist_node meta_t;
typedef struct indexlist_list index_t;

struct indexlist_node {
	sds key;
	int32_t id;
	int32_t state;		/* IN_CACHE or IN_DISK */
#define IN_DISK 	0
#define IN_CACHE	1
	int32_t offset;
	int32_t blocksize;
	int32_t level;
	meta_t *forward[];
} ;

struct indexlist_list {
	meta_t *_head;
	int32_t maxlevel;
	int (*cmp)(meta_t *, meta_t *);
	uint64_t magic;
} ;

meta_t *meta_new();
void meta_free(meta_t *m);
void meta_destroy(meta_t *m);
meta_t *meta_next(meta_t *m);


index_t *index_new();
void index_free(index_t *l);
void index_destroy(index_t *l);
meta_t *index_head(index_t *l);
meta_t *index_tail(index_t *l);
int32_t index_add(index_t *l, meta_t *m);
int32_t index_del(index_t *l, meta_t *m);
meta_t *index_loup(index_t *l, sds key);

index_t *index_load(int32_t fd, int32_t offset, int32_t size);
int32_t index_swap(int32_t fd, int32_t offset, index_t *l);







/* The Bloom-filter imprementation  ------->
 * Bloom-filter is a space-efficient probabilistic data structure
 * that is used to test whether an element is a member of a set.
 */

#define BLOOM_P 0.01

typedef struct {
	int32_t len;
	int32_t mbits;
	int32_t hashn;
	uint8_t *s;
	uint32_t (*hash)(const int8_t  *, int32_t );
} bloom_t;

bloom_t *bloom_new(uint32_t n, float p);
void bloom_destroy(bloom_t *bloom);
int32_t bloom_add(bloom_t *bloom, const sds key);
int32_t bloom_check(bloom_t *bloom, const sds key);
bloom_t *bloom_load(int32_t fd, int32_t offset, int32_t size);
int32_t bloom_swap(int32_t fd, int32_t offset, bloom_t *bloom);








/* The File info segment  ------->
 * It is a small metadata of the storage File, without compression.
 * developer can add user defined small metadata (name/value) here.
 * and rebuild the project.
 */


typedef struct hfileinfo {
	int64_t magic;
	int32_t blockcount;		/* Data block count */
	int32_t entrycount;		/* Entry count */
	sds lastkey;			/* Hfile last key */
} hinfo_t;

hinfo_t *hinfo_new();
hinfo_t *hinfo_load(int32_t fd, int32_t offset, int32_t size);
int32_t hinfo_swap(int32_t fd, int32_t offset, hinfo_t *fileinfo);
void hinfo_free(hinfo_t *fileinfo);
void hinfo_destroy(hinfo_t *fileinfo);







/* The Trailer segment  ------->
 * The fix sized metadata. To hold the offset and size of each segment,
 * etc. To read an HFile, we should always read the Trailer firstly.
 */

typedef struct hfiletrailer {
	int32_t version;		/* Version */
#define CUR_VERSION 0.9
	int32_t timestamp;		/* Hfile created time */
	int32_t infooffset;		/* Hfileinfo offset */
	int32_t infosize;		/* Hfileinfo size */
	int32_t indexoffset;		/* Index block offset */
	int32_t indexsize;		/* Index block size */
	int32_t bloomoffset;		/* Bloom filter offset */
	int32_t bloomsize;		/* Bloom filter size */
} htail_t;

htail_t *htail_new();
htail_t *htail_load(int32_t fd, int32_t offset, int32_t size);
int32_t htail_swap(int32_t fd, int32_t offset, htail_t *trailer);
void htail_free(htail_t *trailer);






/* The HFILE Handler  ------->
 * When open a HFILE, will create an instance of a HFILE_T, HFILE_T
 * maintains a pointer to point to each segment of the storage file.
 */

struct HFILE {
	int8_t name[PATH_MAX];		/* hfile name */
	htail_t *trailer;		/* hfile trailer */
	hinfo_t *fileinfo;		/* hfile info */
	index_t *metas;			/* hfile meta lists */
	bloom_t *bloom;			/* hfile bloom data */
	block_t **blocks;		/* hfile blocks array */
	hfile_t *next;			/* next hfile */
	hfile_t *prev;			/* prev hfile */
} ;


hfile_t *hfile_new();
hfile_t *hfile_load(const int8_t *filename);
int32_t hfile_swap(hfile_t *fp);
void hfile_free(hfile_t *fp);
void hfile_destroy(hfile_t *fp);
hfile_t *hfile_next(hfile_t *fp);
hfile_t *hfile_prev(hfile_t *fp);

int32_t hfile_version(hfile_t *fp);
int32_t hfile_timestamp(hfile_t *fp);
entry_t *hfile_loup(hfile_t *fp, sds key);





/* The HFILE list  ------->
 * Startup, the system will first retrieve the database directory,
 * and then initialize all the data files into hfile_t instance, add
 * to hfilelist.
 */



typedef struct hfilelist {
	hfile_t *first;
	hfile_t *last;
	int32_t count;
	int32_t (*cmp)(hfile_t *s, hfile_t *h);
} hfilelist_t;

hfilelist_t *hfilelist_new();
void hfilelist_free(hfilelist_t *hl);
void hfilelist_destroy(hfilelist_t *hl);
int32_t hfilelist_add(hfilelist_t *hl, hfile_t *fp);
int32_t hfilelist_del(hfilelist_t *hl, hfile_t *fp);
hfile_t *hfilelist_head(hfilelist_t *hl);
hfile_t *hfilelist_tail(hfilelist_t *hl);



#endif
