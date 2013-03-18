/* skiplist.c - A skiplist data structure implementation
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

#include "skiplist.h"

#define SKIPLIST_P 0.25

static int _random_level()
{
	int lvl = 1;
	while ((random()&0xFFFF) < (SKIPLIST_P * 0xFFFF))
		lvl += 1;
	return (lvl < LEVEL_MAX) ? lvl : LEVEL_MAX;
}

static int _elem_compare(sl_elem_t *s, sl_elem_t *d)
{
	if (!s || !d) {
		if (s)
			return(1);
		if (d)
			return(-1);
	}
	return(strcmp(s->key,d->key));
}

static sl_elem_t *_sl_elem_new(int level)
{
	sl_elem_t *e;
	
	e = calloc(1,sizeof(sl_elem_t) + level*sizeof(sl_elem_t *));
    if (e)
        e->level = level;
	return(e);
}

sl_elem_t *sl_elem_new(char *key, void *value)
{
    sl_elem_t *e;

    if (!key)
        return(NULL);
	e = _sl_elem_new(_random_level());
    e->key = key;
    e->value = value;
    return(e);
}

void sl_elem_free(sl_elem_t *e)
{
	free(e);
}


sl_list_t *sl_list_new()
{
	sl_list_t *l;

	if ((l = calloc(1,sizeof(sl_list_t))) == NULL)
		return(NULL);
	if ((l->_head = _sl_elem_new(LEVEL_MAX)) == NULL) {
		sl_list_free(l);
		return(NULL);
	}
	l->cmp = _elem_compare;
	return(l);
}

void sl_list_free(sl_list_t *l)
{
	sl_elem_t *e;

    if (!l)
        return;
	while (l->_head->forward[0]) {
		e = l->_head->forward[0]->forward[0];
	    sl_elem_free(l->_head->forward[0]);
		l->_head->forward[0] = e;
	}
	sl_elem_free(l->_head);
    free(l);
}

int sl_list_add(sl_list_t *l, sl_elem_t *e)
{
	int i,lvl;
	sl_elem_t *cur,*update[LEVEL_MAX - 1];
	
	if (!l || !e)
		return(-1);

	cur = l->_head;
	for (i = l->maxlevel - 1; i >= 0; i--) {
		while (cur->forward[i] != NULL
		 && l->cmp(cur->forward[i],e) < 0)
			cur = cur->forward[i];
		update[i] = cur;
	}
	if (e->level > l->maxlevel) {
		for (i = e->level - 1; i > l->maxlevel - 1; i--)
			update[i] = l->_head;
		l->maxlevel = e->level;
	}
	for (i = e->level - 1; i >= 0; i--) {
		e->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = e;
	}
    l->size++;
	return(0);
}

int sl_list_del(sl_list_t *l, sl_elem_t *e)
{
	int i;
	sl_elem_t *cur,*update[LEVEL_MAX - 1];
	
	if (!l || !e)
		return(-1);
	cur = l->_head;
	for (i = l->maxlevel - 1; i >= 0; i--) {
		while (cur->forward[i] != NULL && l->cmp(cur->forward[i],e) < 0)
			cur = cur->forward[i];
		update[i] = cur;
	}
	cur = cur->forward[0];
	if (cur == e) {
		for (i = e->level - 1; i >= 0; i--)
			update[i]->forward[i] = cur->forward[i];
	}
	if (e->level >= l->maxlevel) {
		for (i = e->level - 1; i >= 0; i--)
			if (l->_head->forward[i] != NULL)
				break;
		l->maxlevel = i + 1;
	}
    l->size--;
	return(0);
}

sl_elem_t *sl_list_get(sl_list_t *l, char *key)
{
	int i;
	sl_elem_t *cur,e;
	
	if (!l || !key)
		return(NULL);
	
	cur = l->_head;
	e.key = key;
	for (i = l->maxlevel - 1; i >= 0; i--)
		while (cur->forward[i] != NULL && l->cmp(cur->forward[i],&e) < 0)
			cur = cur->forward[i];
	if (l->cmp(cur->forward[0],&e) == 0)
		return(cur->forward[0]);
	return(NULL);
}

int sl_stats(sl_list_t *l)
{
    sl_elem_t *e;

    if (!l)
        return(-1);
    fprintf(stderr,"skiplist stats:\n");
    e = l->_head->forward[0];
    while (e) {
        fprintf(stderr,"%s ",e->key);
        e = e->forward[0];
    }
    return(0);
}
