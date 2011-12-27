#ifndef __MABLE_H_
#define __MABLE_H_

#include "sds.h"

/* The Base Datastruct : SkipList ------->
 *
 * A skip list is a data structure for storing a sorted list of items 
 * using a hierarchy of linked lists that connect increasingly sparse 
 * subsequences of the items. These auxiliary lists allow item lookup 
 * with efficiency comparable to balanced binary search trees (that is, 
 * with number of probes proportional to log(n) instead of n).
 */

/* Should be enough for 2^32 elements */
#define SKIPLIST_MAXLEVEL 32

/* Skiplist P = 1/4 */
#define SKIPLIST_P 0.25

typedef struct sN skipNode;
typedef struct sNL skipNodeLink;
typedef struct sL skipList;
typedef struct slI skiplistIter;

struct sN {
	sds key;
	sds value;
	int height;
	skipNodeLink *level;
};

struct sNL {
	skipNode *prev;
	skipNode *next;
};

struct slI {
	int flags;
	int height;
	skipNode *curNode;
};

typedef int skipNodeCompare(skipNode *_n1, skipNode *_n2);

struct sL {
	skipNode *head;
	skipNode *tail;
	skipNodeCompare *cmp;
};

int skiplistInsert(skipList *sl, skipNode *_n);
int skiplistDelete(skipNode *_n);











/* The KV Core Module : memory key-value db ------->
 * 
 * Memory key-value db is base on skiplist witch described above.
 * coutune imprementation some a little succinct function interface
 *
 */

#define MAX_KEYLEN 1024
#define MAX_VALUELEN 2147483647 /* limit to sdsLen structure : 2^31-1 */

typedef skipList mable;
typedef skipNode mecord;
typedef skipNodeLink mecordLink;
typedef skiplistIter mableIter;
typedef int mecordCompare(mecord *r1, mecord *r2);


/* Mable internal function interface. */
mecord *_mecordCreateByHeight(const sds key, int height);


#define NEXTITER 1
#define PREVITER 2

mableIter *mableIterCreate(int flags, int height);
mableIter *mableSetIter(mableIter *ti, mecord *mr);
mecord *mableIterator(mableIter *ti);
void mableIterFree(mableIter *ti);


/* flags ----->
 * 
 * GREATER_MIN the min element of the greaterSet
 * EQUAL 
 * LESSER_MAX the max element of the lesserSet
 */

#define LESSER_MAX 1
#define EQUAL 2
#define GREATE_MIN 4

#define mableHead(_l) (_l->head)
#define mableTail(_l) (_l->tail)

mecord *mableFind(mable *mr, const sds key, int flags);
mable *mableCreate(mecordCompare *recordCmp);
int mableFree(mable *mb);
int mableInsert(mable *mb, mecord *mr);
int mableDelete(mecord *mr);




mecord *mecordCreate(const sds key);
int mecordSetValue(mecord *mr, sds value);
int mecordFree(mecord *mr);



#endif
