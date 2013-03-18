#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "anet.h"

int main(int argc, char **argv)
{
	int fd;
	int8_t msg[] = "*3\r\n$3\r\nSET\r\n$8\r\nyangping\r\n$5\r\nlaopo\r\n";
	if (argc != 3)
		return(0);
	fd = tcp_connect(argv[1],argv[2]);
	if (fd)
		write(fd,msg,strlen(msg));
}
