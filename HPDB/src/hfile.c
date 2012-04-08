/* hfile.c - A persistant storage file implementation
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

#include "hfile.h"
#include "utils.h"

static int32_t _random_level()
{
	int lvl = 1;
	while ((rand()&0xFFFF) < (SKIPLIST_P * 0xFFFF))
		lvl += 1;
	return (lvl < LEVEL_MAX) ? lvl : LEVEL_MAX;
}



/* The Data Block segment ------->
 * To store key/value pairs, may be compressed. each block is the
 * unit of transfer to and from persistent storage. The default
 * block size is approximately 1mb uncompressed bytes.
 */

static int32_t _entry_compare(entry_t *s, entry_t *d)
{
	if (!s || !d) {
		if (s)
			return(1);
		if (d)
			return(-1);
	}
	return(sdscmp(s->key,d->key));
}

static entry_t *_entry_new_by_level(int32_t level)
{
	entry_t *e;
	
	if ((e = malloc(sizeof(entry_t) + level*sizeof(entry_t *))) == NULL)
		return(NULL);
	memset(e,0,sizeof(entry_t) + level*sizeof(entry_t *));
	e->level = level;
	return(e);
}

entry_t *entry_new()
{
	return(_entry_new_by_level(_random_level()));
}

void entry_free(entry_t *e)
{
	free(e);
}

void entry_destroy(entry_t *e)
{
	if (e != NULL) {
		sdsdel(e->key);
		sdsdel(e->value);
		entry_free(e);
	}
}

entry_t *entry_next(entry_t *e)
{
	if (e)
		return(e->forward[0]);
	else
		return(NULL);
}


int32_t entry_size(entry_t *e)
{
	if (!e)
		return(-1);
	return(2*sizeof(int32_t) + sdslen(e->key) + sdslen(e->value));
}

int32_t entry_valid(entry_t *e)
{
	if (!e)
		return(-1);
	if (sdslen(e->value) == 0)
		return(-1);
	return(0);
}


block_t *block_new()
{
	block_t *l;

	if ((l = malloc(sizeof(block_t))) == NULL)
		return(NULL);
	memset(l,0,sizeof(block_t));
	if ((l->_head = _entry_new_by_level(LEVEL_MAX)) == NULL) {
		block_destroy(l);
		return(NULL);
	}
	l->cmp = _entry_compare;
	return(l);
}

entry_t *block_head(block_t *l)
{
	if (l)
		return(l->_head->forward[0]);
	else
		return(NULL);
}

entry_t *block_tail(block_t *l)
{
	entry_t *e;
	
	e = l->_head;
	while (e->forward[0]) {
		e = entry_next(e);
	}
	if (e == l->_head)
		return(NULL);
	return(e);
}

int32_t block_size(block_t *l)
{
	entry_t *e;
	int32_t bsize;

	if (!l)
		return(-1);
	bsize = sizeof(uint64_t);
	for (e = block_head(l); e != NULL; e = entry_next(e))
		bsize = bsize + entry_size(e);
	return(bsize);
}

void block_free(block_t *l)
{
	free(l);
}

void block_destroy(block_t *l)
{
	entry_t *e;
	
	if (!l)
		return;
	while (l->_head->forward[0]) {
		e = l->_head->forward[0]->forward[0];
		entry_destroy(l->_head->forward[0]);
		l->_head->forward[0] = e;
	}
	entry_destroy(l->_head);
	block_free(l);
}

int32_t block_add(block_t *l, entry_t *e)
{
	int32_t i,lvl;
	entry_t *cur;
	entry_t *update[LEVEL_MAX - 1];
	
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

	return(0);
}

int32_t block_del(block_t *l, entry_t *e)
{
	int32_t i;
	entry_t *cur;
	entry_t *update[LEVEL_MAX - 1];
	
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
	return(0);
}

entry_t *block_loup(block_t *l, sds key)
{
	int32_t i;
	entry_t *cur;
	entry_t e;
	
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





block_t *block_load(int32_t fd, int32_t offset, int32_t size)
{
	block_t *l;
	entry_t *o;
	int32_t len;
	int8_t *buffer, *ptr;
	
	if (fd < 0 || offset < 0 || size < 0)
		return(NULL);
	if ((l = block_new()) == NULL)
		return(NULL);
	if ((buffer = malloc(size)) == NULL)
		__ERROR_LOG(B_ERROR);
	lseek(fd,offset,SEEK_SET);
	if (size != read(fd,buffer,size))
		__ERROR_LOG(R_ERROR);

	ptr = buffer;
	l->magic = *(uint64_t *)ptr;
	ptr = ptr + sizeof(uint64_t);
	if (l->magic != crc32_encode(ptr,size - sizeof(uint64_t)))
		__ERROR_LOG(C_ERROR);

	while (ptr - buffer < size) {
		if ((o = entry_new()) == NULL)
			continue;
		len = *(int32_t *)ptr;
		ptr = ptr + sizeof(int32_t);
		o->key = sdsnnew(ptr,len);
		ptr = ptr + len;

		len = *(int32_t *)ptr;
		ptr = ptr + sizeof(int32_t);
		o->value = sdsnnew(ptr,len);
		ptr = ptr + len;
		
		if (o->key && o->value) {
			block_add(l,o);
			continue;
		}
		if (o->key)
			sdsdel(o->key);
		if (o->value)
			sdsdel(o->value);
	}

	free(buffer);
	return(l);

C_ERROR:
R_ERROR:
	free(buffer);
B_ERROR:
	block_free(l);
	return(NULL);
}


int32_t block_swap(int32_t fd, int32_t offset, block_t *l)
{
	int32_t len;
	int32_t size;
	uint64_t magic;
	entry_t *o;
	int8_t *buffer, *ptr;

	if (fd < 0 || offset < 0 || !l)
		return(-1);
	if (-1 == fcntl(fd,F_SETFL,O_APPEND))
		return(-1);
	size = block_size(l);
	if ((buffer = malloc(size)) == NULL)
		return(-1);
	else
		ptr = buffer + sizeof(uint64_t);
	
	for (o = block_head(l); o != NULL; o = entry_next(o)) {
		len = sdslen(o->key);
		memcpy(ptr,&len,sizeof(int32_t)); 
		ptr = ptr + sizeof(int32_t);
		memcpy(ptr,o->key,len);
		ptr = ptr + len;
		
		len = sdslen(o->value);
		memcpy(ptr,&len,sizeof(int32_t));
		ptr = ptr + sizeof(int32_t);
		memcpy(ptr,o->value,len);
		ptr = ptr + len;
	}

	ptr = buffer + sizeof(uint64_t);
	*(uint64_t *)buffer = crc32_encode(ptr,size - sizeof(uint64_t));

	if (size != write(fd,buffer,size))
		__ERROR_LOG(W_ERROR);
	free(buffer);
	return(size);

W_ERROR:
	free(buffer);
	return(-1);
}







/* The Data Block Index segment ------->
 * Indexes the data block offset in the HFile. The key of each meta
 * is the key of first key/value pair in the block. as you see, index
 * is the list of meta node.
 */


static int32_t _meta_compare(meta_t *s, meta_t *d)
{
	return(sdscmp(s->key,d->key));
}

static meta_t *_meta_new_by_level(int32_t level)
{
	meta_t *m;
	
	if ((m = malloc(sizeof(meta_t) + level*sizeof(meta_t *))) == NULL)
		return(NULL);
	memset(m,0,sizeof(meta_t) + level*sizeof(meta_t *));
	m->level = level;
	return(m);
}

meta_t *meta_new()
{
	return(_meta_new_by_level(_random_level()));
}

void meta_free(meta_t *m)
{
	free(m);
}

void meta_destroy(meta_t *m)
{
	if (m == NULL)
		return;
	sdsdel(m->key);
	meta_free(m);
}

meta_t *meta_next(meta_t *m)
{
	if (m)
		return(m->forward[0]);
	else
		return(NULL);
}



index_t *index_new()
{
	index_t *l;
	
	if ((l = malloc(sizeof(index_t))) == NULL)
		return(NULL);
	memset(l,0,sizeof(index_t));
	if ((l->_head = _meta_new_by_level(LEVEL_MAX)) == NULL) {
		free(l);
		return(NULL);
	}
	l->cmp = _meta_compare;
	return(l);
}

void index_free(index_t *l)
{
	free(l);
}

void index_destroy(index_t *l)
{
	meta_t *m;

	if (!l)
		return;
	while (l->_head->forward[0]) {
		m = l->_head->forward[0]->forward[0];
		meta_destroy(l->_head->forward[0]);
		l->_head->forward[0] = m;
	}
	meta_destroy(l->_head);
	index_free(l);
}

meta_t *index_head(index_t *l)
{
	if (l)
		return(l->_head->forward[0]);
	else
		return(NULL);
}

meta_t *index_tail(index_t *l)
{
	meta_t *m;

	m = l->_head;
	while (m->forward[0]) {
		m = meta_next(m);
	}
	if (m == l->_head)
		return(NULL);
	return(m);
}

int32_t index_add(index_t *l, meta_t *m)
{
	int32_t i;
	meta_t *cur;
	meta_t *update[LEVEL_MAX - 1];
	
	if (!l || !m)
		return(-1);

	cur = l->_head;
	for (i = l->maxlevel - 1; i >= 0; i--) {
		while (cur->forward[i] != NULL && l->cmp(cur->forward[i],m) < 0)
			cur = cur->forward[i];
		update[i] = cur;
	}
	if (m->level > l->maxlevel) {
		for (i = m->level - 1; i > l->maxlevel - 1; i--)
			update[i] = l->_head;
		l->maxlevel = m->level;
	}
	for (i = m->level - 1; i >= 0; i--) {
		m->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = m;
	}

	return(0);
}

int32_t index_del(index_t *l, meta_t *m)
{
	int32_t i;
	meta_t *cur;
	meta_t *update[LEVEL_MAX - 1];
	
	if (!l || !m)
		return(-1);
	cur = l->_head;
	for (i = l->maxlevel - 1; i >= 0; i--) {
		while (cur->forward[i] != NULL && l->cmp(cur->forward[i],m) < 0)
			cur = cur->forward[i];
		update[i] = cur;
	}
	cur = cur->forward[0];
	if (cur == m) {
		for (i = m->level - 1; i >= 0; i--)
			update[i]->forward[i] = cur->forward[i];
	}
	if (m->level >= l->maxlevel) {
		for (i = m->level - 1; i >= 0; i--)
			if (l->_head->forward[i] != NULL)
				break;
		l->maxlevel = i + 1;
	}
	return(0);
}

meta_t *index_loup(index_t *l, sds key)
{
	int32_t i;
	meta_t *cur;
	meta_t m;
	
	if (!l || !key)
		return(NULL);
	
	cur = l->_head;
	m.key = key;
	for (i = l->maxlevel - 1; i >= 0; i--)
		while (cur->forward[i] != NULL && l->cmp(cur->forward[i],&m) <= 0)
			cur = cur->forward[i];
	if (cur != l->_head)
		return(cur);
	return(NULL);
}


index_t *index_load(int32_t fd, int32_t offset, int32_t size)
{
	int32_t id;
	int32_t len;
	index_t *l;
	int8_t *buffer, *ptr;
	meta_t *o;
	
	if (fd < 0 || offset < 0 || size < 0)
		return(NULL);
	if ((l = index_new()) == NULL)
		return(NULL);
	if ((buffer = malloc(size)) == NULL)
		__ERROR_LOG(B_ERROR);
	lseek(fd,offset,SEEK_SET);
	if (size != read(fd,buffer,size))
		__ERROR_LOG(R_ERROR);

	ptr = buffer;
	l->magic = *(uint64_t *)ptr;
	ptr = ptr + sizeof(uint64_t);
	if (l->magic != crc32_encode(ptr,size - sizeof(uint64_t)))
		__ERROR_LOG(C_ERROR);

	id = 0;		/* inode id */
	while (ptr - buffer < size) {
		if ((o = meta_new()) == NULL)
			break;
		o->offset = *(int32_t *)ptr;
		ptr = ptr + sizeof(int32_t);

		o->blocksize = *(int32_t *)ptr;
		ptr = ptr + sizeof(int32_t);

		len = *(int32_t *)ptr;
		ptr = ptr + sizeof(int32_t);
		o->key = sdsnnew(ptr,len);
		ptr = ptr + len;

		if (o->key == NULL) {
			meta_free(o);
			continue;
		}
		o->id = id++;
		index_add(l,o);
	}

	free(buffer);
	return(l);

C_ERROR:
R_ERROR:
	free(buffer);
B_ERROR:
	index_free(l);
	return(NULL);
}




int32_t index_swap(int32_t fd, int32_t offset, index_t *l)
{
	int32_t len;
	int32_t size;
	meta_t *o;
	int8_t *buffer, *ptr;

	if (fd < 0 || !l)
		return(-1);
	if (-1 == fcntl(fd,F_SETFL,O_APPEND))
		return(-1);
	size = sizeof(uint64_t);
	for (o = index_head(l); o != NULL; o = meta_next(o))
		size += 3*sizeof(int32_t) + sdslen(o->key);
	if ((buffer = malloc(size)) == NULL)
		return(-1);
	else
		ptr = buffer + sizeof(uint64_t);

	for (o = index_head(l); o != NULL; o = meta_next(o)) {
		memcpy(ptr,&o->offset,sizeof(int32_t));
		ptr = ptr + sizeof(int32_t);
		
		memcpy(ptr,&o->blocksize,sizeof(int32_t));
		ptr = ptr + sizeof(int32_t);
		
		len = sdslen(o->key);
		memcpy(ptr,&len,sizeof(int32_t));
		ptr = ptr + sizeof(int32_t);
		memcpy(ptr,o->key,len);
		ptr = ptr + len;
	}

	ptr = buffer + sizeof(uint64_t);
	*(uint64_t *)buffer = crc32_encode(ptr,size - sizeof(uint64_t));

	if (size != write(fd,buffer,size))
		__ERROR_LOG(S_ERROR);
	free(buffer);
	return(size);

S_ERROR:
	free(buffer);
	return(-1);
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



bloom_t *bloom_new(uint32_t n, float p)
{
	bloom_t *bloom;

	if (!(bloom = malloc(sizeof(bloom_t))))
		return(NULL);

	bloom->mbits = ceil(n*log(p)/(-log(2)*log(2)));
	bloom->len = _S_round_up(bloom->mbits)/CHAR_BIT;

	if (!(bloom->s = calloc(bloom->len,sizeof(int8_t)))) {
		free(bloom);
		return(NULL);
	}

	bloom->hashn = floor(0.7*(bloom->mbits)/n);
	bloom->hash = md532_encode;

	return(bloom);
}

void bloom_destroy(bloom_t *bloom)
{
	if (bloom == NULL)
		return;
	free(bloom->s);
	free(bloom);
}

int32_t bloom_add(bloom_t *bloom, const sds key)
{
	int32_t n;
	int32_t len;
	int8_t buf[KEY_MAX + 1];

	len = sdslen(key);
	memcpy(buf,key,len);
	buf[len+1] = '\0';

	for (n = 0; n < bloom->hashn; n++) {
		buf[len] = n + 1;
		SETBIT(bloom->s, (*bloom->hash)(buf,len)%bloom->mbits);
	}

	return(0);
}

int32_t bloom_check(bloom_t *bloom, const sds key)
{
	int32_t n;
	int32_t len;
	int8_t buf[KEY_MAX + 1];

	len = sdslen(key);
	memcpy(buf,key,len);
	buf[len+1] = '\0';

	for (n = 0; n < bloom->hashn; n++) {
		buf[len] = n + 1;
		if (!(GETBIT(bloom->s, 
			(*bloom->hash)(buf,len)%bloom->mbits)))
			return(-1);
	}

	return(0);
}

bloom_t *bloom_load(int32_t fd, int32_t offset, int32_t size)
{
	bloom_t *b;
	int8_t *buffer, *ptr;

	if (fd < 0 || offset < 0 || size < 0)
		return(NULL);
	if ((buffer = malloc(size)) == NULL)
		return(NULL);
	if ((b = malloc(sizeof(bloom_t))) == NULL)
		__ERROR_LOG(B_ERROR);
	memset(b,0,sizeof(bloom_t));
	lseek(fd,offset,SEEK_SET);
	if (size != read(fd,buffer,size))
		__ERROR_LOG(R_ERROR);
	memcpy((void *)b,buffer,3*sizeof(int32_t));
	ptr = buffer + 3*sizeof(int32_t);
	if ((b->s = malloc(b->len)) == NULL)
		__ERROR_LOG(S_ERROR);
	memcpy(b->s,ptr,b->len);
	b->hash = &md532_encode;

	free(buffer);
	return(b);

S_ERROR:
R_ERROR:
	free(b);
B_ERROR:
	free(buffer);
	return(NULL);
}


int32_t bloom_swap(int32_t fd, int32_t offset, bloom_t *b)
{
	int32_t size;
	int8_t *buffer, *ptr;
	
	if (fd < 0 || offset < 0 || !b)
		return(-1);
	size = 3*sizeof(int32_t) + b->len;
	if ((buffer = malloc(size)) == NULL)
		return(-1);
	memset(buffer,0,size);
	memcpy(buffer,b,3*sizeof(int32_t));
	ptr = buffer + 3*sizeof(int32_t);
	memcpy(ptr,b->s,b->len);
	
	lseek(fd,offset,SEEK_SET);
	if (size != write(fd,buffer,size))
		__ERROR_LOG(W_ERROR);
	free(buffer);
	return(size);

W_ERROR:
	free(buffer);
	return(-1);
}







/* The File info segment  ------->
 * It is a small metadata of the storage File, without compression.
 * developer can add user defined small metadata (name/value) here.
 * and rebuild the project.
 */


hinfo_t *hinfo_new()
{
	hinfo_t *fileinfo;
	
	if ((fileinfo = malloc(sizeof(hinfo_t))) == NULL)
		return(NULL);
	memset(fileinfo,0,sizeof(hinfo_t));
	return(fileinfo);
}

hinfo_t *hinfo_load(int32_t fd, int32_t offset, int32_t size)
{
	hinfo_t *fileinfo;
	int8_t *buffer, *ptr;
	
	if (fd < 0 || offset < 0 || size < 0)
		return(NULL);
	if ((fileinfo = hinfo_new()) == NULL)
		return(NULL);
	if ((buffer = malloc(size)) == NULL)
		__ERROR_LOG(M_ERROR);
	lseek(fd,offset,SEEK_SET);
	if (size != read(fd,buffer,size))
		__ERROR_LOG(R_ERROR);
	ptr = buffer;
	fileinfo->magic = *(int64_t *)ptr;
	ptr = ptr + sizeof(int64_t);
	if (fileinfo->magic != crc32_encode(ptr,size - sizeof(int64_t)))
		__ERROR_LOG(C_ERROR);
	fileinfo->blockcount = *(int32_t *)ptr;
	ptr = ptr + sizeof(int32_t);
	fileinfo->entrycount = *(int32_t *)ptr;
	ptr = ptr + sizeof(int32_t);
	fileinfo->lastkey = sdsnnew(ptr + sizeof(int32_t),*(int32_t *)ptr);
	
	free(buffer);
	return(fileinfo);

C_ERROR:
R_ERROR:
	free(buffer);
M_ERROR:
	hinfo_free(fileinfo);
	return(NULL);
}


int32_t hinfo_swap(int32_t fd, int32_t offset, hinfo_t *fileinfo)
{
	int32_t size;
	int8_t *buffer, *ptr;

	if (fd < 0 || offset < 0 || !fileinfo)
		return(-1);
	if (-1 == fcntl(fd,F_SETFL,O_APPEND))
		return(-1);
	size = sizeof(int64_t) + 3*sizeof(int32_t) + sdslen(fileinfo->lastkey);
	if ((buffer = malloc(size)) == NULL)
		return(-1);
	ptr = buffer + sizeof(int64_t);
	memcpy(ptr,&fileinfo->blockcount,sizeof(int32_t));
	ptr = ptr + sizeof(int32_t);
	memcpy(ptr,&fileinfo->entrycount,sizeof(int32_t));
	ptr = ptr + sizeof(int32_t);
	memcpy(ptr,fileinfo->lastkey - sizeof(sdshdr),
		sdslen(fileinfo->lastkey) + sizeof(int32_t));

	ptr = buffer + sizeof(uint64_t);
	*(uint64_t *)buffer = crc32_encode(ptr,size - sizeof(uint64_t));

	lseek(fd,offset,SEEK_SET);
	if (size != write(fd,buffer,size))
		__ERROR_LOG(R_ERROR);

	free(buffer);
	return(size);

R_ERROR:
	free(buffer);
	return(-1);
}

void hinfo_free(hinfo_t *fileinfo)
{
	free(fileinfo);
}

void hinfo_destroy(hinfo_t *fileinfo)
{
	if (fileinfo == NULL)
		return;
	sdsdel(fileinfo->lastkey);
	hinfo_free(fileinfo);
}





/* The Trailer segment  ------->
 * The fix sized metadata. To hold the offset and size of each segment,
 * etc. To read an HFile, we should always read the Trailer firstly.
 */


htail_t *htail_new()
{
	htail_t *trailer;
	if ((trailer = malloc(sizeof(htail_t))) == NULL)
		return(NULL);
	memset(trailer,0,sizeof(htail_t));
	trailer->version = CUR_VERSION;
	return(trailer);
}

htail_t *htail_load(int32_t fd, int32_t offset, int32_t size)
{
	htail_t *trailer;

	if (fd < 0 || size < 0)
		return(NULL);
	if ((trailer = htail_new()) == NULL)
		return(NULL);
	lseek(fd,offset,SEEK_END);
	if (size == read(fd,trailer,size))
		return(trailer);
	htail_free(trailer);
	return(NULL);
}

int32_t htail_swap(int32_t fd, int32_t offset, htail_t *trailer)
{
	int32_t size;
	if (fd < 0 || offset < 0 || !trailer)
		return(-1);
	if (-1 == fcntl(fd,F_SETFL,O_APPEND))
		return(-1);
	size = sizeof(htail_t);
	lseek(fd,offset,SEEK_SET);
	if (size != write(fd,trailer,size))
		return(-1);
	return(0);
}

void htail_free(htail_t *trailer)
{
	free(trailer);
}

void htail_destroy(htail_t *trailer)
{
	if (trailer == NULL)
		return;
	free(trailer);
}





/* The HFILE Handler  ------->
 * When open a HFILE, will create an instance of a HFILE_T, HFILE_T
 * maintains a pointer to point to each segment of the storage file.
 */


hfile_t *hfile_new()
{
	hfile_t *fp;
	if ((fp = malloc(sizeof(hfile_t))) == NULL)
		return(NULL);
	memset(fp,0,sizeof(hfile_t));
	return(fp);
}

void hfile_free(hfile_t *fp)
{
	free(fp);
}

void hfile_destroy(hfile_t *fp)
{
	meta_t *o;
	if (fp == NULL)
		return;
	hinfo_destroy(fp->fileinfo);
	htail_destroy(fp->trailer);
	for (o = index_head(fp->metas); o != NULL; o = meta_next(o))
		block_destroy(fp->blocks[o->id]);
	index_destroy(fp->metas);
	bloom_destroy(fp->bloom);
	free(fp->blocks);
	hfile_free(fp);
}


hfile_t *hfile_next(hfile_t *fp)
{
	return(fp->next);
}

hfile_t *hfile_prev(hfile_t *fp)
{
	return(fp->prev);
}


hfile_t *hfile_load(const int8_t *filename)
{
	int32_t fd;
	hfile_t *fp;
	meta_t *o;
	htail_t *ht;
	hinfo_t *hi;
	
	if (!filename)
		return(NULL);
	if ((fp = hfile_new()) == NULL)
		return(NULL);
	memcpy(fp->name,filename,strlen(filename));
	if ((fd = open(fp->name,O_RDONLY)) < 0)
		__ERROR_LOG(H_ERROR);

	fp->trailer = htail_load(fd,-sizeof(htail_t),sizeof(htail_t));
	if (fp->trailer == NULL)
		__ERROR_LOG(H_ERROR);
	else
		ht = fp->trailer;

	fp->fileinfo = hinfo_load(fd,ht->infooffset,ht->infosize);
	if (fp->fileinfo == NULL)
		__ERROR_LOG(H_ERROR);
	else
		hi = fp->fileinfo;

	fp->metas = index_load(fd,ht->indexoffset,ht->indexsize);
	if (fp->metas == NULL)
		__ERROR_LOG(H_ERROR);
	
	fp->bloom = bloom_load(fd,ht->bloomoffset,ht->bloomsize);
	if (fp->bloom == NULL)
		__ERROR_LOG(H_ERROR);

	fp->blocks = malloc(hi->blockcount*sizeof(block_t *));
	if (fp->blocks == NULL)
		__ERROR_LOG(H_ERROR);
	memset(fp->blocks,0,hi->blockcount*sizeof(block_t *));

	close(fd);
	return(fp);

H_ERROR:
	close(fd);
	hfile_destroy(fp);
	return(NULL);
}

int32_t hfile_swap(hfile_t *fp)
{
	int32_t fd;
	meta_t *o;
	int8_t *ptr;
	int32_t len;
	if (!fp)
		return(-1);

	len = strlen(fp->name);
	ptr = fp->name + len;

	umask(0);	
	do {
		fp->trailer->timestamp = time(NULL);
		snprintf(ptr,PATH_MAX - len,"%ld",fp->trailer->timestamp);
		fd = open(fp->name,O_WRONLY|O_APPEND|O_CREAT|O_EXCL,0644);
	} while (fd < 0);
	
	for (o = index_head(fp->metas); o != NULL; o = meta_next(o)) {
		o->offset = lseek(fd,0,SEEK_CUR);
		o->blocksize = block_swap(fd,o->offset,fp->blocks[o->id]);
	}
	fp->trailer->indexoffset = lseek(fd,0,SEEK_CUR);
	fp->trailer->indexsize = index_swap(fd,fp->trailer->indexoffset,fp->metas);
	fp->trailer->bloomoffset = lseek(fd,0,SEEK_CUR);
	fp->trailer->bloomsize = bloom_swap(fd,fp->trailer->bloomoffset,fp->bloom);
	fp->trailer->infooffset = lseek(fd,0,SEEK_CUR);
	fp->trailer->infosize = hinfo_swap(fd,fp->trailer->infooffset,fp->fileinfo);
	fp->trailer->version = hfile_version(NULL);
	htail_swap(fd,lseek(fd,0,SEEK_CUR),fp->trailer);
	close(fd);
	return(0);
}



int32_t hfile_version(hfile_t *fp)
{
	if (!fp)
		return(-1);
	return(fp->trailer->version);
}

int32_t hfile_timestamp(hfile_t *fp)
{
	if (!fp)
		return(-1);
	return(fp->trailer->timestamp);
}


entry_t *hfile_loup(hfile_t *fp, sds key)
{
	int32_t fd;
	meta_t *m;
	entry_t *t;
	sds firstkey;

	if (!fp || !key)
		return(NULL);
	if (-1 == bloom_check(fp->bloom,key))
		return(NULL);
	firstkey = (index_head(fp->metas))->key;
	if (sdscmp(key,firstkey) < 0)
		return(NULL);
	if (sdscmp(key,fp->fileinfo->lastkey) > 0)
		return(NULL);
	m = index_loup(fp->metas,key);
	if (m->state != IN_CACHE) {
		if ((fd = open(fp->name,O_RDONLY)) < 0)
			/* error handle */
			return(NULL);
		fp->blocks[m->id] = block_load(fd,m->offset,m->blocksize);
		m->state = IN_CACHE;
		close(fd);
	}
	t = block_loup(fp->blocks[m->id],key);
	return(t);
}







/* The HFILE list  ------->
 * Startup, the system will first retrieve the database directory,
 * and then initialize all the data files into hfile_t instance, add
 * to hfilelist.
 */


static int32_t _hfile_compare(hfile_t *s, hfile_t *h)
{
	if (!s || !h)
		return(0);
	if (hfile_timestamp(s) > hfile_timestamp(h))
		return(1);
	else if (hfile_timestamp(s) < hfile_timestamp(h))
		return(-1);
	return(0);
}



hfilelist_t *hfilelist_new()
{
	hfilelist_t *hl;
	
	if ((hl = malloc(sizeof(hfilelist_t))) == NULL)
		return(NULL);
	memset(hl,0,sizeof(hfilelist_t));
	hl->cmp = _hfile_compare;
	return(hl);
}

void hfilelist_free(hfilelist_t *hl)
{
	free(hl);
}

void hfilelist_destroy(hfilelist_t *hl)
{
	hfile_t *fp, *next_fp;

	if (hl == NULL)
		return;
	for (fp = hl->first; fp != NULL; ) {
		next_fp = fp->next;
		hfile_destroy(fp);
		fp = next_fp;
	}
	
	hfilelist_free(hl);
}



int32_t hfilelist_add(hfilelist_t *hl, hfile_t *fp)
{
	hfile_t *j;

	if (!hl || !fp)
		return(-1);
	if (hl->first == NULL) {
		hl->first = hl->last = fp;
		hl->count++;
		return(0);
	}
	
	if (hl->first == hl->last) {
		if (hl->cmp(fp,hl->first) > 0) {
			fp->next = hl->first;
			hl->first->prev = fp;
			hl->first = fp;
			hl->count++;
			return(0);
		} else {
			fp->prev = hl->first;
			hl->first->next = fp;
			hl->last = fp;
			hl->count++;
			return(0);
		}
	}

	for (j = hl->first; j != NULL; j = j->next)
		if (hl->cmp(fp,j) > 0)
			break;

	if (j == hl->first) {
		fp->next = j;
		j->prev = fp;
		hl->first = fp;
		fp->prev = NULL;	
		hl->count++;
		return(0);
	}

	if (j == NULL) {
		fp->prev = hl->last;
		hl->last->next = fp;
		hl->last = fp;
		hl->count++;
		return(0);
	}
	
	fp->next = j;
	fp->prev = j->prev;
	j->prev->next = fp;
	j->prev = fp;
	hl->count++;
	return(0);
}

int32_t hfilelist_del(hfilelist_t *hl, hfile_t *fp)
{
	if (!hl || !fp)
		return(-1);
	if (fp->next == NULL) {
		hl->last = fp->prev;
		if (hl->last != NULL)
			hl->last->next = NULL;
	} else
		fp->next->prev = fp->prev;

	if (fp->prev == NULL) {
		hl->first = fp->next;
		if (hl->first != NULL)
			hl->first->prev = NULL;
	} else
		fp->prev->next = fp->next;
}

hfile_t *hfilelist_head(hfilelist_t *hl)
{
	if (hl == NULL)
		return(NULL);
	return(hl->first);
}

hfile_t *hfilelist_tail(hfilelist_t *hl)
{
	if (hl == NULL)
		return(NULL);
	return(hl->last);
}













