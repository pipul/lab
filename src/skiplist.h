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

#define SKIPLIST_MAXLEVEL 32	/* Should be enough for 2^32 elements */
#define SKIPLIST_P 0.25			/* Skiplist P = 1/4 */

struct skipNodeLevel;

typedef struct {
	char *key;
	char *value;
	int height;
	skipNodeLevel level[];
} skipNode;

typedef struct {
	skipNode *prev;
	skipNode *next;
} skipNodeLevel;

typedef int skipNodeCompare(skipNode *_n1, skipNode *_n2);

typedef struct {
	skipNode *head;
	skipNode *tail;
	skipNodeCompare *cmp;
} skipList;

#define nodePrev(_n,l) (_n->level[l]->prev)
#define nodeNext(_n,l) (_n->level[l]->next)
#define nodeHeight(_n) (_n->height)

#define listHead(_l) (_l->head)
#define listTail(_l) (_l->tail)


int skipListInsert(skipList *L, skipNode *_n);
int skipListDelete(skipNode *_n);




/* The KV Core Module : memory key-value db ------->
 * 
 * Memory key-value db is base on skiplist witch described above.
 * coutune imprementation some a little succinct function interface
 *
 */

#define MAX_KEYLEN 1024

typedef skipList mdbTable;
typedef skipNode mdbRecord;
typedef int mdbRecordCompare(mdbRecord *r1, mdbRecord *r2);

mdbTable *mdbTableCreate(mdbRecordCompare *recordCmp);
int mdbTableFree(mdbTable *mt);
int mdbTableInsert(mdbTable *mt, mdbRecord *mr);
int mdbTableDelete(mdbTable *mr, mdbRecord *mr);

/* flags ----->
 * 
 * GREATER_MIN the min element of the greaterSet
 * EQUAL 
 * LESSER_MAX the max element of the lesserSet
 */

#define LESSER_MAX 1
#define EQUAL 2
#define GREATER_MIN 4

#define prevRecord(_n,l) (_n->level[l]->prev)
#define nextRecord(_n,l) (_n->level[l]->next)
#define recordLevel(_n) (_n->height)

mdbRecord *mdbTableFind(mdbTable *mr, const char *key, int flags);


mdbRecord *mdbRecordCreate(const char *key);
int mdbRecordSetValue(mdbRecord *mr, char *value);
int mdbRecordFree(mdbRecord *mr);



#endif
