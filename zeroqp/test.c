#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zeroqp.h"

zqp_t *zeroqp;

void echo_svr(int fd, int argc, zqp_context_slice_t **argv)
{
    while (argc) {
        printf("%s\n",argv[argc - 1]->slice_data);
        argc--;
    }
//    zeroqp_stop(zeroqp);
}



int main(int argc, char **argv)
{
    zeroqp = zeroqp_init("127.0.0.1",6379);
    zeroqp_append_service(zeroqp,"SET",echo_svr);

    zeroqp_start(zeroqp);
    zeroqp_close(zeroqp);
    sleep(10);
    return(0);
}
