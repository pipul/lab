#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <fcntl.h>


int main(int argc, char **argv)
{
    int sfd,i,p[2],n;
    char buffer[65536];
    if (argc != 3)
        return(-1);
    for (i = 1; i<atoi(argv[1]) ; i++) {
        printf("%d\n",i);
        printf("%d\n",i+1);
    }
}
