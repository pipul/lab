/* skiplist.h - A skiplist data structure implementation
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



#ifndef __SKIPLIST_H_
#define __SKIPLIST_H_

#include <stdint.h>
#define LEVEL_MAX 15

typedef struct __node {
	char *key;
	void *value;
	int level;
	struct __node *forward[];
} sl_elem_t;

typedef struct __list {
	sl_elem_t *_head;
	int maxlevel,size;
	int (*cmp)(sl_elem_t *, sl_elem_t *);
} sl_list_t;


sl_elem_t *sl_elem_new(char *key, void *value);
void sl_elem_free(sl_elem_t *e);

sl_list_t *sl_list_new();
void sl_list_free(sl_list_t *l);
int sl_list_add(sl_list_t *l, sl_elem_t *e);
int sl_list_del(sl_list_t *l, sl_elem_t *e);
sl_elem_t *sl_list_get(sl_list_t *l, char *key);

int sl_stats(sl_list_t *l);





#endif
