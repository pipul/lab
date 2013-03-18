#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "skiplist.h"

#define SKIPLIST_P 0.25

static int32_t _random_level()
{
	int lvl = 1;
	while ((random()&0xFFFF) < (SKIPLIST_P * 0xFFFF))
		lvl += 1;
	return (lvl < LEVEL_MAX) ? lvl : LEVEL_MAX;
}

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



