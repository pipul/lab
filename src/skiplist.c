#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "skiplist.h"
#include "sds.h"

/* The Base Datastruct : SkipList ------->
 *
 * A skip list is a data structure for storing a sorted list of items 
 * using a hierarchy of linked lists that connect increasingly sparse 
 * subsequences of the items. These auxiliary lists allow item lookup 
 * with efficiency comparable to balanced binary search trees (that is, 
 * with number of probes proportional to log(n) instead of n).
 */

#define prevNode(_n,h) (_n->level[h].prev)
#define nextNode(_n,h) (_n->level[h].next)
#define nodeHeight(_n) (_n->height)

#define listHead(_l) (_l->head)
#define listTail(_l) (_l->tail)

skiplistIter *skiplistIterCreate(int flags, int height)
{
	skiplistIter *si;
	
	/*
	 * This flags bit mush be NEXTITER or PREVITER.
	 */
	if (!(flags & NEXTITER || flags & PREVITER))
		return(NULL);
	
	/*
	 * What height indicate to iterator is performing operation
	 * on which height of skipNode. so the value of height mush
	 * less than SKIPLIST_MAXLEVEL(32)
	 */
	if (height >= SKIPLIST_MAXLEVEL-1)
		return(NULL);
	if ((si = (skiplistIter *)malloc(sizeof(skiplistIter))) == NULL)
		return(NULL);
		
	si->flags = flags;
	si->height = height;
	si->curNode = NULL;

	return(si);
}

skiplistIter *skiplistSetIter(skiplistIter *si, skipNode *sn)
{
	if (!si || !sn || sn->height-1 < si->height)
		return(NULL);
	si->curNode = sn;
	return(si);
}

skipNode *skiplistIterator(skiplistIter *si)
{
	if (!si || si->curNode->height-1 < si->height)
		return(NULL);

	switch (si->flags) {
		case NEXTITER:
			if (nextNode(si->curNode,si->height))
				si->curNode = nextNode(si->curNode,si->height);
			break;
		case PREVITER:
			if (prevNode(si->curNode,si->height))
				si->curNode = prevNode(si->curNode,si->height);
			break;
		default:
			break;
	}
	
	return(si->curNode);
}


void skiplistIterFree(skiplistIter *si)
{
	if (!si)
		return;
	free(si);
}

/*
 * _layerUpdate() is a internal interface for skipListInsert()
 * function in order to update skipNode level's link.
 */
 
static int _layerUpdate(skipNode *_n, int l)
{
	skipNode *cur = prevNode(_n,0);
	
	if (!_n)
		return(-1);

	while (nodeHeight(cur)-1 < l) {
		cur = prevNode(cur,0);
	}
	
	prevNode(_n,l) = cur;
	nextNode(_n,l) = nextNode(cur,l);
	
	prevNode(nextNode(cur,l),l) = _n;
	nextNode(cur,l) = _n;

	return(0);
}

int skiplistInsert(skipList *sl, skipNode *_n)
{
	skipNode *cur = listHead(sl);
	int h, cmpRes;
	
	h = nodeHeight(cur) - 1;
	
LOOKUP:
	
	if (h > 0) {
		cmpRes = sl->cmp(cur,_n);
		if (cmpRes < 0) {
			if (nextNode(cur,h))
				cur = nextNode(cur,h);
			else
				cur = nextNode(prevNode(cur,h--),h);	
			goto LOOKUP;
		} else if (cmpRes > 0) {
			cur = nextNode(prevNode(cur,h--),h);
			goto LOOKUP;
		}
	}

	nextNode(_n,0) = cur;
	prevNode(_n,0) = prevNode(cur,0);
	nextNode(prevNode(cur,0),0) = _n;
	prevNode(cur,0) = _n;
	
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


#undef prevNode
#undef nextNode
#undef nodeHeight
#undef listHead
#undef listTail



/* The KV Core Module : memory key-value db ------->
 * 
 * Memory key-value db is base on skiplist witch described above.
 * coutune impremente some a little succinct function interface
 *
 */

#define prevMecord(_n,h) (_n->level[h].prev)
#define nextMecord(_n,h) (_n->level[h].next)
#define mecordHeight(_n) (_n->height)


static int _mecordRandomLevel()
{
	int level = 1;
	while ((random()&0xFFFF) < (SKIPLIST_P * 0xFFFF))
		level++;
	return (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}

/*
 * The _mecordCompare() function is the default internal interface
 * for comparing two mecords r1 and r2 by its key strings. It returns
 * an integer less than, equal to, or greater than zero.
 */

int _mecordCompare(mecord *r1, mecord *r2)
{
	int len;
	
	len = (sdsLen(r1->key) 
		> sdsLen(r2->key)) ? sdsLen(r1->key) : sdsLen(r2->key);
	
	return memcmp(r1->key,r2->key,len);
}


mableIter *mableIterCreate(int flags, int height)
{
	return skiplistIterCreate(flags,height);
}

mableIter *mableSetIter(mableIter *ti, mecord *mr)
{
	return skiplistSetIter(ti,mr);
}

mecord *mableIterator(mableIter *ti)
{
	return skiplistIterator(ti);
}

void mableIterFree(mableIter *ti)
{
	skiplistIterFree(ti);
}


mable *mableCreate(mecordCompare *recordCmp)
{
	mable *mt;
	mecord *head, *tail;

	char *head_key, *tail_key;
	
	/*
	 * Head and Tail is two special mecord in the mable.
	 * Head is the biggest one and the Tail is the least. this
	 * two specific mecord is used to avoid overlap of insert
	 * and lookup operation.
	 */

	head_key = (char *)malloc(MAX_KEYLEN);
	tail_key = (char *)malloc(MAX_KEYLEN);
	
	if (!head_key || !tail_key)
		return(NULL);
	
	memset(head_key,0,MAX_KEYLEN);
	memset(tail_key,1,MAX_KEYLEN);
	
	head = (mecord *)malloc(sizeof(mecord));
	head->level = malloc(SKIPLIST_MAXLEVEL*sizeof(mecordLink));
	tail = (mecord *)malloc(sizeof(mecord));
	tail->level = malloc(SKIPLIST_MAXLEVEL*sizeof(mecordLink));
	
	if (head && tail) {
		head->key = sdsNew(head_key,MAX_KEYLEN);
		head->value = NULL;
		tail->key = sdsNew(tail_key,MAX_KEYLEN);
		head->value = NULL;
	}
	
	free(head_key);
	free(tail_key);

	if (!head || !tail) {
		mecordFree(head);
		mecordFree(tail);
		return(NULL);
	} else {
		head->height = tail->height = SKIPLIST_MAXLEVEL;
		int i;		
		for (i = 0;i < SKIPLIST_MAXLEVEL;i++) {
			head->level[i].next = tail;
			head->level[i].prev = NULL;
			tail->level[i].prev = head;
			tail->level[i].next = NULL;
		}
	}
	
	if ((mt = malloc(sizeof(mable))) == NULL) {
		mecordFree(head);
		mecordFree(tail);
		return(NULL);
	}
	
	mt->head = head;
	mt->tail = tail;
	mt->cmp = (recordCmp == NULL) ? _mecordCompare : recordCmp;

	return(mt);
}

int mableFree(mable *mt)
{
	if (!mt)
		return(-1);
	mecordFree(mt->head);
	mecordFree(mt->tail);
	free(mt);

	return(0);
}

int mableInsert(mable *mt, mecord *mr)
{
	return skiplistInsert(mt,mr);
}

int mableDelete(mecord *mr)
{
	skiplistDelete(mr);

	return(0);
}

mecord *mableFind(mable *mt, const char *key, int flags)
{
	mecord *cur = mableHead(mt);
	mecord *tmp = mecordCreate(sdsNew(key,sdsLen(key)));
	int h = mecordHeight(cur) - 1;
	int cmpRes;
	
LOOKUP:
	
	if (h > 0) {
		cmpRes = mt->cmp(cur,tmp);
		if (cmpRes < 0) {
			if (nextMecord(cur,h))
				cur = nextMecord(cur,h);
			else
				cur = nextMecord(prevMecord(cur,h--),h);	
			goto LOOKUP;
		} else if (cmpRes > 0) {
			cur = nextMecord(prevMecord(cur,h--),h);
			goto LOOKUP;
		} else {
			if (flags & EQUAL) {
				tmp->key = NULL;
				mecordFree(tmp);
				return(cur);
			}
		}
	}

	/*
	 * Free the tmp mecord.
	 */
	tmp->key = NULL;
	mecordFree(tmp);	

	if (cur == mableHead(mt) || cur == mableTail(mt)) {
		return(NULL);
	} else {
		switch (flags) {
			case LESSER_MAX:
				return(prevMecord(cur,0));
			case GREATE_MIN:
				return(cur);
		}
	}
}

mecord *mecordCreate(const char *key)
{
	mecord *mr;
	if (!key)
		return(NULL);
	int height = _mecordRandomLevel();

	if (!(mr = malloc(sizeof(mecord))))
		return(NULL);
	if (!(mr->level = malloc(height*sizeof(mecordLink)))) {
		mecordFree(mr);
		return(NULL);
	}
	mr->height = height;
	mr->key = (char *)key;
	mr->value = NULL;

	return(mr);
}

int mecordSetValue(mecord *mr, char *value)
{
	mr->value = value;
	return(0);
}

int mecordFree(mecord *mr)
{
	if (!mr)
		return(-1);
	if (mr->key != mr->value) {
		sdsDel(mr->key);
		sdsDel(mr->value);
	} else {
		sdsDel(mr->value);
	}
	
	free(mr);
	return(0);
}


#undef prevMecord
#undef nextMecord
#undef mecordHeight

