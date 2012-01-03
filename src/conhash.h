#ifndef __CONHASH_H_
#define __CONHASH_H_


/*
 * The Base Datastruct : rbtree ------->
 */

#define color(node) ((node)->color)
#define value(node) ((node)->value)
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
	void *value;
};

typedef int rbNodeCompare(rbNode *, rbNode *);


struct rT {
	rbNode *root;
	rbNode NIL;
	rbNodeCompare *cmp;
};


rbTree *rbtreeCreate(rbNodeCompare *cmp);
void rbtreeDestroy(rbTree *T);
rbNode *rbnodeCreate();
void rbnodeDestroy(rbNode *z);
int rbnodeSetkey(rbNode *z, long key);
int rbnodeSetvalue(rbNode *z, void *value);

int rbtreeInsert(rbTree *T, rbNode *z);
int rbtreeDelete(rbTree *T, rbNode *z);

rbNode *rbtreeSearch(rbTree *T, long key);
rbNode *rbtreeLookup(rbTree *T, long key);

rbNode *rbtreeMin(rbTree *T, rbNode *z);
rbNode *rbtreeMax(rbTree *T, rbNode *z);




/* 
 * Replication Management ------->
 * 
 */


typedef struct {
	rbTree *svlist; /* cluster server list */
	int vnodes;		/* virtual node number */
	long (*hash)(const char *);
} cluster;

cluster *clusterCreate();
void *clusterDestroy(cluster *c);
int clusterAddServer(cluster *c, const char *serv, int replicas);
int clusterDelServer(cluster *c, const char *serv);
int clusterSetServer(cluster *c, const char *serv, int replicas);
const char *clusterGetServer(cluster *c, const char *cli);

















#endif
