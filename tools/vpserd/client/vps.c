#include "anet.h"
#include <stdio.h>

#define CMDBUF 1024

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	socklen_t len;
	struct sockaddr_storage ss;
	
	if (argc!=3)
		err_quit("usage:...\n");
	
	sockfd = tcp_connect(argv[1],argv[2]);
	
	char cmd[CMDBUF] = "", *cM;
	int _argc;
	while (1) {
		cM = cmd;
		argc = 0;
		
		/* get the command. */
		strcpy(cM, "<c>"); cM = cM + strlen("<c>");
		scanf("%s",cM); 
		if (strcmp(cM, "cp") == 0)
			_argc = 2;
		else if (strcmp(cM, "touch") == 0)
			_argc = 1;
		else if (strcmp(cM, "rm") == 0)
			_argc = 1;		
		cM = cM + strlen(cM);
		strcpy(cM, "</c><a>");
		cM = cM + strlen("</c><a>");
		
		for (;_argc>0;_argc--) {
			scanf("%s",cM);
			cM = cM + strlen(cM);
		}
		
		strcpy(cM, "</a>"); cM = cM + strlen("</a>");
		*cM = '\0';
		
		if ((n = write(sockfd, cmd, strlen(cmd))) <=0) {
			printf("Net is not working.\n");
			break;
		}
		
		if ((n = read(sockfd, recvline, MAXLINE)) > 0) {
			recvline[n] = '\0';
			fputs(recvline, stdout);
		}
	}
	
	exit(0);
}
