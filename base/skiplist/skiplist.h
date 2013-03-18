#ifndef __SKIPLIST_H_
#define __SKIPLIST_H_

#include <stdint.h>
#include "utils.h"

#define LEVEL_MAX 15

typedef struct HFILE hfile_t;

typedef struct skiplist_node entry_t;
typedef struct skiplist_list block_t;

struct skiplist_node {
	sds key;
	sds value;
	int32_t level;
	entry_t *forward[];
} ;

struct skiplist_list {
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
int32_t block_size(block_t *l);
int32_t block_add(block_t *l, entry_t *e);
int32_t block_del(block_t *l, entry_t *e);

block_t *block_load(int32_t fd, int32_t offset, int32_t size);
int32_t block_swap(int32_t fd, int32_t offset, block_t *l);
entry_t *block_loup(block_t *l, sds key);







#endif
