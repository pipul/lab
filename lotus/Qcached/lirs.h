/* lirs.h - A lirs buffer cache algorithm implementation
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



#ifndef __LIRS_H_
#define __LIRS_H_

#include <sys/types.h>

typedef struct lirs_elem {
    char *key;
    char *value;
    int flags;
#define TYPE_LIR 0x01
#define TYPE_HIR 0x02
    struct lirs_elem *le_next, *le_prev;
} lirs_elem_t;

typedef struct {
    int type;
    lirs_elem_t *head, *tail;
    size_t cap, size;
} lirs_list_t;

lirs_elem_t *lirs_elem_new(char *key);
void lirs_elem_free(lirs_elem_t *e);

lirs_list_t *lirs_list_new(size_t cap, int type);
void lirs_list_free(lirs_list_t *l);

int lirs_list_push(lirs_list_t *l,lirs_elem_t *e);
lirs_elem_t *lirs_list_pop(lirs_list_t *l);
int lirs_list_del(lirs_list_t *l,lirs_elem_t *e);
lirs_elem_t *lirs_list_get(lirs_list_t *l, char *key);






#endif
