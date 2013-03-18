#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "skiplist.h"
#include "utils.h"


int main(int argc, char **argv)
{
	block_t *l;
	entry_t *e;
	int8_t buffer[KEY_MAX];
	sds key;
	int32_t count, cost, i;
	int32_t ok_c, err_c;

	if (argc != 2)
		return(-1);
	count = atoi(argv[1]);
	l = block_new();
	

	printf("\nBlock add Test...\n");
	ok_c = err_c = 0;
	cost = time(NULL);
	for (i = 0; i < count; i++) {
		e = entry_new();
		snprintf(buffer,KEY_MAX,"%d",i);
		e->key = sdsnew(buffer);
		snprintf(buffer,KEY_MAX,"key = %d",i);
		e->value = sdsnew(buffer);
		if (0 == block_add(l,e))
			ok_c++;
		else
			err_c++;
	}
	cost = time(NULL) - cost;
	printf("%d add ok and %d error\n",ok_c,err_c);
	printf("total cost : %d\n",cost);

	printf("\nBlock search Test...\n");	
	ok_c = err_c = 0;
	cost = time(NULL);
	for (i = 0; i < count; i++) {
		snprintf(buffer,KEY_MAX,"%d",i);
		key = sdsnew(buffer);
		if ((e = block_loup(l,key)) == NULL)
			err_c++;
		else {
			if (strstr(e->value,"0") != NULL) {
				printf("%s del ok\n",e->value);
				block_del(l,e);;
			}
			ok_c++;
		}
		sdsdel(key);
	}
	cost = time(NULL) - cost;
	printf("%d find ok and %d error\n",ok_c,err_c);
	printf("total cost : %d\n",cost);

	printf("\nBlock research Test...\n");	
	ok_c = err_c = 0;
	cost = time(NULL);
	for (i = 0; i < count; i++) {
		snprintf(buffer,KEY_MAX,"%d",i);
		key = sdsnew(buffer);
		if ((e = block_loup(l,key)) == NULL)
			err_c++;
		else {
			ok_c++;
		}
		sdsdel(key);
	}
	cost = time(NULL) - cost;
	printf("%d find ok and %d error\n",ok_c,err_c);
	printf("total cost : %d\n",cost);



	sleep(10);
	block_destroy(l);
	return(0);
}
