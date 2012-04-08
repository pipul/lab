/* lsm.h - LSM core algorithm implementation
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

#ifndef __LSM_H_
#define __LSM_H_

#include <stdarg.h>
#include <stdint.h>





#define _PATH_MAX       1024
#define _KEY_MAX        1024
#define _LEVEL_MAX      15
#define _SKIPLIST_P     0.25
#define _BLOCK_SIZE     40960





/* KET/VALUE structure type  ------->
 * File: utils.c utils.h
 * SDSHDR uses the similar technique to pointer of malloc/free.
 * the "len" field describes the length of sds object and "data"
 * field store the really object data
 */

typedef int8_t * sds;

typedef struct {
	int32_t len;
	int8_t  data[];
} sdshdr;

/* The sdsnew() creates a new sds object and copies the string pointed
 * to by src, including the terminating null type ('\0'), to the buffer
 * pointed to by data[] field. */
sds sdsnew(int8_t  *src);

/* The sdsnnew() function is similar, except that at most len bytes of
 * src are copied. Warning: If the len large than _KEY_MAX, it just
 * only copy the _KEY_MAX bytes to sdshdr buffer(.data[]) */
sds sdsnnew(int8_t *src, int32_t len);

/* The sdsdel() function will destroy a sds object and free its resource */
void sdsdel(sds s);

/* sdsdup() create a copy of the sds object s */
sds sdsdup(sds s);

/* The sdslen() function returns the number of bytes in s */
int32_t sdslen(sds s);

/* The sdscmp() function compares two sds object s and h, It returns an
 * integer less then, equal to, or greater than zero if s is found,
 * respectively, to be less than, to match, or be greater than h.  */
int32_t sdscmp(sds s, sds h);





/* ENTRY structure type  ------->
 * File: sstable.c sstable.h
 * Each entry is either a value for the key, or a deletion marker for the key.
 * (Deletion markers are kept around to hide obsolete values present in older
 * sorted sstables).
 */
 

typedef struct entry {
    sds key;
    sds value;

/* The level field used for a skiplist can contain more than one pointer since
 * they can participate in more than one list. forward[level] field is an array
 * of pointer which point to the next entry. */ 
    int32_t level;
    struct entry *forward[];
} entry_t;

/* entryCreate() will create a new entry_t object. */
entry_t *entryCreate(void);

/* entryDestroy() frees the memory space of object node, which mush have been
 * created by entryCreate() */
void entryDestroy(entry_t *node);





/* The SStable Handler  ------->
 * File: sstable.c sstable.h
 * A SStable stores a sequence of entries sorted by key. Each entry is
 * either a value for the key, or a deletion marker for the key. for more
 * details about the SStable storage file format, please look the doc/*
 * documents .
 */

struct ssttrailer;
struct sstinfo;
struct sstbloom;
struct sstindex;

typedef struct SStable {
    struct ssttrailer *trailer;         /* SStable trailer */
    struct sstinfo *fileinfo;           /* SStable info */
    struct sstindex *metas;	            /* SStable meta lists */
    struct sstbloom *bloom;             /* SStable bloom data */
    int8_t s_name[_PATH_MAX];           /* SStable name */
} SST;


/* Given a sstname for a sstable file, ssTableOpen() open a sstable file or
 * create a new one and returns a SST handle. the argument flags must include
 * one of the following access modes: SST_OPEN, SST_CREAT. These request
 * opening the sstable file or create a new sstable, respectively.
 
 * mode specifies the permisstions to use in case a new sstable is created.
 * This argument must be supplied when SST_CREAT is specified in flags. 
 */
SST *ssTableOpen(const int8_t *sstname, int32_t flags, ... /* mode */);
#define SST_OPEN    0x01
#define SST_CREAT   0x02

/* The ssTableClose() function will frees all the memory space handled
 * by SST object and close the sst handle. */
void ssTableClose(SST *sst);


/* The ssTableFind() function finds the key in a SStable file, if not found
 * it will return NULL Otherwise return a pointer to entry which contain the
 * same key */
entry_t *ssTableFind(SST *sst, sds key);


/* The ssTableMerge() function merge two SStable file into one and delete the
 * obsolete entries Warning: you mush provide the two new SStable file name
 * in ssa->s_name and ssb->s_name. sst are in/out parameters.
 * 
 * in: just an empty object of SST and only contain its filename.
 *      1. sst = ssTableOpen(sstname,SST_CREAT,0644);
 *
 * out: a new SST object contains all the contents of ssa and ssb. when return
 * , you shouldn't continue to use the ssa/ssb handle, recommend you to delete
 * the Old SStable file(ssa/ssb) manual.
 *      2.  ssa = ssTableOpen(sstname,SST_OPEN);
            ssb = ssTableOpen(sstname,SST_OPEN);
 */
int32_t ssTableMerge(SST *sst, SST *ssa, SST *ssb);


/* The ssTableSplit() function split the whole SStable file into two smaller
 * SStable file. Warning: similar to above, but the ssa and ssb is in/out
 * parameters and you also should provide filename of the two small sstable
 */
int32_t ssTableSplit(SST *sst, SST *ssa, SST *ssb);








/* The Memtable Handler  ------->
 * File: memtable.c memtable.h
 * Memtable, a skiplist structure in memory, maintains all the entries which
 * may be aof(log persistent) or not. Once the Memtable size hits the Threshold
 * , it flushes all the entries to an on disc SSTable which can no longer be
 * modified only merged during compaction.
 */


typedef struct memtable {
    int32_t items;
    entry_t *_head;
    int32_t maxlevel;
    int32_t aof_fd;
    int32_t aof_flag;
#define AOF_ON      0x01
#define AOF_OFF     0x02
#define AOF_SYNC    0x04
    struct {
        int32_t len;
        int32_t free;
        int8_t *data;
    } aof_buf;
    int32_t timestamp;      /* Records the last transaction perform time */
#define AOF_BUFFERSIZE 4096
    int32_t (*compareEntry)(entry_t *, entry_t *);
} MET;


/* The memTableCreate() creates a new MET object, the argument flags must
 * inlucde one of the following aof modes:
        AOF_ON:  turn on AppendOnlyFile function
        AOF_OFF:  turn off AppendOnlyFile function
 * In addition, zero or more memtable createion flags can be bitwise-or'd
 * in flags.
        AOF_SYNC: this flag mush be specified in conjuction with AOF_ON.
        Open AOF_SYNC flag does not necessarily needed. applications that
        access log file often update(insert/delete) a entry and then call
        fsync() immediately in order to ensure that the update-log is
        physically stored on the harddisk.
 * Warning: if AOF_SYNC isn't specified and AOF_ON was turn on. MemTable
 * will cache the aof log and fflush the log data to aofile when it hits
 * a threshold. 
 *
 * aof_fd : This argument must be supplied when AOF_ON is specified.
 */
MET *memTableCreate(int32_t flag, ... /* int32_t aof_fd */);


/* The memTableDestroy() function frees all the memory space handled by
 * MET object and close the MET handle. */
void memTableDestroy(MET *list);


/* The memTableControl() set the MET status flags to the value specified
 * by flag. similar to the memTableCreate() function.*/
int32_t memTableControl(MET *list, int32_t flag);


/* The memTableFind() function finds the key in a SStable file, if not found
 * it will return NULL Otherwise return a pointer to entry which contain the
 * same key */
entry_t *memTableFind(MET *list, sds key);


/* The memTableInsertEntry() and memTableDeleteEntry() functions insert/delete
 * entry into/from memtable. if AOF_ON is turn on, Append only file is an
 * alternative durability option. */
int32_t memTableInsertEntry(MET *list, entry_t *node);
int32_t memTableDeleteEntry(MET *list, entry_t *node);


/* When memtable size hits the Threshold, memTableDumpToSStable() will flushes
 * all the entries to an on disk SSTable.
 * Warning: sst is an in/out parameters.
 * an new sstable which must be created by ssTableOpen(sstname,SST_CREAT,0...)
 */
int32_t memTableDumpToSStable(MET *list, SST *sst);


/* aofileLoadToMemtable() function opens the aof file and load all the aof
 * file contents to memory, return a MET handle for continue inserting operate
 * log. insert or delete.
 * Warning: list also is an in/out parameters.
 * an empty MET object which must be created by memTableCreate()
 */

int32_t aofileLoadToMemtable(MET *list, int32_t aof_fd);












#endif
