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

static int _rbtreeDeleteFixup(rbTree *, rbNode *);

static int _rbtreeRotateLeft(rbTree *T, rbNode *x)
{
	rbNode *y = right(x);
	right(x) = left(y);
	
	if (left(y) != _NULL(T))
		parent(left(y)) = x;
	
	parent(y) = parent(x);
	if (parent(x) == _NULL(T)) {
		T->root = y;
	} else if (x == left(parent(x)) {
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
					rbtreeRotateLeft(T,x);
				} else {
					color(parent(x)) = BLACK;
					color(parent(parent(x))) = RED;
					rbtreeRotateRight(T,parent(parent(x)));
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
					rbtreeRotateRight(T,x);
				} else {
					color(parent(x)) = BLACK;
					color(parent(parent(x))) = RED;
					rbtreeRotateLeft(T,parent(parent(x)));
				}
			}
		}
	}
		
	color(T->root) = BLACK;
	return(0);
}
		
		
		
		
		

