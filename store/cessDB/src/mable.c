#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mable.h"
#include "sds.h"

/* The Base Datastruct : SkipList ------->
 *
 * A skip list is a data structure for storing a sorted list of items 
 * using a hierarchy of linked lists that connect increasingly sparse 
 * subsequences of the items. These auxiliary lists allow item lookup 
 * with efficiency comparable to balanced binary search trees (that is, 
 * with number of probes proportional to log(n) instead of n).
 */

#define prevNode(_n,h) ((_n->level+h)->prev)
#define nextNode(_n,h) ((_n->level+h)->next)
#define nodeHeight(_n) (_n->height)

#define listHead(_l) (_l->head)
#define listTail(_l) (_l->tail)

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
	int h= nodeHeight(cur) - 1, cmpRes;
	
	while (h >= 0) {
		cmpRes = sl->cmp(cur,_n);
		if (cmpRes < 0) {
			cur = nextNode(cur,h);
		} else if (cmpRes > 0) {
			if (h > 0) {
				cur = nextNode(prevNode(cur,h),h-1);
				h--;
			} else
				break;
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
	mableIter *mi;
	
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
	if ((mi = (mableIter *)malloc(sizeof(mableIter))) == NULL)
		return(NULL);
		
	mi->flags = flags;
	mi->height = height;
	mi->curNode = NULL;

	return(mi);
}

mableIter *mableSetIter(mableIter *mi, mecord *mr)
{
	if (!mi || !mr || mr->height-1 < mi->height)
		return(NULL);
	mi->curNode = mr;
	return(mi);
}

mecord *mableIterator(mableIter *mi)
{
	if (!mi || mi->curNode->height-1 < mi->height)
		return(NULL);

	switch (mi->flags) {
		case NEXTITER:
			if (nextMecord(mi->curNode,mi->height))
				mi->curNode = nextMecord(mi->curNode,mi->height);
			break;
		case PREVITER:
			if (prevMecord(mi->curNode,mi->height))
				mi->curNode = prevMecord(mi->curNode,mi->height);
			break;
		default:
			break;
	}
	
	return(mi->curNode);
}

void mableIterFree(mableIter *mi)
{
	if (!mi)
		return;
	free(mi);
}

mable *mableCreate(mecordCompare *recordCmp)
{
	mable *mb;
	mecord *head, *tail;

	sds head_key, tail_key;
	
	/*
	 * Head and Tail is two special mecord in the mable.
	 * Head is the biggest one and the Tail is the least. this
	 * two specific mecord is used to avoid overlap of insert
	 * and lookup operation.
	 */

	head_key = sdsNew(NULL,MAX_KEYLEN);
	tail_key = sdsNew(NULL,MAX_KEYLEN);
	
	if (!head_key || !tail_key)
		return(NULL);
	else
		sdsSet(tail_key,0xFFFFFFFF,MAX_KEYLEN);
	
	head = _mecordCreateByHeight(head_key,SKIPLIST_MAXLEVEL);
	tail = _mecordCreateByHeight(tail_key,SKIPLIST_MAXLEVEL);
	
	if (!head || !tail) {
		mecordFree(head);
		mecordFree(tail);
		return(NULL);
	} else {

		head->key = head_key;
		head->value = NULL;
		tail->key = tail_key;
		head->value = NULL;

		int i;		
		for (i = 0;i < SKIPLIST_MAXLEVEL;i++) {
			head->level[i].next = tail;
			head->level[i].prev = NULL;
			tail->level[i].prev = head;
			tail->level[i].next = NULL;
		}
	}
	
	if ((mb = malloc(sizeof(mable))) == NULL) {
		mecordFree(head);
		mecordFree(tail);
		return(NULL);
	}
	
	mb->head = head;
	mb->tail = tail;
	mb->cmp = (recordCmp == NULL) ? _mecordCompare : recordCmp;

	return(mb);
}

int mableFree(mable *mb)
{
	if (!mb)
		return(-1);
	mecordFree(mb->head);
	mecordFree(mb->tail);
	free(mb);

	return(0);
}

int mableInsert(mable *mb, mecord *mr)
{
	return skiplistInsert(mb,mr);
}

int mableDelete(mecord *mr)
{
	skiplistDelete(mr);
	return(0);
}

mecord *mableFind(mable *mb, const sds key, int flags)
{
	mecord *cur, *Found = NULL;
	mecord *tmp = mecordCreate(key);
	int h, cmpRes;

	if (flags & LESSER_MAX) {
		cur = mableHead(mb);
		h = mecordHeight(cur) - 1;
		while (h >= 0) {
			cmpRes = mb->cmp(cur,tmp);
			if (cmpRes < 0) {
				cur = nextMecord(cur,h);
			} else if (cmpRes >= 0) {
				if (h > 0) {
					cur = nextMecord(prevMecord(cur,h),h-1);
					h--;
				} else
					break;
			}
		}
		Found = prevMecord(cur,h);
	} else if (flags & GREATE_MIN) {
		cur = mableTail(mb);
		h = mecordHeight(cur) - 1;
		while (h >= 0) {
			cmpRes = mb->cmp(cur,tmp);
			if (cmpRes > 0) {
				cur = prevMecord(cur,h);
			} else if (cmpRes <= 0) {
				if (h > 0) {
					cur = prevMecord(nextMecord(cur,h),h-1);
					h--;
				} else
					break;
			}
		}
		Found = nextMecord(cur,h);	
	} else if (flags & EQUAL) {
		cur = mableHead(mb);
		h = mecordHeight(cur) - 1;
		while (h >= 0) {
			cmpRes = mb->cmp(cur,tmp);
			if (cmpRes < 0) {
				cur = nextMecord(cur,h);
			} else if (cmpRes > 0) {
				cur = nextMecord(prevMecord(cur,h),h-1);
				h--;
			} else {
				Found = cur;
				break;
			}
		}
	}

	/*
	 * Free the tmp mecord.
	 */
	tmp->key = NULL;
	mecordFree(tmp);	

	return(Found);
}

mecord *_mecordCreateByHeight(const sds key, int height)
{
	mecord *mr;
	if (!key)
		return(NULL);
	int h = (height > 1) ? height : 1;

	if (!(mr = malloc(sizeof(mecord))))
		return(NULL);
	if (!(mr->level = malloc(h*sizeof(mecordLink)))) {
		mecordFree(mr);
		return(NULL);
	}
	mr->height = h;
	mr->key = (sds)key;
	mr->value = NULL;

	return(mr);
}

mecord *mecordCreate(const sds key)
{
	int height = _mecordRandomLevel();
	return _mecordCreateByHeight(key,height);
}

int mecordSetValue(mecord *mr, sds value)
{
	mr->value = value;
	return(0);
}

int mecordFree(mecord *mr)
{
	if (!mr)
		return(-1);
	free(mr);
	return(0);
}


#undef prevMecord
#undef nextMecord
#undef mecordHeight

