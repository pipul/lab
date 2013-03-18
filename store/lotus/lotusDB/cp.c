/* cp.c - A compact handle implementation
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
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include "lotus.h"


CP *cp_open(const char *path)
{
    int i;
    sst_t *sst;
    CP *cp;
    DIR *dp;
    struct dirent *dir;
    char sstfile[_PATH_MAX] = {0};

    if (!path || !(dp = opendir(path)))
        return(NULL);
    if (!(cp = calloc(1,sizeof(CP)))) {
        closedir(dp);
        return(NULL);
    }
    for (i = 0; i < _SLOTTAG_MAX; i++)
        cp->cp_slot[i] = sloter_new();

    snprintf(cp->cp_path,_PATH_MAX,"%s",path);
    while ((dir = readdir(dp)) != NULL) {
        if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0)
            continue;
        if (strstr(dir->d_name,_SST_SUFFIX) != NULL) {
            snprintf(sstfile,_PATH_MAX,"%s/%s",path,dir->d_name);
            if (!(sst = sstable_open(sstfile,SST_CPONLY))) {
                __DEBUG();
                continue;
            }
            sloter_add(cp->cp_slot[sst->stat.slottag],sst);
        }
    }

    cp->pcp = _cp_pcp;
    cp->runcp = _cp_runcp;
    cp->close = _cp_close;

    cp->_sst_link = __cp_sst_link;
    cp->_sst_unlink = __cp_sst_unlink;
    closedir(dp);
    return(cp);
}

int _cp_close(CP *cp)
{
    int i;

    if (!cp)
        return(-1);
    for (i = 0; i < _SLOTTAG_MAX; i++)
        sloter_free(cp->cp_slot[i]);
    free(cp);
}


sloter_t *_cp_pcp(const CP *cp) {}


sloter_t *_cp_runcp(const CP *cp, const sloter_t *slt)
{

}


int __cp_sst_link(CP *cp, char *sst_name)
{
    char filename[_PATH_MAX] = {0};
    sst_t *sst;

    if (!cp || !sst_name)
        return(-1);
    snprintf(filename,_PATH_MAX,"%s/%s",cp->cp_path,sst_name);
    if (!(sst = sstable_open(filename,SST_RDONLY)))
        return(-1);
    return(sloter_add(cp->cp_slot[sst->stat.slottag],sst));
}
int __cp_sst_unlink(CP *cp, char *sst_name)
{
    int i;
    sst_t *sst = NULL;

    if (!cp || !sst_name)
        return(-1);
    for (i = 0; i < _SLOTTAG_MAX; i++)
        if (sst = sloter_get(cp->cp_slot[i],sst_name))
            break;
    if (sst) {
        sloter_del(cp->cp_slot[sst->stat.slottag],sst);
        sstable_close(sst);
        remove(sst_name); /* when unlink a sstable file, remove it */
    }
    return(0);
}
