#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "skiplist.h"
#include "utils.h"


double _ustime(void)
{
	struct timeval tv;
	long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec)*1000000;
	ust += tv.tv_usec;
	return ust;
}

void _random_str(char *key,int length) {
    int i;
	char salt[36]= "abcdefghijklmnopqrstuvwxyz0123456789";
	memset(key, 0, length);
	for (i = 0; i < length; i++)
		key[i] = salt[rand() % length];
}

int main(int argc, char **argv)
{
    block_t *list;
	entry_t *node;
	int8_t buffer[KEY_MAX];
	sds key;
	int32_t count, i;
	double cost;
	FILE *logfp;

	if (argc != 3)
        return(-1);
	count = atoi(argv[1]);
	if ((logfp = fopen(argv[2],"w")) == NULL)
        return(-1);
	list = block_new();

	cost = _ustime();
	for (i = 0; i < count; i++) {
        node = entry_new();
	    _random_str(buffer,20);
	    node->key = sdsnew(buffer);
	    _random_str(buffer,80);
	    node->value = sdsnew(buffer);
	    block_add(list,node);
	    if (i % 10000 == 0) {
	        cost = _ustime() - cost;
	        fprintf(logfp,"%d %10.4f\n",i,cost/10000);
	        cost = _ustime();
	    }
	}
	block_destroy(list);
	fclose(logfp);
	exit(0);
}
