#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "db.h"


long long _ustime()
{
	struct timeval tv;
	long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec)*1000000;
	ust += tv.tv_usec;
	return ust / 1000000;
}



int32_t _db_write_test(const int8_t *db_path, int32_t n)
{
	DB *db;
	int32_t i;
	int8_t buffer[KEY_MAX];
	sds key,value;
	int32_t start_t, end_t;
	int32_t count_o, count_e;

	if (!db_path)
		return(-1);
	if ((db = db_open(db_path)) == NULL)
		return(-1);
	count_o = count_e = 0;
	start_t = _ustime();

	fprintf(stderr,"%s: %30d\n","keys",20);
	fprintf(stderr,"%s: %28d\n","values",40);
	fprintf(stderr,"%s: %27d\n","entries",n);
	
	for (i = 0; i < n; i++) {
		snprintf(buffer,KEY_MAX,"%20d",i);
		key = sdsnew(buffer);
		snprintf(buffer,KEY_MAX,"value = %32d",i);
		value = sdsnew(buffer);
		if (0 == db->put(db,key,value))
			count_o++;
		else
			count_e++;
		if (i % 10000 == 0) {
			fprintf(stderr,"random-write finished %d ops\r",i);
			fflush(stderr);
		}
	}
	db->sync(db);
	end_t = _ustime();
	fprintf(stderr,
		"Random-Write (done:%ld cost:%lds): %.1f writes/sec\n",
		count_o,end_t - start_t,(double)count_o/(end_t - start_t + 1));
	db->close(db);
	return(0);
}

int32_t _db_read_test(const int8_t *db_path, int32_t n)
{
	DB *db;
	int32_t i;
	sds key,value;
	int8_t buffer[KEY_MAX];
	int32_t start_t, end_t;
	int32_t count_o, count_e;
	
	if (!db_path)
		return(-1);
	if ((db = db_open(db_path)) == NULL)
		return(-1);
	count_o = count_e = 0;
	start_t = _ustime();

	fprintf(stderr,"%s: %30d\n","keys",20);
	fprintf(stderr,"%s: %28d\n","values",40);
	fprintf(stderr,"%s: %27d\n","entries",n);
	
	for (i = 0; i < n; i++) {
		snprintf(buffer,KEY_MAX,"%20d",i);
		key = sdsnew(buffer);
		if ((value = db->get(db,key)) == NULL) {
			count_e++;
		} else {
			count_o++;
//			fprintf(stderr,"found key %s\r",value);
//    		fflush(stderr);
		}
		sdsdel(key);

	}
	end_t = _ustime();
	fprintf(stderr,
		"Random-Read (done:%ld cost:%lds): %.1f reads/sec\n",
		count_o,end_t - start_t,(double)count_o/(end_t - start_t + 1));
	db->close(db);
	return(0);
}


int32_t _db_delete_test(const int8_t *db_path, int32_t n)
{
	DB *db;
	int32_t i;
	sds key,value;
	int8_t buffer[KEY_MAX];
	int32_t start_t, end_t;
	int32_t count_o, count_e;
	
	if (!db_path)
		return(-1);
	if ((db = db_open(db_path)) == NULL)
		return(-1);
	count_o = count_e = 0;

	fprintf(stderr,"%s: %30d\n","keys",20);
	fprintf(stderr,"%s: %28d\n","values",40);
	fprintf(stderr,"%s: %27d\n","entries",n);

	start_t = _ustime();
	for (i = 0; i < n; i++) {
		snprintf(buffer,KEY_MAX,"%20d",i);
		key = sdsnew(buffer);
		if (0 == db->del(db,key)) {
			count_o++;
		} else {
			count_e++;
		}
		sdsdel(key);
		if (i % 10000 == 0) {
			fprintf(stderr,"random-read finished %d ops\r",i);
			fflush(stderr);
		}
	}
	end_t = _ustime();
	db->sync(db);
	fprintf(stderr,
		"Random-delete (done:%ld cost:%lds): %.1f deletes/sec\n",
		count_o,end_t - start_t,(double)count_o/(end_t - start_t + 1));
	db->close(db);
	return(0);
}


int main(int argc, char **argv)
{
	if (argc != 4) {
		fprintf(stderr,
		 "Usage: ./db-bench <database path> <write | read | delete> count\n");
		return(-1);
	}

	if (strcmp(argv[2],"read") == 0)
		_db_read_test(argv[1], atoi(argv[3]));
	if (strcmp(argv[2],"write") == 0)
		_db_write_test(argv[1], atoi(argv[3]));
	if (strcmp(argv[2],"delete") == 0)
		_db_delete_test(argv[1], atoi(argv[3]));
	return(0);
}

