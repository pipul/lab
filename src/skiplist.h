#ifndef __SKIPLIST_H_
#define __SKIPLIST_H_

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
	char *key;
	char *value;
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


#define NEXTITER 1
#define PREVITER 2

skiplistIter *skiplistIterCreate(int flags, int height);
skiplistIter *skiplistSetIter(skiplistIter *si, skipNode *sn);
skipNode *skiplistIterator(skiplistIter *si);
void skiplistIterFree(skiplistIter *si);










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


mableIter *mableIterCreate(int flags, int height);
mableIter *mableSetIter(mableIter *ti, mecord *mr);
mecord *mableIterator(mableIter *ti);
void mableIterFree(mableIter *ti);


mable *mableCreate(mecordCompare *recordCmp);
int mableFree(mable *mt);
int mableInsert(mable *mt, mecord *mr);
int mableDelete(mecord *mr);

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

mecord *mableFind(mable *mr, const char *key, int flags);



mecord *mecordCreate(const char *key);
int mecordSetValue(mecord *mr, char *value);
int mecordFree(mecord *mr);



#endif
