#include <stdio.h>
#include <string.h>
#include "ae.h"
#include "anet.h"
#include "cmd.h"
#include "configure.h"

#define CMDBUF 1024

typedef struct sockaddr SA;

static void connProc(EL *el, int listenfd, void *data, int op);


static void connProc(EL *el, int listenfd, void *data, int op)
{
	int clientfd, n;
	char e[] = "The invalid command.\n";

	socklen_t clientaddr_len;
	
	/* save the command name. */
	char cN[CMDBUF] = "";
	char cmdBuf[CMDBUF] = "";
	
	struct sockaddr_storage clientaddr;
	
	clientaddr_len = sizeof(clientaddr_len);
	clientfd = accept(listenfd,(SA *)&clientaddr,&clientaddr_len);
	
again:
	/* reading the command. and process. */
	while ((n = read(clientfd, cmdBuf, CMDBUF)) > 0) {
		if (getCmdName(cmdBuf, "<c>", "</c>", cN) == -1)
			write(clientfd, e, strlen(e));
			
		if (strcmp(cN, "cp") == 0)
			cp_proc(cmdBuf, clientfd);
		else if (strcmp(cN, "touch") == 0)
			touch_proc(cmdBuf, clientfd);
		else if (strcmp(cN, "rm") == 0)
			rm_proc(cmdBuf, clientfd);
	}
	
	close(clientfd);
}


int main(int argc, char **argv)
{
	int listenfd,clientfd;
	socklen_t len;
	struct sockaddr_storage cliaddr;
	EL *eventloop;
	
	listenfd = tcp_listen(NULL, PORT, &len);
	eventloop = el_open();
		
	el_addFileEvent(eventloop, listenfd, AE_READABLE, connProc, NULL);
	
	el_start(eventloop, AE_FILE_EVENTS);
	
	el_close(eventloop);
	
	return(0);
}

