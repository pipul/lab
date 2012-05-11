#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lotus.h"



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


int __db_readtest(DB *db, int s, int c)
{
    int i,ok;
    slice_t value;
    char buffer[_KEY_MAX + 4] = {0},*lptr = buffer,*dptr = lptr + 4;

    ok = 0;
    for (i = s; i < s + c; i++) {
        snprintf(dptr,_KEY_MAX,"%d",i);
        *(int *)lptr = strlen(dptr);
        if (value = db->get(db,dptr))
            if (slice_len(value) != 0)
                ok++;
        if (ok % 10000) {
            fprintf(stderr,"%d will read,%d have ok\r",c,ok);
            fflush(stderr);
        }
    }
    fprintf(stderr,"%d read ok\n",ok);
    return(0);
}

int __db_writetest(DB *db, int s, int c)
{
    int i,ok;
    char buffer[_KEY_MAX + 4] = {0},*lptr = buffer,*dptr = lptr + 4;

    ok = 0;
    for (i = s; i < s + c; i++) {
        snprintf(dptr,_KEY_MAX,"%d",i);
        *(int *)lptr = strlen(dptr);
        if (0 == db->put(db,dptr,dptr))
            ok++;
        if (ok % 10000) {
            fprintf(stderr,"%d will write,%d have ok\r",c,ok);
            fflush(stderr);
        }
    }
    db->fflush(db);
    fprintf(stderr,"%d write ok\n",ok);
    return(0);
}

int __db_deletetest(DB *db, int s, int c)
{
    int i,ok;
    char buffer[_KEY_MAX + 4] = {0},*lptr = buffer,*dptr = lptr + 4;

    ok = 0;
    for (i = s; i < s + c; i++) {
        snprintf(dptr,_KEY_MAX,"%d",i);
        *(int *)lptr = strlen(dptr);
        if (0 == db->del(db,dptr))
            ok++;
        if (ok % 10000) {
            fprintf(stderr,"%d will delete,%d have ok\r",c,ok);
            fflush(stderr);
        }
    }
    db->fflush(db);
    fprintf(stderr,"%d delete ok\n",ok);
    return(0);
}

int main(int argc, char **argv)
{
    DB *db;
    int op,s,c;

    if (argc != 5)
        return(-1);
    db = db_open(argv[1],0);
    if (strcmp(argv[2],"put") == 0) {
        s = atoi(argv[3]);
        c = atoi(argv[4]);
        __db_writetest(db,s,c);
    } else if (strcmp(argv[2],"get") == 0) {
        s = atoi(argv[3]);
        c = atoi(argv[4]);
        __db_readtest(db,s,c);
    } else if (strcmp(argv[2],"del") == 0) {
        s = atoi(argv[3]);
        c = atoi(argv[4]);
        __db_deletetest(db,s,c);
    }
    db->close(db);
}
