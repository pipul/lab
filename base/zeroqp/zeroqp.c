#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zeroqp.h"

zqp_context_t *zeroqp_context_new()
{
    zqp_context_t *zc;
    
    zc = malloc(sizeof(zqp_context_t));
    if (zc)
        memset(zc,0,sizeof(zqp_context_t));
    return(zc);
}

void zeroqp_context_free(zqp_context_t *zc)
{
    if (!zc)
        return;
    while (zc->zc_idx) {
        free(zc->zc_slice_v[zc->zc_idx - 1]);
        zc->zc_idx--;
    }
    free(zc->zc_slice_v);
    free(zc);
}

int32_t zeroqp_context_append(zqp_context_t *zc, int8_t *s, int32_t n)
{
    if (!zc || !s || n == 0)
        return(-1);
    if ((zc->zc_len + n) > ZEROQP_CONTEXT_SIZE)
        return(-1);
    memcpy(zc->zc_buf + zc->zc_len, s, n);
    zc->zc_len += n;
    return(0);
}


typedef enum {S0,S1,S2,S3,S4,UNKNOW} zeroqp_dfastate_t;

int32_t zeroqp_context_parse(zqp_context_t *zc)
{
    int8_t c;
    int32_t zc_pos = zc->zc_pos;
    int32_t zc_slice_len;
    zeroqp_dfastate_t zc_state = S0;
    int8_t sb[ZEROQP_CONTEXT_SIZE] = {0}, *_sb = sb;

    if (!zc)
        return(ZEROQP_CONTEXT_ERR);

    while (!zc->zc_slice_c && zc_pos <= zc->zc_len) {
        switch (zc_state) {
            case S0 :
                c = zc->zc_buf[zc_pos++];
                zc_state = (c == '*') ? S1 : UNKNOW;
                break;
            case S1 :
                c = zc->zc_buf[zc_pos++];
                if (c <= '9' && c >= '0') {
                    zc_state = S2;
                    *_sb++ = c;
                } else
                    zc_state = UNKNOW;
                break;
            case S2 :
                c = zc->zc_buf[zc_pos++];
                zc_state = (c == '\r') ? S3 : UNKNOW; break;
            case S3 :
                c = zc->zc_buf[zc_pos++];
                zc_state = (c == '\n') ? S4 : UNKNOW; break;
            case S4 :
                *_sb = '\0';
                if ((zc->zc_slice_c = atoi(sb)) < 0)
                    return(ZEROQP_CONTEXT_ERR);
                zc->zc_pos = zc_pos;
                if ((zc->zc_slice_v =
                 malloc(zc->zc_slice_c * sizeof(zqp_context_slice_t **))) == NULL)
                    return(ZEROQP_CONTEXT_ERR);
                /* initial all the env arguments */
                _sb = sb;
                zc_state = S0;
                break;
            case UNKNOW:
                return(ZEROQP_CONTEXT_ERR);
        }
    }

    while (zc->zc_slice_c && zc_pos <= zc->zc_len) {
        switch (zc_state) {
            case S0 :
                c = zc->zc_buf[zc_pos++];
                zc_state = (c == '$') ? S1 : UNKNOW;
                break;
            case S1 :
                c = zc->zc_buf[zc_pos++];
                if (c <= '9' && c >= '0') {
                    zc_state = S2;
                    *_sb++ = c;
                } else
                    zc_state = UNKNOW;
                break;
            case S2 :
                c = zc->zc_buf[zc_pos++];
                if (c <= '9' && c >= '0') {
                    zc_state = S2;
                    *_sb++ = c;
                } else if (c == '\r')
                    zc_state = S3;
                else
                    zc_state = UNKNOW; break;
            case S3 :
                c = zc->zc_buf[zc_pos++];
                zc_state = (c == '\n') ? S4 : UNKNOW; break;
            case S4 :
                *_sb = '\0';
                if ((zc_slice_len = atoi(sb)) < 0)
                    return(ZEROQP_CONTEXT_ERR);
                if (zc_slice_len + zc_pos + 2 > zc->zc_len)
                    return(ZEROQP_CONTEXT_WAIT);
                if ((zc->zc_slice_v[zc->zc_idx] =
                 malloc(sizeof(zqp_context_slice_t) + zc_slice_len + 1)) == NULL)
                    return(ZEROQP_CONTEXT_ERR);
                memset(zc->zc_slice_v[zc->zc_idx],0,sizeof(zqp_context_slice_t) + zc_slice_len + 1);
                zc->zc_slice_v[zc->zc_idx]->slice_len = zc_slice_len;
                zc->zc_pos = zc_pos;
                memcpy(zc->zc_slice_v[zc->zc_idx]->slice_data,zc->zc_buf + zc->zc_pos,zc_slice_len);
                zc_pos = zc->zc_pos += zc_slice_len + 2;     /* \r\n  */
                zc->zc_idx++;

                /* initial all the env arguments */
                _sb = sb;
                zc_state = S0;
                break;
            case UNKNOW:
                return(ZEROQP_CONTEXT_ERR);
        }
        if (zc->zc_idx == zc->zc_slice_c)
            return(ZEROQP_CONTEXT_OK);
    }        
    return(ZEROQP_CONTEXT_WAIT);
}

int32_t zeroqp_context_clean(zqp_context_t *zc)
{
    int32_t i;

    if (!zc)
        return(-1);
    while (zc->zc_idx) {
        free(zc->zc_slice_v[zc->zc_idx - 1]);
        zc->zc_idx--;
    }
    free(zc->zc_slice_v);
    zc->zc_slice_v = NULL;
    for (i = 0; i < zc->zc_len - zc->zc_pos; i++)
        zc->zc_buf[i] = zc->zc_buf[i + zc->zc_pos];
    zc->zc_len -= zc->zc_pos;
    zc->zc_slice_c = zc->zc_pos = 0;
}



static zqp_service_t
 *zeroqp_service_new(int8_t *srv_name, _srv_callback *cb)
{
    zqp_service_t *zs;
    
    if (!srv_name || !cb)
        return(NULL);
    zs = malloc(sizeof(zqp_service_t));
    if (zs) {
        memset(zs,0,sizeof(zqp_service_t));
        snprintf(zs->srv_name,ZEROQP_SRVNAME_SIZE,"%s",srv_name);
        zs->srv_cbfunc = cb;
    }
    return(zs);
}

static void zeroqp_service_free(zqp_service_t *zs)
{
    free(zs);
}

int32_t zeroqp_append_service(zqp_t *z, int8_t *srv_name, _srv_callback *cb)
{
    zqp_service_t *zs_cur, *zs_new;

    if (!z || !srv_name || !cb)
        return(-1);
    zs_cur = z->zqp_srvlist;
    if ((zs_new = zeroqp_service_new(srv_name,cb)) == NULL)
        return(-1);
    while (zs_cur && zs_cur->srv_next) {
        if (strcmp(zs_cur->srv_next->srv_name,zs_new->srv_name) > 0)
            break;
        zs_cur = zs_cur->srv_next;
    }
    if (!zs_cur)
        z->zqp_srvlist = zs_new;
    else {
        zs_new->srv_next = zs_cur->srv_next;
        zs_cur->srv_next = zs_new;
    }
    return(0);
}

int32_t zeroqp_remove_service(zqp_t *z, int8_t *srv_name)
{
    zqp_service_t *zs_cur, *zs_old;

    if (!srv_name)
        return(-1);
    zs_cur = z->zqp_srvlist;
    while (zs_cur && zs_cur->srv_next) {
        if (strcmp(zs_cur->srv_next->srv_name,srv_name) == 0)
            break;
        zs_cur = zs_cur->srv_next;
    }
    if (zs_cur) {
        zs_old = zs_cur->srv_next;
        zs_cur->srv_next = zs_old->srv_next;
    }
    zeroqp_service_free(zs_old);
    return(0);
}



zqp_t *zeroqp_init(int8_t *addr, int32_t port)
{
    zqp_t *z;
    int8_t zqp_svr_port[ZEROQP_BUFSIZE] = {0};

    if (!addr || port < 0)
        return(NULL);
    if ((z = malloc(sizeof(zqp_t))) == NULL)
        return(NULL);
    memset(z,0,sizeof(zqp_t));
    z->zqp_svr_port = port;
    snprintf(z->zqp_svr_addr,ZEROQP_SVRADDR_SIZE,"%s",addr);
    snprintf(zqp_svr_port,ZEROQP_BUFSIZE,"%d",port);
    if ((z->zqp_fd = tcp_listen(z->zqp_svr_addr,zqp_svr_port,NULL)) < 0) {
        free(z);
        return(NULL);
    }
    if ((z->zqp_el = el_open()) == NULL) {
        close(z->zqp_fd);
        free(z);
        return(NULL);
    }
    return(z);
}



static zqp_session_t *zeroqp_session_new()
{
    zqp_session_t *ze;
    
    ze = malloc(sizeof(zqp_session_t));
    if (ze)
        memset(ze,0,sizeof(zqp_session_t));
    if (ze) {
        if ((ze->zqp_context = zeroqp_context_new()) == NULL) {
            free(ze);
            ze = NULL;
        }
    }
    return(ze);
}

static void zeroqp_session_free(zqp_session_t *ze)
{
    zeroqp_context_free(ze->zqp_context);
    free(ze);
}


static void
 zeroqp_handle_exec(EL *el, int32_t clifd, void *argv, int32_t mask)
{
    int32_t nread,state;
    int8_t buf[ZEROQP_CONTEXT_SIZE] = {0};
    zqp_service_t *zs;
    zqp_session_t *ze;
    zqp_context_t *zc;

    if (!el || clifd < 0 || !argv || !(mask & AE_READABLE))
        return;

    ze = (zqp_session_t *)argv;
    zs = ze->zqp_srvlist;
    zc = ze->zqp_context;

    nread = read(clifd,buf,ZEROQP_CONTEXT_SIZE);
    if (nread <= 0) {
        zeroqp_session_free(ze);
        el_delEvent(el,clifd,AE_READABLE);
        close(clifd);
    } else {
        zeroqp_context_append(zc,buf,nread);
        state = zeroqp_context_parse(zc);
        if (ZEROQP_CONTEXT_OK == state) {
            while (zs) {
                if (strcmp(zs->srv_name,zc->zc_slice_v[0]->slice_data) == 0)
                    break;
                zs = zs->srv_next;
            }
            if (zs)
                zs->srv_cbfunc(clifd,zc->zc_slice_c - 1, &zc->zc_slice_v[1]);
            zeroqp_context_clean(ze->zqp_context);
    /* -- Valrgind test.
            zeroqp_session_free(ze);
            el_delEvent(el,clifd,AE_READABLE);
            close(clifd);
     */
        } else if (ZEROQP_CONTEXT_ERR == state) {
            zeroqp_session_free(ze);
            el_delEvent(el,clifd,AE_READABLE);
            close(clifd);
        }
    }
}

static void
 zeroqp_handle_accept(EL *el, int32_t listenfd, void *argv, int32_t mask)
{
    int32_t cli_fd;
    int8_t cli_ip[ZEROQP_BUFSIZE];
    int32_t cli_port;
    zqp_session_t *ze;

    if (!el || listenfd < 0)
        return;
    if ((ze = zeroqp_session_new()) == NULL)
        return;
    if ((cli_fd = tcp_accept(listenfd,cli_ip,&cli_port)) < 0) {
        zeroqp_session_free(ze);
        return;
    }
    ze->zqp_srvlist = ((zqp_t *)argv)->zqp_srvlist;
    fprintf(stderr,"client:%s connected from %d\n",cli_ip,cli_port);
    el_addEvent(el,cli_fd,AE_READABLE,zeroqp_handle_exec,ze);
}


void zeroqp_start(zqp_t *z)
{
    el_addEvent(z->zqp_el,z->zqp_fd,AE_READABLE,zeroqp_handle_accept,z);
    el_start(z->zqp_el,AE_NO_BLOCK);
}

void zeroqp_stop(zqp_t *z)
{
    el_delEvent(z->zqp_el,z->zqp_fd,AE_READABLE);
    el_stop(z->zqp_el);
}

void zeroqp_close(zqp_t *z)
{
    zqp_service_t *zs_cur;
    
    if (!z)
        return;
    while (z->zqp_srvlist) {
        zs_cur = z->zqp_srvlist->srv_next;
        zeroqp_service_free(z->zqp_srvlist);
        z->zqp_srvlist = zs_cur;
    }
    el_close(z->zqp_el);
    free(z);
}




