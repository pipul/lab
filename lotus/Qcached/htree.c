/* htree.c - A hash tree implementation
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
#include "htree.h"

static size_t _djb_hash(const char *key)
{
    size_t c, hash_v = 5381;

    if (!key)
        return(0);
    while ((c = *key++))
        hash_v = ((hash_v << 5) + hash_v) + (size_t)c;
    return(hash_v);
}

static int find_slot(size_t cap, char *key)
{
    return(_djb_hash(key) % cap);
}

hash_table_t *hash_table_new(uint32_t cap)
{
    int i;
    hash_table_t *ht;

    if (cap < 0)
        return(NULL);
    ht = calloc(1,sizeof(hash_table_t) + cap*sizeof(hash_slot_t));
    if (ht)
        ht->cap = cap;
    for (i = cap; i > 0; i--) {
        ht->slots[i - 1].list = sl_list_new(cap * HASH_COLLISION_P);
        ht->slots[i - 1].cap = cap * HASH_COLLISION_P;
    }
    return(ht);
}

int hash_table_add(hash_table_t *ht, char *key, void *value)
{
    size_t off;
    sl_elem_t *e;
    hash_slot_t *hs;

    if (!ht || !key)
        return(-1);
    off = find_slot(ht->cap,key);
    if (!(e = sl_elem_new(key,value)))
        return(-1);
    sl_list_add(ht->slots[off].list,e);
    return(0);
}

void *hash_table_get(hash_table_t *ht, char *key)
{
    size_t off;
    sl_elem_t *e;
    hash_slot_t *hs;

    if (!ht || !key)
        return(NULL);
    off = find_slot(ht->cap,key);
    e = sl_list_get(ht->slots[off].list,key);
    if (e)
        return(e->value);
    return(NULL);
}

int hash_table_del(hash_table_t *ht, char *key)
{
    size_t off;
    sl_elem_t *e;
    hash_slot_t *hs;

    if (!ht || !key)
        return(-1);
    off = find_slot(ht->cap,key);
    e = sl_list_get(ht->slots[off].list,key);
    if (e) {
        sl_list_del(ht->slots[off].list,e);
        sl_elem_free(e);
    }
    return(0);
}

void hash_table_free(hash_table_t *ht)
{
    if (!ht)
        return;
    while (ht->cap) {
        sl_list_free(ht->slots[ht->cap - 1].list);
        ht->cap--;
    }
    free(ht);
}


int hash_stats(hash_table_t *ht)
{
    int i;

    if (!ht)
        return(-1);
    for (i = ht->cap; i > 0; i--)
        sl_stats(ht->slots[i - 1].list);
    return(0);
}
