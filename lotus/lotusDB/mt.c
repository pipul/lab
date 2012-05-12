#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lotus.h"


int main(int argc, char **argv)
{
    int i,dtc;
    sst_t **dts,*st;

    dtc = atoi(argv[1]);
    st = sstable_open(argv[2],SST_CPONLY);
    sstable_split(st,dtc,&dts);
    for (i = 0; i < dtc; i++)
        sstable_close(dts[i]);
    sstable_close(st);
    free(dts);
    return(0);
}
