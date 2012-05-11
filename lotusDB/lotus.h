#ifndef __LSM_H_
#define __LSM_H_

#include <stdint.h>


#define _PATH_MAX 1024
#define _KEY_MAX 1024
#define _LEVEL_MAX 15
#define _LEVEL_RANDOM 0
#define _BLOOM_P 0.01
#define _SKIPLIST_P 0.25
#define _BLOCK_SIZE 40960
#define _PAGE_SIZE 4096


#define __DEBUG()


typedef char * slice_t;
typedef struct {
    int32_t len;
    char data[];
} slicehdr_t;

#define _SLICE_HP(s) ((slicehdr_t *)(s - sizeof(slicehdr_t)))
#define _SLICE_HL(s) (sizeof(int32_t) + slice_len(s))

slice_t slice_new(char *src, int32_t len);
slice_t slice_null();
slice_t slice_full();
void slice_free(slice_t src);
slice_t slice_dup(slice_t src);
int32_t slice_len(slice_t src);
int slice_cmp(slice_t src, slice_t drc);







typedef struct _m_elem {
    slice_t key;
    slice_t value;
    int level;
    struct _m_elem *forward[];
} melem_t;

melem_t *melem_new(int lev, slice_t key, slice_t value);
void melem_free(melem_t *me);
int melem_cmp(melem_t *se, melem_t *de);


typedef struct _m_table {
    melem_t *header;
    time_t a_time;
    int size,maxlevel;
    int (*melem_cmp)(melem_t *se, melem_t *de);
} mut_t;


mut_t *mutable_new();
void mutable_free(mut_t *mt);
melem_t *mutable_lookup(mut_t *mt, slice_t key);
int mutable_add(mut_t *mt, melem_t *kv);
int mutable_del(mut_t *mt, melem_t *kv);
int mutable_dump(mut_t *mt, const char *sst_name);
int mutable_cat(mut_t *sm, mut_t *dm);












typedef struct _s_elem {
    uint32_t khash;
    int32_t klen,vlen;
    slice_t kptr,vptr;
    int level;
    struct _s_elem *forward[];
} selem_t;

selem_t *selem_new(int lev);
void selem_free(selem_t *se);
int selem_cmp(selem_t *se, selem_t *de);


typedef struct _s_list {
    selem_t *header;
    int size,maxlevel;
    int (*selem_cmp)(selem_t *se, selem_t *de);
} slist_t;

slist_t *slist_new();
int slist_free(slist_t *sl);
int slist_add(slist_t *sl, selem_t *se);
int slist_del(slist_t *sl, selem_t *se);
selem_t *slist_get(slist_t *sl, uint32_t kh, int *sc);

typedef struct _s_meta {
    slice_t key;
    int32_t boffset,bsize;
    slist_t *list;
    int level;
    struct _s_meta *forward[];
} smeta_t;

smeta_t *smeta_new(int lev);
int smeta_free(smeta_t *sm);
int smeta_cmp(smeta_t *sm, smeta_t *dm);

typedef struct _s_idx {
    smeta_t *header;
    int size,maxlevel;
    int (*smeta_cmp)(smeta_t *sm, smeta_t *dm);
} sidx_t;

sidx_t *sidx_new();
int sidx_free(sidx_t *si);
int sidx_add(sidx_t *si, smeta_t *sm);
int sidx_del(sidx_t *si, smeta_t *sm);
smeta_t *sidx_get(sidx_t *si, slice_t key);


typedef struct _s_bloom {
    int32_t size,mbits,hashn;
    char *s;
    uint32_t (*hash)(const char *, int len);
} sbloom_t;

sbloom_t *sbloom_new(uint32_t n, float p);
void sbloom_free(sbloom_t *sb);
int sbloom_set(sbloom_t *sb, const slice_t key);
int sbloom_get(sbloom_t *sb, const slice_t key);



typedef struct _s_stat {
    int32_t smetanum,selemnum,version,slottag,
        skoff,ekoff,timestamp,idxoff,idxsize,bloomoff,bloomsize;
} sstat_t;
#define _SST_TIMESTAMP(s) (s->stat.timestamp)

sstat_t *sstat_new();
void sstat_free(sstat_t *ss);



typedef struct _s_ {
    char sst_name[_PATH_MAX];
    int flags;
    sstat_t stat;
    void *mmaptr;
    slice_t sk,ek; /* start_key and end_key */
    sbloom_t *bloom;
    sidx_t *index;
} sst_t;

sst_t *sstable_open(const char *sst_name, int flags);
#define SST_CPONLY 0x01
#define SST_RDONLY 0x02

int sstable_close(sst_t *st);
slice_t sstable_lookup(sst_t *st, slice_t key);




#define _AOT_SUFFIX ".aot"

typedef struct {
    int fd,flags;
    char aot_name[_PATH_MAX], aot_buf[_PAGE_SIZE];
} aot_t;

aot_t *aotable_open(const char *filename, int flags, ... /* mode */);
#define AOT_OPEN  0x01
#define AOT_CREAT 0x02
#define AOT_SYNC  0x04

void aotable_close(aot_t *at);
int aotable_truncate(aot_t *at);
mut_t *aotable_recover(aot_t *at);
int aotable_append(aot_t *at, slice_t key, slice_t value);




#define _SLOTFILE_HINT (100)


typedef struct {
    int cap,size;
    sst_t **sstfiles;
} sloter_t;

sloter_t *sloter_new();
void sloter_free(sloter_t *st);
int sloter_add(sloter_t *st, sst_t *sst);
int sloter_del(sloter_t *st, sst_t *sst);
sst_t *sloter_get(sloter_t *st, const char *sst_name);




#define _SST_SUFFIX ".sst"
#define _SLOTTAG_MAX 8

typedef struct DBHANDLE {
    mut_t *memtable;
    aot_t *apponlyfile;
    char db_path[_PATH_MAX];
    int db_flags,db_state;

    sloter_t *db_slot[_SLOTTAG_MAX];

    int (*close)(struct DBHANDLE *db);
    int (*del)(const struct DBHANDLE *db, slice_t key);
    int (*put)(const struct DBHANDLE *db, slice_t key, slice_t value);
    slice_t (*get)(const struct DBHANDLE *db, slice_t key);
    int (*fflush)(struct DBHANDLE *db);

/* inter interface */
    int (*_sst_link)(struct DBHANDLE *db, char *sst_name);
    int (*_sst_unlink)(struct DBHANDLE *db, char *sst_name);
} DB;


DB *db_open(const char *path,int flags);
#define DB_SYNC 0x01

int _db_close(struct DBHANDLE *db);
int _db_del(const struct DBHANDLE *db, slice_t key);
int _db_put(const struct DBHANDLE *db, slice_t key, slice_t value);
slice_t _db_get(const struct DBHANDLE *db, slice_t key);
int _db_fflush(struct DBHANDLE *db);


int __db_sst_link(DB *db, char *sst_name);
int __db_sst_unlink(DB *db, char *sst_name);





typedef struct CPHANDLE {
    char cp_path[_PATH_MAX];
    int cp_flags,cp_state;

    sloter_t *cp_slot[_SLOTTAG_MAX];
    int (*close)(struct CPHANDLE *cp);
    sloter_t *(*pcp)(const struct CPHANDLE *cp);
    sloter_t *(*runcp)(const sloter_t *slt);

/* inter interface */
    int (*_sst_link)(struct CPHANDLE *cp, char *sst_name);
    int (*_sst_unlink)(struct CPHANDLE *cp, char *sst_name);
} CP;

CP *cp_open(const char *path);

int _cp_close(CP *cp);
sloter_t *_cp_pcp(const CP *cp);
sloter_t *_cp_runcp(const sloter_t *slt);

int __cp_sst_link(CP *cp, char *sst_name);
int __cp_sst_unlink(CP *cp, char *sst_name);




#endif
