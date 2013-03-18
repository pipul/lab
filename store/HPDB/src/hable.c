/* hable.c - A memory table implementation
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


#include "hable.h"


hable_t *hable_new()
{
	hable_t *h;
	
	if ((h = malloc(sizeof(hable_t))) == NULL)
		return(NULL);
	memset(h,0,sizeof(hable_t));
	if ((h->block = block_new()) == NULL)
		__ERROR_LOG(B_ERROR);
	return(h);

B_ERROR:
	hable_free(h);
	return(NULL);
}

void hable_free(hable_t *h)
{
	free(h);
}

void hable_destroy(hable_t *h)
{
	if (!h)
		return;
	block_destroy(h->block);
	hable_free(h);
}

entry_t *hable_head(hable_t *h)
{
	return(block_head(h->block));
}

entry_t *hable_tail(hable_t *h)
{
	return(block_tail(h->block));
}

int32_t hable_add(hable_t *h, entry_t *e)
{
	if (0 != block_add(h->block,e))
		return(-1);
	h->count++;
	return(0);
}

int32_t hable_del(hable_t *h, entry_t *e)
{
	if (0 != block_del(h->block,e))
		return(-1);
	h->count--;
	return(0);
}

entry_t *hable_loup(hable_t *h, sds key)
{
	return(block_loup(h->block,key));
}





static int32_t hable_get_block_count(hable_t *h)
{
	int32_t blockcount = 0;
	int32_t blocksize;
	entry_t *e = hable_head(h);
	
	if (h == NULL)
		return(-1);

	do {
		blocksize = BLOCK_SIZE;
		blocksize += - sizeof(uint64_t) - entry_size(e);
		e = entry_next(e);
		while (entry_size(e) < blocksize) {
			blocksize = blocksize - entry_size(e);
			if ((e = entry_next(e)) == NULL)
				break;
		}
	
		blockcount++;
	} while (e != NULL);
	return(blockcount);
}


static block_t *hable_get_one_block(hable_t *h)
{
	block_t *block;
	int32_t blocksize = BLOCK_SIZE;
	entry_t *e = hable_head(h);

	if (h == NULL || e == NULL)
		return(NULL);
	if ((block = block_new()) == NULL)
		return(NULL);

	blocksize += -sizeof(uint64_t) - entry_size(e);
	hable_del(h,e);
	block_add(block,e);

	e = hable_head(h);
	while (e && entry_size(e) < blocksize) {
		hable_del(h,e);
		block_add(block,e);
		blocksize = blocksize - entry_size(e);
		if ((e = hable_head(h)) == NULL)
			break;
	}
	return(block);
}


static bloom_t *hable_get_bloomdata(hable_t *h)
{
	bloom_t *bloom;
	entry_t *e;

	if (h == NULL || h->count == 0)
		return(NULL);
	if ((bloom = bloom_new(h->count,BLOOM_P)) == NULL)
		return(NULL);
	for (e = hable_head(h); e != NULL; e = entry_next(e))
		bloom_add(bloom,e->key);
	return(bloom);
}










hfile_t *hable_to_hfile(hable_t *h)
{
	hfile_t *fp;
	meta_t *m;
	entry_t *e;
	int32_t id;

	if (!h)
		return(NULL);
	if ((fp = hfile_new()) == NULL)
		return(NULL);
	if ((fp->trailer = htail_new()) == NULL)
		__ERROR_LOG(H_ERROR);
	if ((fp->fileinfo = hinfo_new()) == NULL)
		__ERROR_LOG(H_ERROR);
	if ((fp->metas = index_new()) == NULL)
		__ERROR_LOG(H_ERROR);
	if ((fp->bloom = hable_get_bloomdata(h)) == NULL)
		__ERROR_LOG(H_ERROR);

	fp->fileinfo->entrycount = h->count;
	fp->fileinfo->blockcount = hable_get_block_count(h);
	fp->fileinfo->lastkey = sdsdup(hable_tail(h)->key);

	fp->blocks = malloc(fp->fileinfo->blockcount*sizeof(block_t *));
	if (fp->blocks == NULL)
		__ERROR_LOG(H_ERROR);
	memset(fp->blocks,0,fp->fileinfo->blockcount*sizeof(block_t *));

	for (id = 0; id < fp->fileinfo->blockcount; id++) {
		if ((fp->blocks[id] = hable_get_one_block(h)) == NULL)
			__ERROR_LOG(H_ERROR);
		if ((m = meta_new()) == NULL)
			__ERROR_LOG(H_ERROR);
		m->id = id;
		e = block_head(fp->blocks[m->id]);
		m->key = sdsdup(e->key);
		m->blocksize = block_size(fp->blocks[m->id]);
		m->state = IN_CACHE;
		if (-1 == index_add(fp->metas,m))
			__ERROR_LOG(H_ERROR);
	}
	

	return(fp);

H_ERROR:
	hfile_destroy(fp);
	return(NULL);
}
