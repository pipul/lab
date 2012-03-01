#include "net.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 1024
#define NODE_PORT "6666"
#define SERV_PORT "7777"

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	char reques[MAXLINE + 1];
	char buf[4];
	long cost = 0;
	time_t start,end;
	
	if (argc != 4) {
		printf("usage: <HOST> <PORT> <file>\n");
		exit(0);
	}
	srand((unsigned)time(NULL));
	start = _ustime();

	while (count < 10000) {

		sockfd = tcpConnect(argv[1],argv[2]);
		
		strcpy(reques,"GET ");
		snprintf(buf,4,"%d",rand()%20 + 1);
		strcat(reques,buf);
		strcat(reques,"\r\n");

		if ((n = write(sockfd,reques,strlen(reques))) != strlen(reques)) {
			printf("write error\n");
			exit(0);
		}
	
	
		if ((n = read(sockfd,recvline,MAXLINE)) > 0) {
			recvline[n] = 0;
			fputs(recvline,stdout);
			printf("\n");
		} else
			exit(0);

		close(sockfd);
		sockfd = tcpConnect(recvline,NODE_PORT);
		strcat(reques,"OFFSET 0\r\n");
		strcat(reques,"SIZE 10\r\n");
	
		if ((n = write(sockfd,reques,strlen(reques))) != strlen(reques)) {
			printf("write error\n");
			exit(0);
		}
	
		if ((n = read(sockfd,recvline,MAXLINE)) > 0) {
			recvline[n] = 0;
			fputs(recvline,stdout);
			printf("\n");
		}
		close(sockfd);
		count++;
	}

	end = _ustime();
	
	printf("10000 --> %ld",end - start);

	exit(0);
}	
	
