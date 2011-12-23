#ifndef __SKIPLIST_H_
#define __SKIPLIST_H_

#define DEFAULTHEIGHT 5

typedef struct skipNode {
	sds *key;
	sds *value;
	int height;
	skipNodeLevel level[];
};

struct skipNodeLevel {
	skipNode *prev;
	skipNode *next;
};

typedef int skipNodeCompare(skipNode *_n1, skipNode *_n2);

typedef struct skipList {
	skipNode *head;
	skipNode *tail;
	skipNodeCompare *cmp;
} SL;

int skipListInsert(SL *L, skipNode *_n);
int skipListDelete(SL *L, skipNode *_n);




#endif
