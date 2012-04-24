#ifndef __ZEROQP_H_
#define __ZEROQP_H_

#include "ae.h"
#include "anet.h"

#define ZEROQP_CONTEXT_SIZE  10240
#define ZEROQP_BUFSIZE  1024

/* zeroqp_context
             ----------------------------------------------------
format:      | * | 3 | \r\n | $ | 1024 | \r\n | slice_data |....
             ----------------------------------------------------
len:(bytes)   1    1     2    1    4      2        ....  */

typedef struct {
    int32_t slice_len;
    int8_t slice_data[];
} zqp_context_slice_t;

typedef struct {
    int32_t zc_slice_c;
    zqp_context_slice_t **zc_slice_v;
    int8_t zc_buf[ZEROQP_BUFSIZE];
    int32_t zc_len, zc_pos, zc_idx;
} zqp_context_t;


zqp_context_t *zeroqp_context_new();
void zeroqp_context_free(zqp_context_t *zc);

#define ZEROQP_CONTEXT_OK 0
#define ZEROQP_CONTEXT_ERR -1
#define ZEROQP_CONTEXT_WAIT 1
int32_t zeroqp_context_parse(zqp_context_t *zc);

int32_t zeroqp_context_clean(zqp_context_t *zc);
int32_t zeroqp_context_append(zqp_context_t *zc, int8_t *s, int32_t n);



#define ZEROQP_SRVNAME_SIZE 10
#define ZEROQP_SVRADDR_SIZE 128 /* ipv4 and ipv6 */
/*
 Warning: don't read any data from the cli_fd, that is very dangerous !
 */
typedef void
 _srv_callback(int32_t cli_fd, int32_t argc, zqp_context_slice_t **argv);


typedef struct zqp_service {
    int8_t srv_name[ZEROQP_SRVNAME_SIZE];
    _srv_callback *srv_cbfunc;
    struct zqp_service *srv_next;
} zqp_service_t;

typedef struct {
    zqp_service_t *zqp_srvlist;     /* point to the service list of zqp_t */
    zqp_context_t *zqp_context;     /* Malloc for each session, save context */
} zqp_session_t;


typedef struct {
    EL *zqp_el;
    int32_t zqp_fd,zqp_svr_port;
    int8_t zqp_svr_addr[ZEROQP_SVRADDR_SIZE];
    zqp_service_t *zqp_srvlist;
} zqp_t;




zqp_t *zeroqp_init(int8_t *addr, int32_t port);
int32_t zeroqp_append_service(zqp_t *z, int8_t *srv_name, _srv_callback *cb);
int32_t zeroqp_remove_service(zqp_t *z, int8_t *srv_name);

void zeroqp_start(zqp_t *z);
void zeroqp_stop(zqp_t *z);
void zeroqp_close(zqp_t *z);







#endif
