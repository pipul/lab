#include <stdio.h>
#include <string.h>
#include "skiplist.h"

/* The Base Datastruct : SkipList ------->
 *
 * A skip list is a data structure for storing a sorted list of items 
 * using a hierarchy of linked lists that connect increasingly sparse 
 * subsequences of the items. These auxiliary lists allow item lookup 
 * with efficiency comparable to balanced binary search trees (that is, 
 * with number of probes proportional to log(n) instead of n).
 */

static int _layerUpdate(skipNode *_n, uint8 l)
{
	skipNode *cur = nodePrev(_n,0);
	
	if (!_n)
		return(-1);
		
	while (nodeHeight(cur) < l) {
		cur = nodePrev(cur,0);
	}
	
	nodePrev(_n,l) = cur;
	nodeNext(_n,l) = nodeNext(cur,l);
	
	nodePrev(nodeNext(cur,l),l) = _n;
	nodeNext(cur,l) = _n;
	
	return(0);
}

int skiplistInsert(skipList *L, skipNode *_n)
{
	skipNode *cur = listHead(L);
	int h;
	
	h = nodeHeight(cur) - 1;
	
LOOKUP:
	switch (L->cmp(cur,_n)) {
	
	case -1:
		if (nodeNext(cur,h))
			cur = nodeNext(cur,h);
		else
			cur = nodeNext(nodePrev(cur,h--),h);	
		goto LOOKUP;

	case 1:
		if (h > 0)
			cur = nodeNext(nodePrev(cur,h--),h)
		else
			break;
	}

	nodeNext(_n,0) = cur;
	nodePrev(_n,0) = nodePrev(cur,0);
	nodeNext(nodePrev(cur,0),0) = _n;
	nodePrev(cur,0) = _n;
	
	for (h = 1 ; h < nodeHeight(_n) ; h++)
		_layerUpdate(_n, h);
	
	return(0);
}

int skiplistDelete(skipNode *_n)
{
	int h = 0;
	
	while (h < nodeHeight(_n)) {
		prevNode(nextNode(_n,h),h) = prevNode(_n,h);
		nextNode(prevNode(_n,h),h) = nextNode(_n,h);
		h++;
	}

	return(0);
}

/* The KV Core Module : memory key-value db ------->
 * 
 * Memory key-value db is base on skiplist witch described above.
 * coutune impremente some a little succinct function interface
 *
 */

static int _mdbRecordRandomLevel()
{
	int level = 1;
	while ((random()&0xFFFF) < (SKIPLIST_P * 0xFFFF))
		level++;
	return (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}

/*
 * The _mdbRecordCompare() function is the default internal interface
 * for comparing two mdbRecords r1 and r2 by its key strings. It returns
 * an integer less than, equal to, or greater than zero.
 */

int _mdbRecordCompare(mdbRecord *r1, mdbRecord *r2)
{
	int len;
	
	len = (sdsLen(r1->key) 
		> sdsLen(r2->key)) ? sdsLen(r1->key) : sdsLen(r2->key);
	
	return memcpy(r1->key,r2->key,len);
}

mdbTable *mdbTableCreate(mdbRecordCompare *recordCmp)
{
	mdbTable *mt;
	mdbRecord *head, *tail;

	char *head_key, *tail_key;
	
	/*
	 * Head and Tail is two special mdbRecord in the mdbTable.
	 * Head is the biggest one and the Tail is the least. this
	 * two specific mdbRecord is used to avoid overlap of insert
	 * and lookup operation.
	 */

	head_key = (char *)malloc(MAX_KEYLEN);
	tail_key = (char *)malloc(MAX_KEYLEN);
	
	if (!head_key || !tail_key)
		return(NULL);
	
	memset(head_key,0,MAX_KEYLEN);
	memset(tail_key,1,MAX_KEYLEN);
	head = mdbRecordCreate(sdsNew(head_key,MAX_KEYLEN));
	tail = mdbRecordCreate(sdsNew(tail_key,MAX_KEYLEN));
	free(head_key);
	free(tail_key);

	if (!head || !tail)
		return(NULL):
	if ((mt = malloc(sizeof(mdbTable))) == NULL)
		return(NULL);
	mt->head = head;
	mt->tail = tail;
	mt->cmp = (recordCmp == NULL) ? _mdbRecordCompare : recordCmp;

	return(mt);
}

int mdbTableFree(mdbTable *mt)
{
	if (!mt)
		return(-1);
	mdbRecordFree(mt->head);
	mdbRecordFree(mt->tail);
	free(mt);

	return(0);
}

int mdbTableInsert(mdbTable *mt, mdbRecord *mr)
{
	return skiplistInsert(mt,mr);
}

int mdbTableDelete(mdbTable *mt, mdbRecord *mr)
{
	skiplistDelete(mt,mr);
	mdbRecordFree(mr);

	return(0);
}

mdbRecord *mdbTableFind(mdbTable *mr, const char *key, int flags)
{
	mdbRecord *cur = listHead(mr);
	mdbRecord *tmp = mdbRecordCreate(key);
	int h = recordLevel(cur) - 1;

LOOKUP:
	switch (mt->cmp(cur,tmp)) {
	
	case -1:
		if (nextRecord(cur,h))
			cur = nextRecord(cur,h);
		else
			cur = nextRecord(prevRecord(cur,h--),h);	
		goto LOOKUP;

	case 1:
		if (h > 0)
			cur = nextRecord(prevRecord(cur,h--),h)
		else
			break;
	case 0:
		if (flags & EQUAL) {
			tmp->key = NULL;
			mdbRecordFree(tmp);
			return(cur);
		} else {
			break;
		}
	}

	/*
	 * Free the tmp mdbRecord.
	 */
	tmp->key = NULL;
	mdbRecordFree(tmp);	

	if (cur == listHead(mr) || cur == listTail(mr)) {
		return(NULL);
	} else {
		switch (flags) {
			case LESSER_MAX:
				return(prevRecord(cur));
			case GREATER_MIN:
				return(cur);
		}
	}
}

mdbRecord *mdbRecordCreate(const char *key)
{
	if (!key)
		return(NULL);
	int height = _mdbRecordRandomLevel();

	mdbRecord *mr = malloc(
			sizeof(mdbRecord) + height*sizeof(skipNodeLevel));
	if (!mr)
		return(NULL);
	mr->height = height;
	mr->key = key;

	return(mr);
}

int mdbRecordSetValue(mdbRecord *mr, char *value)
{
	mr->value = value;
	return(0);
}

int mdbRecordFree(mdbRecord *mr)
{
	sdsDel(mr->key);
	sdsDel(mr->value);
	free(mr);

	return(0);
}


