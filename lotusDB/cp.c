#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
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
sloter_t *_cp_runcp(const sloter_t *slt) {}
