/* qcached.c - A quickly cached system implementation
 *
 * Copyright (c) 2011-2012, FangDong <yp.fangdong@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qcached.h"


qcached_t *qcached_new(size_t cap, float r)
{
    int hir_size,lir_size,ht_size;
    qcached_t *qc;

    if (cap == 0 || r <= 0)
        return(NULL);
    if (!(qc = calloc(1,sizeof(qcached_t))))
        return(NULL);
    hir_size = cap * r;
    lir_size = cap - hir_size;
    ht_size = cap * HASH_LIRS_P;
    qc->ht = hash_table_new(ht_size);
    qc->lhir = lirs_list_new(hir_size,TYPE_HIR);
    qc->llir = lirs_list_new(lir_size,TYPE_LIR);
    if (!qc->ht || !qc->lhir || !qc->llir) {
        qcached_free(qc);
        return(NULL);
    }
    return(qc);
}

void qcached_free(qcached_t *qc)
{
    if (!qc)
        return;
    hash_table_free(qc->ht);
    lirs_list_free(qc->lhir);
    lirs_list_free(qc->llir);
    free(qc);
}


static int _qcached_fixup(qcached_t *qc)
{
    int fix_lir,fix_hir;
    lirs_elem_t *he,*le;

    if (!qc)
        return(-1);
    if ((fix_lir = qc->llir->size - qc->llir->cap) > 0) {
        fix_hir = fix_lir - (qc->lhir->cap - qc->lhir->size);
        while (fix_hir--) {
            if (he = lirs_list_pop(qc->lhir)) {
                le = (lirs_elem_t *)hash_table_get(qc->ht,he->key);
                if (le) {
                    hash_table_del(qc->ht,he->key);
                    lirs_list_del(qc->llir,le);
                    lirs_elem_free(le);
                }
                free(he->value);
                lirs_elem_free(he);
            } else
                break; /* HIR queue is empty */
        }
        for (le = qc->llir->tail; fix_lir > 0; ) {
            if (le->flags & TYPE_HIR) {
                le = le->le_prev;
                continue;
            }
            he = le;
            le = le->le_prev;
            lirs_list_del(qc->llir,he);
            hash_table_del(qc->ht,he->key);
            he->flags = he->flags & (~TYPE_LIR);
            he->flags |= TYPE_HIR;
            lirs_list_push(qc->lhir,he);
            fix_lir--;
        }
    }

    while (qc->llir->tail) {
        if (qc->llir->tail->flags & TYPE_HIR) {
            le = lirs_list_pop(qc->llir);
            hash_table_del(qc->ht,le->key);
            lirs_elem_free(le);
        } else
            break;
    }

    if ((fix_hir = qc->lhir->size - qc->lhir->cap) > 0) {
        while (fix_hir--) {
            he = lirs_list_pop(qc->lhir);
            le = (lirs_elem_t *)hash_table_get(qc->ht,he->key);
            if (le)
                le->value = NULL;
            free(he->value);
            lirs_elem_free(he);
        }
    }
    return(0);
}

int qcached_set(qcached_t *qc, char *key, char *value)
{
    lirs_elem_t *le,*he;

    if (!qc || !key || !value)
        return(-1);
    if (!qc->llir->size) {
        le = lirs_elem_new(key);
        le->flags = TYPE_LIR;
        le->value = strdup(value);
        lirs_list_push(qc->llir,le);
        hash_table_add(qc->ht,le->key,le);
        return(0);
    }
    le = (lirs_elem_t *)hash_table_get(qc->ht,key);
    if (le) {
        le->flags = le->flags & (~TYPE_HIR);
        le->flags |= TYPE_LIR;
        le->value = strdup(value);
        lirs_list_del(qc->llir,le);
        lirs_list_push(qc->llir,le);
    } else {
        he = lirs_elem_new(key);
        le = lirs_elem_new(key);
        le->flags = he->flags |= TYPE_HIR;
        le->value = he->value = strdup(value);
        lirs_list_push(qc->lhir,he);
        lirs_list_push(qc->llir,le);
        hash_table_add(qc->ht,le->key,le);
    }
    _qcached_fixup(qc);
    return(0);
}

char *qcached_get(qcached_t *qc, char *key)
{
    lirs_elem_t *he,*le;

    if (!qc || !key)
        return(NULL);
    le = (lirs_elem_t *)hash_table_get(qc->ht,key);
    if (le) {
        if (le->flags & TYPE_LIR) {
            lirs_list_del(qc->llir,le);
            lirs_list_push(qc->llir,le);
            _qcached_fixup(qc);
            return(le->value);
        } else if (le->value) {
            /* this is a HIR resident, change it into LIR */
            lirs_list_del(qc->llir,le);
            le->flags = le->flags & (~TYPE_HIR);
            le->flags |= TYPE_LIR;
            he = lirs_list_get(qc->lhir,key);
            lirs_list_del(qc->lhir,he);
            lirs_elem_free(he);
            lirs_list_push(qc->llir,le);
            _qcached_fixup(qc);
            return(le->value);
        } else
            return(NULL);
    }
    he = lirs_list_get(qc->lhir,key);
    if (he) {
        le = lirs_elem_new(key);
        le->flags = he->flags;
        le->value = he->value;
        lirs_list_del(qc->lhir,he);
        lirs_list_push(qc->lhir,he);
        lirs_list_push(qc->llir,le);
        hash_table_add(qc->ht,le->key,le);
        _qcached_fixup(qc);
        return(he->value);
    }
    return(NULL);
}


void qcached_stats(qcached_t *qc)
{
    lirs_elem_t *le,*he;

    if (!qc)
        return;
    le = qc->llir->head;
    he = qc->lhir->head;
    fprintf(stderr,"\nLIR stats:\n");
    while (le) {
        fprintf(stderr,"%s:%d ",le->key,le->flags);
        le = le->le_next;
    }

    fprintf(stderr,"\nHIR stats:\n");
    while (he) {
        fprintf(stderr,"%s:%d ",he->key,he->flags);
        he = he->le_next;
    }
    fprintf(stderr,"\n");
    return;
}
