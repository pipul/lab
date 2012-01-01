#ifndef __CONHASH_H_
#define __CONHASH_H_


/*
 * The Base Datastruct : rbtree ------->
 */

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
void rbtreeDestroy(rbTree *);
rbNode *rbnodeCreate();
void rbnodeDestroy(rbNode *);
int rbnodeSetkey(rbNode *, long );
int rbnodeSetdata(rbNode *, void *);

int rbtreeInsert(rbTree *, rbNode *);
int rbtreeDelete(rbTree *, rbNode *);

rbNode *rbtreeSearch(rbTree *, long );
rbNode *rbtreeLookup(rbTree *, long );

rbNode *rbtreeMin(rbTree *, rbNode *);
rbNode *rbtreeMax(rbTree *, rbNode *);


/*
 * The Default Hash algorithm: md5 -------->
 *
 */

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
    md5_word_t count[2];	/* message length in bits, lsw first */
    md5_word_t abcd[4];		/* digest buffer */
    md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

/* Initialize the algorithm. */

void md5_init(md5_state_t *pms);

/* Append a string to the message. */

void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);

/* Finish the message and return the digest. */

void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);

/* 
 * the default hash function, using md5 algorithm
 */
long _conDefaultHash(const char *instr);




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
void *clusterDestroy(cluster *);
int clusterAddServer(cluster *, const char *, int );
int clusterDelServer(cluster *, const char *);
const char *clusterGetServer(cluster *, const char *);


















#endif
