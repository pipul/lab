#include "skiplist.h"
#include "sds.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*
 * Test Case for mable.
 */

int main(int argc, char **argv)
{
	int i;
	mable *mb = mableCreate(NULL);
	mableIter *mi;
	mecord *cur;
	char *str[argc];

	if ((mi = mableIterCreate(NEXTITER,0)) == NULL)
		return(-1);

	for (i = 0;i < argc;i++) {
		str[i] = sdsNew(argv[i],strlen(argv[i]));
		cur = mecordCreate(str[i]);
		// mecordSetValue(cur,ptr);
		mableInsert(mb,cur);
	}

	cur = mableIterator(mableSetIter(mi,mableHead(mb)));

	printf("After insert:\n");
	while (cur != mableTail(mb)) {
		printf("key = %s : keylen = %d \n",cur->key,sdsLen(cur->key));
		// if (cur->value)
			// printf("value = %s : vallen = %d \n",cur->value,sdsLen(cur->value));
		cur = mableIterator(mi);
	}

	printf("Found elements:\n");
	
	for (i = 0;i < argc;i++) {
		cur = mableFind(mb,str[i],LESSER_MAX);
		if (cur)
			printf("%s is found. ",cur->key);
		cur = mableFind(mb,str[i],EQUAL);
		if (cur)
			printf("and prev mecord is %s,",cur->key);
		cur = mableFind(mb,str[i],GREATE_MIN);
		if (cur)
			printf("%s is the next.\n",cur->key);
	}
	
	cur = mableIterator(mableSetIter(mi,mableHead(mb)));
	while (cur != mableTail(mb)) {
		mableDelete(cur);
		mecordFree(cur);
		cur = mableIterator(mi);
	}
	
	cur = mableIterator(mableSetIter(mi,mableHead(mb)));
	printf("After delete and free:\n");
	while (cur != mableTail(mb)) {
		printf("key = %s : keylen = %d / ",cur->key,sdsLen(cur->key));
		if (cur->value)
			printf("value = %s : vallen = %d\n",cur->value,sdsLen(cur->value));
		cur = mableIterator(mi);
	}
	
	mableIterFree(mi);
	return(0);
}
	
	
