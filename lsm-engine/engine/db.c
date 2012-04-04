/* db.c - A DB handler implementation
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
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

#include "sstable.h"
#include "memtable.h"
#include "utils.h"
#include "db.h"


DB *db_new()
{
	DB *db;
	if ((db = malloc(sizeof(DB))) == NULL)
		return(NULL);
	memset(db,0,sizeof(db));
	return(db);
}

void db_free(DB *db)
{
	free(db);
}

void db_destroy(DB *db)
{
	if (!db)
		return;
	memtable_destroy(db->memtable);
	hfilelist_destroy(db->hfiles);
	db_free(db);
}

int32_t _db_hfileNameCheck(int8_t *filename)
{
	if (strcmp(filename,".") == 0)
		return(-1); 
	if (strcmp(filename,"..") == 0)
		return(-1);
	if (strcmp(filename,EVT_FILE) == 0)
		return(-1);
	if (strcmp(filename,LOG_FILE) == 0)
		return(-1);
	return(0);
}


DB *db_open(const int8_t *pathname)
{
	DB *db;
	DIR *dp;
	sst_t *fp;
	struct dirent *dir;
	int8_t filename[PATH_MAX];

	snprintf(filename,PATH_MAX,"%s/%s",pathname,LOG_FILE);
	stdlog = fopen(filename,"a+");
	snprintf(filename,PATH_MAX,"%s/%s",pathname,EVT_FILE);
	stdevt = fopen(filename,"a+");
	
	if (stdlog < 0 || stdevt < 0) {
		fclose(stdlog);
		fclose(stdevt);
		return(NULL);
	}

	if ((dp = opendir(pathname)) == NULL)
		__ERROR_LOG(P_ERROR);
	if ((db = db_new()) == NULL)
		__ERROR_LOG(D_ERROR);
	if ((db->memtable = memtable_new()) == NULL)
		__ERROR_LOG(D_ERROR);
	if ((db->hfiles = hfilelist_new()) == NULL)
		__ERROR_LOG(D_ERROR);

	snprintf(db->path,PATH_MAX,"%s",pathname);
	while ((dir = readdir(dp)) != NULL) {
		if (_db_hfileNameCheck(dir->d_name) == -1)
			continue;
		snprintf(filename,PATH_MAX,"%s/%s",pathname,dir->d_name);
		if ((fp = sst_open(filename)) == NULL)
			continue;
		hfilelist_add(db->hfiles,fp);
	}

	db->close = _db_close;
	db->del = _db_del;
	db->get = _db_get;
	db->put = _db_put;
	db->sync = _db_sync;
	
	closedir(dp);
	return(db);

D_ERROR:
	db_destroy(db);
	closedir(dp);
P_ERROR:
	fclose(stdlog);
	fclose(stdevt);
	return(NULL);
}

int32_t _db_close(DB *db)
{
	if (!db)
		return(-1);
	db_destroy(db);
	fclose(stdlog);
	fclose(stdevt);
}

int32_t _db_put(const DB *db, sds key, sds value)
{
	entry_t *o;
	if (!db || !key || !value)
		return(-1);
	if ((o = memtable_loup(db->memtable,key)) != NULL) {
		memtable_del(db->memtable,o);
		entry_destroy(o);
	}
	if ((o = entry_new()) == NULL)
		return(-1);
	o->key = key;
	o->value = value;
	if (0 == memtable_add(db->memtable,o))
		return(0);
	entry_free(o);
	return(-1);
}


int32_t _db_del(const DB *db, sds key)
{
	entry_t *o;
	if (!db || !key)
		return(-1);
	o = memtable_loup(db->memtable,key);
	if (o == NULL) {
		o = entry_new();
		o->key = sdsdup(key);
		o->value = sdsnul();
		return(memtable_add(db->memtable,o));
	}
	memtable_del(db->memtable,o);
	entry_destroy(o);
	o = entry_new();
	o->key = sdsdup(key);
	o->value = sdsnul();
	return(memtable_add(db->memtable,o));
}

int8_t *_db_get(const DB *db, sds key)
{
	entry_t *o;
	sst_t *fp;
	
	o = memtable_loup(db->memtable,key);
	if (o != NULL) {
		if (entry_valid(o) == 0)
			return(o->value);
		else
			return(NULL);
	}

	for (fp = hfilelist_head(db->hfiles); fp != NULL; fp = sst_next(fp))
		if ((o = sst_loup(fp,key)) != NULL)
			break;
	if (entry_valid(o) == 0)
		return(o->value);
	else
		return(NULL);
}

int32_t _db_sync(DB *db)
{
	sst_t *fp;
	memtable_t *memtable;

	if (!db)
		return(-1);
	memtable = db->memtable;
	if ((db->memtable = memtable_new()) == NULL)
		return(-1);
	if ((fp = memtable_to_sst(memtable)) == NULL)
		__ERROR_LOG(ERROR);

	snprintf(fp->name,PATH_MAX,"%s/",db->path);
	if (0 == sst_swap(fp))
		hfilelist_add(db->hfiles,fp);
	memtable_destroy(memtable);
	return(0);
ERROR:
	memtable_destroy(memtable);
	return(-1);
}









/* The HFILE list  ------->
 * Startup, the system will first retrieve the database directory,
 * and then initialize all the data files into sst_t instance, add
 * to hfilelist.
 */


static int32_t _sst_compare(sst_t *s, sst_t *h)
{
	if (!s || !h)
		return(0);
	if (sst_timestamp(s) > sst_timestamp(h))
		return(1);
	else if (sst_timestamp(s) < sst_timestamp(h))
		return(-1);
	return(0);
}



hfilelist_t *hfilelist_new()
{
	hfilelist_t *hl;
	
	if ((hl = malloc(sizeof(hfilelist_t))) == NULL)
		return(NULL);
	memset(hl,0,sizeof(hfilelist_t));
	hl->cmp = _sst_compare;
	return(hl);
}

void hfilelist_free(hfilelist_t *hl)
{
	free(hl);
}

void hfilelist_destroy(hfilelist_t *hl)
{
	sst_t *fp, *next_fp;

	if (hl == NULL)
		return;
	for (fp = hl->first; fp != NULL; ) {
		next_fp = fp->next;
		sst_close(fp);
		fp = next_fp;
	}
	
	hfilelist_free(hl);
}



int32_t hfilelist_add(hfilelist_t *hl, sst_t *fp)
{
	sst_t *j;

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

int32_t hfilelist_del(hfilelist_t *hl, sst_t *fp)
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

sst_t *hfilelist_head(hfilelist_t *hl)
{
	if (hl == NULL)
		return(NULL);
	return(hl->first);
}

sst_t *hfilelist_tail(hfilelist_t *hl)
{
	if (hl == NULL)
		return(NULL);
	return(hl->last);
}







