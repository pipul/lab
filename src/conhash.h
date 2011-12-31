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

#define _rbtreeNodeClear(s) do {\
	node->left = NULL;\
	node->right = NULL;\
	node->parent = NULL;\
} while (0);

#define _rbtreeIsEmpty(s) ((s)->root == &(s->NIL))

typedef struct rbTree rbTree;
typedef struct rbNode rbNode;
typedef int rbNodeCompare(rbNode *n1, rbNode *n2);

struct rbNode {
	long key;
	rbNode *parent;
	rbNode *right;
	rbNode *left;
	int color;
	void *data;
}

struct rbTree {
	rbNode *root;
	rbNode NIL;
	size_t size;
	rbNodeCompare *cmp;
}


int rbtreeCreate(rbNodeCompare *);
int rbtreeInsert(rbTree *, rbNode *);
int rbtreeDelete(rbTree *, rbNode *);

rbNode *rbtreeSearch(rbTree *, long );
rbNode *rbtreeLookup(rbTree *, long );

rbNode *rbtreeMin(rbTree *, rbNode *);
rbNode *rbtreeMax(rbTree *, rbNode *);













#endif
