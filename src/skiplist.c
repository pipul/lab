#include <stdio.h>
#include "skiplist.h"

#define nodePrev(_n,l) (_n->layers[l]->prev)
#define nodeNext(_n,l) (_n->layers[l]->next)

static int _layerUpdate(skipNode *_n1, uint8 l)
{
	skipNode *cur = nodePrev(_n1,0);
	
	if (!_n1)
		return(-1);
		
	while (cur->level < l) {
		cur = nodePrev(cur,0);
	}
	
	nodePrev(_n1,l) = cur;
	nodeNext(_n1,l) = nodeNext(cur,l);
	
	nodePrev(nodeNext(cur,l),l) = _n1;
	nodeNext(cur,l) = _n1;
	
	return(0);
}

int skiplistInsert(SL *L, skipNode *_n)
{
	skipNode *cur = L->head;
	uint8 i;
	
	i = cur->level-1;
	
LOOKUP:
	switch (cmp(cur,_n)) {
	
	case -1:
		if (nodeNext(cur,i))
			cur = nodeNext(cur,i);
		else
			cur = nodeNext(nodePrev(cur,i--),i);	
		goto LOOKUP;

	case 1:
		if (i>0)
			cur = nodeNext(nodePrev(cur,i--),i)
		else
			break;
	}

	nodeNext(_n,0) = cur;
	nodePrev(_n,0) = nodePrev(cur,0);
	nodeNext(nodePrev(cur,0),0) = _n;
	nodePrev(cur,0) = _n;
	
	for (i = 1 ; i < _n->level ; i++)
		_layerUpdate(_n, i);
	
	return(0);
}

int skiplistDelete(SL *L, skipNode *_n)
{
	unit8 h = 0;
	
	while (h < _n->level) {
		prevNode(nextNode(_n,h),h) = prevNode(_n,h);
		nextNode(prevNode(_n,h),h) = nextNode(_n,h);
		h++;
	}

	return(0);
}



	
	
	
	
	
	
