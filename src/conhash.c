#include "conhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The Base Datastruct : rbtree ------->
 *
 * A red–black tree is a type of self-balancing binary search tree, a data 
 * structure used in computer science, typically to implement associative 
 * arrays. The original structure was invented in 1972 by Rudolf Bayer and 
 * named "symmetric binary B-tree," but acquired its modern name in a paper 
 * in 1978 by Leonidas J. Guibas and Robert Sedgewick. It is complex, but 
 * has good worst-case running time for its operations and is efficient in 
 * practice: it can search, insert, and delete in O(log n) time, where n is 
 * the total number of elements in the tree. Put very simply, a red–black 
 * tree is a binary search tree that inserts and deletes in such a way that 
 * the tree is always reasonably balanced.
 *
 */

#define _NULL(s) (&((s)->NIL))

static int _rbNodeDefaultCompareFunction(rbNode *n1, rbNode *n2)
{
	if (key(n1) < key(n2))
		return(-1);
	else if (key(n1) > key(n2))
		return(1);
	else
		return(0);
}

static int _rbtreeRotateLeft(rbTree *T, rbNode *x)
{
	rbNode *y = right(x);
	right(x) = left(y);
	
	if (left(y) != _NULL(T))
		parent(left(y)) = x;
	
	parent(y) = parent(x);
	if (parent(x) == _NULL(T)) {
		T->root = y;
	} else if (x == left(parent(x))) {
		left(parent(x)) = y;
	} else {
		right(parent(x)) = y;
	}
	
	left(y) = x;
	parent(x) = y;
	return(0);
}

static int _rbtreeRotateRight(rbTree *T, rbNode *x)
{
	rbNode *y = left(x);
	left(x) = right(y);
	
	if (right(y) != _NULL(T))
		parent(right(y)) = x;
	
	parent(y) = parent(x);
	if (parent(x) == _NULL(T)) {
		T->root = y;
	} else if (x == left(parent(x))) {
		left(parent(x)) = y;
	} else {
		right(parent(x)) = y;
	}
	
	right(y) = x;
	parent(x) = y;
	
	return(0);
}


rbNode* rbtreeMin(rbTree *T, rbNode *sT)
{
	while(sT->left != _NULL(T)){
		sT = sT->left;
	}

	return(sT);
}

rbNode* rbtreeMax(rbTree *T, rbNode *sT)
{
	while(sT->right != _NULL(T)){
		sT = sT->right;
	}

	return(sT);
}


static rbNode *_rbtreeSuccessor(rbTree *T, rbNode *x)
{
	rbNode *y;
	
	if (right(x) != _NULL(T)) {
		y = rbtreeMin(T,right(x));
	} else {
		y = parent(x);
		while ((y != _NULL(T)) && (x == right(y))) {
			x = y;
			y = parent(y);
		}
	}
	
	return(y);
}

static rbNode *_rbtreePredecessor(rbTree *T,rbNode *x)
{
	rbNode *y;
	
	if (left(x) != _NULL(T)) {
		y = rbtreeMax(T,left(x));
	} else {
		y = parent(x);
		while ((y != _NULL(T)) && (x == left(y))) {
			x = y;
			y = parent(y);
		}
	}
	
	return(y);
}

static int _rbtreeInsertFixup(rbTree *T, rbNode *x)
{
	rbNode *y;
	while (color(parent(x)) == RED) {
		if (parent(x) == left(parent(parent(x)))) {
			y = right(parent(parent(x)));
			if (color(y) == RED) {
				color(parent(x)) = BLACK;
				color(y) = BLACK;
				color(parent(parent(x))) = RED;
				x = parent(parent(x));
			} else {
				if (x == right(parent(x))) {
					x = parent(x);
					_rbtreeRotateLeft(T,x);
				} else {
					color(parent(x)) = BLACK;
					color(parent(parent(x))) = RED;
					_rbtreeRotateRight(T,parent(parent(x)));
				}
			}
		} else {
			y = left(parent(parent(x)));
			if (color(y) == RED) {
				color(parent(x)) = BLACK;
				color(y) = BLACK;
				color(parent(parent(x))) = RED;
				x = parent(parent(x));
			} else {
				if (x == left(parent(x))) {
					x = parent(x);
					_rbtreeRotateRight(T,x);
				} else {
					color(parent(x)) = BLACK;
					color(parent(parent(x))) = RED;
					_rbtreeRotateLeft(T,parent(parent(x)));
				}
			}
		}
	}
		
	color(T->root) = BLACK;
	return(0);
}
		

static int _rbtreeDeleteFixup(rbTree *T, rbNode *x)		
{
	rbNode *w;

	while (x != _NULL(T) && color(x) == BLACK) {
		if (x == left(parent(x))) {
			w = right(parent(x));
			if (color(w) == RED) {
				color(w) = BLACK;
				color(parent(x)) = RED;
				_rbtreeRotateLeft(T,parent(x));
				w = right(parent(x));
			}
			if (color(left(w)) == BLACK && color(right(w)) == BLACK) {
				color(w) = RED;
				w = right(parent(x));
			} else if (color(right(w)) == BLACK) {
				color(left(w)) = BLACK;
				color(w) = RED;
				_rbtreeRotateRight(T,w);
				w = right(parent(x));
			} else {
				color(w) = color(parent(x));
				color(parent(x)) = BLACK;
				color(right(w)) = BLACK;
				_rbtreeRotateLeft(T,parent(x));
				x=T->root;
			}
		} else {
			w = left(parent(x));
			if (color(w) == RED) {
				color(w) = BLACK;
				color(parent(x)) = RED;
				_rbtreeRotateRight(T,parent(x));
				w = left(parent(x));
			}
			if (color(right(w)) == BLACK && color(left(w)) == BLACK){
				color(w) = RED;
				w = left(parent(x));
			} else if (color(left(w)) == BLACK) {
				color(right(w)) = BLACK;
				color(w) = RED;
				_rbtreeRotateLeft(T,w);
				w = left(parent(x));
			} else {
				color(w) = color(parent(x));
				color(parent(x)) = BLACK;
				color(left(w)) = BLACK;
				_rbtreeRotateRight(T,parent(x));
				x = T->root;
			}
		}
	}

	color(x) = BLACK;

	return(0);
}

int rbtreeInsert(rbTree *T, rbNode *z)
{
	rbNode *cur,*curParent;
	int cmpRes;
	
	if (T == NULL || z == NULL || z == _NULL(T)) {
		return(-1);
	} else {
		curParent = _NULL(T);
		cur = T->root;
	}
	
	while (cur != _NULL(T)) {
		curParent = cur;
		cmpRes = T->cmp(z,cur);
		if (cmpRes < 0)
			cur = left(cur);
		else
			cur = right(cur);
	}

	parent(z) = curParent;
	cmpRes = T->cmp(z,curParent);
	
	if (curParent == _NULL(T)) {
		T->root = z;
	} else if (cmpRes < 0) {
		left(curParent) = z;
	} else {
		right(curParent) = z;
	}
	
	left(z) = right(z) = _NULL(T);
	color(z) = RED;
	
	_rbtreeInsertFixup(T,z);
	
	return(0);
}


int rbtreeDelete(rbTree *T, rbNode *z)
{
	rbNode *x,*y;

	if (T == NULL || z == NULL || z == _NULL(T)) {
		return(-1);
	}

	if (left(z) == _NULL(T) || right(z) == _NULL(T))
		y=z;
	else
		y=_rbtreeSuccessor(T,z);

	if (left(z) != _NULL(T))
		x=left(y);
	else
		x=right(y);

	parent(x)=parent(y);
	if (parent(y) == _NULL(T)) {
		T->root=x;
	} else if (y == left(parent(y))) {
		left(parent(y))=x;
	} else {
		right(parent(y))=x;
	}
	
	if (y != z){
		key(z) = key(y);
		data(z) = data(y);
	}

	if (color(y) == BLACK)
		_rbtreeDeleteFixup(T,x);

	return(0);
}

rbTree *rbtreeCreate(rbNodeCompare *cmp)
{
	rbTree *T;
	if ((T = malloc(sizeof(rbTree))) == NULL)
		return(NULL);
	T->root = _NULL(T);
	color(_NULL(T)) = BLACK;
	if (cmp != NULL) {
		T->cmp = cmp;	
	}else{
		T->cmp = _rbNodeDefaultCompareFunction;
	}

	return(T);
}

rbNode *rbtreeSearch(rbTree *T, long k)
{
	rbNode *x = T->root;
	rbNode tmp;
	int cmpRes;
	
	tmp.key = k;

	while (x != _NULL(T)) {
		cmpRes = T->cmp(x,&tmp);
		if (cmpRes < 0) {
			x = right(x);
		} else if (cmpRes > 0){
			x = left(x);
		} else {
			break;
		}
	}

	if (x != _NULL(T))
		return(x);
	else
		return(NULL);
}

rbNode *rbtreeLookup(rbTree *T, long k)
{
	rbNode *x=T->root,*y;
	rbNode tmp;
	int cmpRes;
	
	tmp.key = k;
	
	while (x != _NULL(T)) {
		y = x;
		cmpRes = T->cmp(x,&tmp);
		if (cmpRes < 0) {
			x = right(x);
		} else if (cmpRes > 0){
			x = left(x);
		} else {
			break;
		}
	}

	if (x != _NULL(T))
		return(x);
	else if (y != _NULL(T))
		return(y);
	else
		return(NULL);
}


/*
 * rbTree test main ---------->
 *

int main(int argc, char **argv)
{
	rbTree *T = rbtreeCreate(NULL);
	int i;
	rbNode n[9],*x;
	
	n[0].key=11;
	n[1].key=15;
	n[2].key=2;
	n[3].key=7;
	n[4].key=1;
	n[5].key=14;
	n[6].key=5;
	n[7].key=8;
	n[8].key=4;

	for(i=0;i<9;i++){
		rbtreeInsert(T,&n[i]);
	}
	x=T->root;
	while(x!=_NULL(T)){
		printf("%ld,%d\n",x->key,x->color);
		x=_rbtreeSuccessor(T,x);
	}
	x=T->root;
	while(x!=_NULL(T)){
		printf("%ld,%d\n",x->key,x->color);
		x=_rbtreePredecessor(T,x);
	}

	rbtreeDelete(T,&n[3]);
	printf("\n");

	x=T->root;
	while(x!=_NULL(T)){
		printf("%ld,%d\n",x->key,x->color);
		x=_rbtreeSuccessor(T,x);
	}
	x=T->root;
	while(x!=_NULL(T)){
		printf("%ld,%d\n",x->key,x->color);
		x=_rbtreePredecessor(T,x);
	}
	return(0);
}

*/
