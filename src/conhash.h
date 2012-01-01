#ifndef __CONHASH_H_
#define __CONHASH_H_


#define color(node) ((node)->color)
#define data(node) ((node)->data)
#define key(node) ((node)->key)
#define parent(node) ((node)->parent)
#define left(node) ((node)->left)
#define right(node) ((node)->right)

#define BLACK 1
#define RED 0

typedef struct rN rbNode;
typedef struct rT rbTree;

struct rN {
	long key;
	rbNode *parent;
	rbNode *right;
	rbNode *left;
	int color;
	void *data;
};

typedef int rbNodeCompare(rbNode *, rbNode *);


struct rT {
	rbNode *root;
	rbNode NIL;
	rbNodeCompare *cmp;
};


rbTree *rbtreeCreate(rbNodeCompare *);
int rbtreeInsert(rbTree *, rbNode *);
int rbtreeDelete(rbTree *, rbNode *);

rbNode *rbtreeSearch(rbTree *, long );
rbNode *rbtreeLookup(rbTree *, long );

rbNode *rbtreeMin(rbTree *, rbNode *);
rbNode *rbtreeMax(rbTree *, rbNode *);













#endif
