#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zqp.h"


void echo_svr(int fd, int argc, slice **argv)
{
	while (argc) {
		printf("%s ",argv[argc - 1]->data);
		argc--;
	}
	printf("\n");
}



int main(int argc, char **argv)
{
	int32_t zfd,i;

	struct zeroqp_service srvlist[] = {
		{"127.0.0.1",6379,"SET",echo_svr},
		{"127.0.0.1",6666,"GET",echo_svr},
		{"127.0.0.1",8888,"PUT",echo_svr},
		{"127.0.0.9",8888,"DEL",echo_svr}
	};

	zfd = zeroqp_open();
	for (i = 0; i < 4; i++)
		zeroqp_ctl(zfd,ZEROQP_CTL_ADD,&srvlist[i]);
	zeroqp_wait(zfd);
	zeroqp_close(zfd);
    return(0);
}
