/* lirs.c - A lirs buffer cache algorithm implementation
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
#include "lirs.h"

lirs_elem_t *lirs_elem_new(char *key)
{
    lirs_elem_t *e;

    if (!key)
        return(NULL);
    e = calloc(1,sizeof(lirs_elem_t));
    if (e)
        e->key = strdup(key);
    return(e);
}

void lirs_elem_free(lirs_elem_t *e)
{
    if (e)
        free(e->key);
    free(e);
}

lirs_list_t *lirs_list_new(size_t cap, int type)
{
    lirs_list_t *l;

    if (cap == 0 || !(l = calloc(1,sizeof(lirs_list_t))))
        return(NULL);
    l->cap = cap;
    l->type = type;
    return(l);
}

void lirs_list_free(lirs_list_t *l)
{
    lirs_elem_t *e;

    while (l->head) {
        e = l->head;
        l->head = e->le_next;
        lirs_elem_free(e);
    }
    free(l);
}

int lirs_list_push(lirs_list_t *l, lirs_elem_t *e)
{
    if (!l || !e)
        return(-1);
    e->le_next = l->head;
    if (e->le_next)
        e->le_next->le_prev = e;
    l->head = e;
    if (!l->tail)
        l->tail = e;
    if (e->flags & l->type)
        l->size++;
    return(0);
}

lirs_elem_t *lirs_list_pop(lirs_list_t *l)
{
    lirs_elem_t *e;

    if (!l || !l->tail)
        return(NULL);
    e = l->tail;
    if (e) {
        l->tail = e->le_prev;
        if (l->tail)
            l->tail->le_next = NULL;
        else
            l->head = NULL;
        if (e->flags & l->type)
            l->size--;
    }
    return(e);
}

int lirs_list_del(lirs_list_t *l, lirs_elem_t *e)
{
    if (!l || !e)
        return(-1);
    if (e->le_prev)
        e->le_prev->le_next = e->le_next;
    else
        l->head = e->le_next;

    if (e->le_next)
        e->le_next->le_prev = e->le_prev;
    else
        l->tail = e->le_prev;
    if (e->flags & l->type)
        l->size--;
    return(0);
}

lirs_elem_t *lirs_list_get(lirs_list_t *l, char *key)
{
    lirs_elem_t *e;
    
    if (!l || !key)
        return(NULL);
    e = l->head;
    while (e) {
        if (strcmp(e->key,key) == 0)
            break;
        e = e->le_next;
    }
    return(e);
}
