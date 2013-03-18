/* memtable.h - A memory table implementation
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


#ifndef __HABLE_H_
#define __HABLE_H_


#include "lsm.h"
#include "sstable.h"


#ifndef __LSM_H_

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
    #define AOF_BUFFERSIZE 4096
        int32_t timestamp;
        int32_t (*compareEntry)(entry_t *, entry_t *);
    } MET;

    MET *memTableCreate(int32_t flag, ...);
    void memTableDestroy(MET *list);
    int32_t memTableControl(MET *list, int32_t flag);

    entry_t *memTableHeader(MET *list);
    entry_t *memTableTailer(MET *list);
    entry_t *memTableFind(MET *list, sds key);

    int32_t memTableInsertEntry(MET *list, entry_t *node);
    int32_t memTableDeleteEntry(MET *list, entry_t *node);


    int32_t memTableDumpToSStable(MET *list, SST *sst);
    int32_t aofileLoadToMemtable(MET *list, int32_t aof_fd);

#endif

#ifdef __LSM_H_

    entry_t *memTableHeader(MET *list);
    entry_t *memTableTailer(MET *list);
    
#endif








#endif
