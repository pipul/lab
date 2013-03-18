#include "debug.h"
#include "conhash.h"
#include "ae.h"
#include "net.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

static cluster *cl;




int _get_command_proc(int argc, char **argv, int sockfd);
int _put_command_proc(int argc, char **argv, int sockfd);
int _del_command_proc(int argc, char **argv, int sockfd);



#define PARSE_OK 0x1
#define PARSE_ERR 0x2

CMD *command_parse(char *resq,size_t size)
{
	CMM *c;
	char *ptr;
	char buffer[MAXLINE];
	int state = PARSE_ERR;
	
	if (!resq)
		return(-1);
	else
		resq[size-1] = '\0';
	
	if ((c = malloc(sizeof(CMM))) == NULL)
		return(-1);

	c->argc = 0;
	c->argv = NULL;
	
	if ((ptr = strstr(resq,"GET ")) != NULL) {
		c->name = CMD_GET;
		c->argc = 1;
		c->proc = _get_command_proc;
	} else if ((ptr = strstr(resq,"PUT ")) != NULL) {
		c->name = CMD_PUT;
		c->argc = 1;
		c->proc = _put_command_proc;
	} else if ((ptr = strstr(resq,"DEL ")) != NULL) {
		c->name = CMD_DEL;
		c->argc = 1;
		c->proc = _del_command_proc;
	}
	
	switch(c->name)
	{
		case CMD_GET:
			c->argv = malloc(c->argc * sizeof(char *));

			if ((ptr = strstr(resq,"GET ")) == NULL) break;
			snprintf(buffer,strstr(ptr,"\r\n")-ptr-3,"%s",ptr+4);
			if ((c->argv[0] = malloc(strlen(buffer)+1)) == NULL) {
				free(c->argv);
				break;
			}
			strncpy(c->argv[0],buffer,strlen(buffer));
			
			state = PARSE_OK;
			break;

		case CMD_PUT:
			c->argv = malloc(c->argc * sizeof(char *));

			if ((ptr = strstr(resq,"PUT ")) == NULL) break;
			snprintf(buffer,strstr(ptr,"\r\n")-ptr-3,"%s",ptr+4);
			if ((c->argv[0] = malloc(strlen(buffer)+1)) == NULL) {
				free(c->argv);
				break;
			}
			strncpy(c->argv[0],buffer,strlen(buffer));
			
			state = PARSE_OK;
			break;
		
		case CMD_PUT:
			c->argv = malloc(c->argc * sizeof(char *));

			if ((ptr = strstr(resq,"DEL ")) == NULL) break;
			snprintf(buffer,strstr(ptr,"\r\n")-ptr-3,"%s",ptr+4);
			if ((c->argv[0] = malloc(strlen(buffer)+1)) == NULL) {
				free(c->argv);
				break;
			}
			strncpy(c->argv[0],buffer,strlen(buffer));
			
			state = PARSE_OK;
			break;

		default:
			break;
	}
	
	if (state == PARSE_ERR) {
		free(c);
		return(NULL);
	}
	
	return(c);
}

int command_exec(CMD *c, int sockfd)
{
	return(c->proc(c->argc,c->argv,sockfd));
}

void command_free(CMD *c)
{
	int i;

	if (c) {
		for (i = 0;i < c->argc;i++)
			free(c->argv[i]);
		free(c);
	}
}

int _get_command_proc(int argc, char **argv, int sockfd)
{

}

int _put_command_proc(int argc, char **argv, int sockfd)
{

}

int _del_command_proc(int argc, char **argv, int sockfd)
{

}

void nodeRegister(int registerfd, void *argv, int mask)
{
	int nodefd, n;
	char BUFF[MAXLINE] = "";
	char REGIP[MAXLINE] = "";
	char *ptr;
	
	socklen_t nodeaddr_len;
	struct sockaddr_storage nodeaddr;
	
	nodeaddr_len = sizeof(nodeaddr_len);
	nodefd = accept(registerfd,(SA *)&nodeaddr,&nodeaddr_len);

	if ((n = read(nodefd,BUFF,MAXLINE)) > 0) {
		ptr = strstr(BUFF,"REG ");
		snprintf(REGIP,strstr(ptr,"\r\n")-ptr-3,"%s",ptr+4);
		
		if (0 == clusterAddServer((cluster *)argv,REGIP,REPLICAS)) {
			write(nodefd,"REG OK\r\n",strlen("REG OK\r\n"));
			printf("%s REG OK\n",REGIP);
		} else {
			write(nodefd,"REG ERR\r\n",strlen("REG ERR\r\n"));
			printf("%s REG ERR\n",REGIP);
		}
	}

	close(registerfd);
}

void sendServerIp(int listenfd, void *argv, int mask)
{
	int clientfd, n;
	char BUFF[MAXLINE] = "";
	char RQFILE[MAXLINE] = "";
	char *ptr;
	const char *serIp;

	socklen_t cliaddr_len;
	struct sockaddr_storage cliaddr;
	
	cliaddr_len = sizeof(cliaddr);
	clientfd = accept(listenfd,(SA *)&cliaddr,&cliaddr_len);
	
	if ((n = read(clientfd,BUFF,MAXLINE)) > 0) {
		ptr = strstr(BUFF,"GET ");
		snprintf(RQFILE,strstr(ptr,"\r\n")-ptr-3,"%s",ptr+4);

		serIp = clusterGetServer(cl,RQFILE);
		
		if (serIp) {
			snprintf(BUFF,MAXLINE,"HOST %s\r\nPORT 6666\r\n",serIp);
			write(clientfd,BUFF,strlen(BUFF));
		} else
			write(clientfd,"HOST ERR\r\n",10);
	}
	
	close(clientfd);
}


int main(int argc, char **argv)
{
	int listenfd, registerfd;
	EL *el;
	CF *conf;
	char *opName[] = {
		"Listen",
		"NodeRegisterPort"};
	char *ptr;
	char BUFF[MAXLINE] = "";

	if (argc > 2)
		err_quit("usage: node [path to config file] <--help>\n");
	
	if (argc == 2 && strcmp(argv[1],"--help") != 0)
		conf = cfileInit(argv[1],opName);
	else if (argc == 1)
		conf = cfileInit(SERVER_CONF,opName);

	if ((ptr = getOption(conf,"Listen")) != NULL)
		listenfd = tcpListen(
			getOption(conf,"ServerName"),ptr,NULL);
	else
		err_quit("error for opening listen port.\n");

	if ((ptr = getOption(conf,"NodeRegisterPort")) != NULL)
		registerfd = tcpListen(
			getOption(conf,"ServerName"),ptr,NULL);
	else
		err_quit("error for opening listen port.\n");

	
	if ((el = el_open()) == NULL)
		err_quit("server error for creating epoll.\n");
	if ((cl = clusterCreate()) == NULL)
		err_quit("server error for creating cluster.\n");


	el_addFileEvent(el,listenfd,AE_READABLE,sendServerIp,NULL);
	el_addFileEvent(el,registerfd,AE_READABLE,nodeRegister,(void *)cl);

	el_start(el,AE_BLOCK);
	
	el_close(el);
}
