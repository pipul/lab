#ifndef __DB_H_
#define __DB_H_


/*
 * Internal index file constants.
 * These are used to construct records in the
 * index file and data file.
 */

#define IDXLEN_SZ	   4	/* index record length (ASCII chars) */
#define SEP         ':'	/* separator char in index record */
#define SPACE       ' '	/* space character */
#define NEWLINE     '\n'	/* newline character */

/*
 * The following definitions are for hash chains and free
 * list chain in the index file.
 */
#define PTR_SZ        6	/* size of ptr field in hash chain */
#define PTR_MAX  999999	/* max file offset = 10**PTR_SZ - 1 */
#define NHASH_DEF	 137	/* default hash table size */
#define FREE_OFF      0	/* free list offset in index file */
#define HASH_OFF PTR_SZ	/* hash table offset in index file */



/*
 * Implementation limits.
 */
#define IDXLEN_MIN	   6	/* key, sep, start, sep, length, \n */
#define IDXLEN_MAX	1024	/* arbitrary */
#define DATLEN_MIN	   2	/* data byte, newline */
#define DATLEN_MAX	1024	/* arbitrary */



typedef unsigned long	DBHASH;	/* hash values */

/*
 * Library's private representation of the database.
 */
typedef struct {
	int    idxfd; 		/* fd for index file */
	int    datfd; 		/* fd for data file */
	char  *idxbuf; 		/* malloc'ed buffer for index record */
	char  *datbuf; 		/* malloc'ed buffer for data record*/
	char  *name; 		/* name db was opened under */

	off_t  idxoff; 		/* offset in index file of index record */
			    		/* key is at (idxoff + PTR_SZ + IDXLEN_SZ) */
	size_t idxlen; 		/* length of index record */
			      		/* excludes IDXLEN_SZ bytes at front of record */
			      		/* includes newline at end of index record */
	off_t  datoff; 		/* offset in data file of data record */
	size_t datlen; 		/* length of data record */
			      		/* includes newline at end */
	off_t  ptrval; 		/* contents of chain-ptr in index record */
	off_t  ptroff; 		/* chain-ptr offset pointing to this idx record */
	off_t  chainoff; 	/* offset of hash chain for this index record */
	off_t  hashoff;  	/* offset in index file of hash table */
	DBHASH nhash;    	/* current hash table size */
} DB;



DB *db_open(const char *, int, ...);
void db_close(DB *);
char *db_fetch(DB *, const char *);


#endif /* __DB_H_ */
