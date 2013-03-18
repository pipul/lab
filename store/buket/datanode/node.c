#include "net.h"
#include "ae.h"
#include "config.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/sendfile.h>




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
		c->argc = 3;
		c->proc = _get_command_proc;
	} else if ((ptr = strstr(resq,"PUT ")) != NULL) {
		c->name = CMD_PUT;
		c->argc = 3;
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

			if ((ptr = strstr(resq,"OFFSET ")) == NULL) break;
			snprintf(buffer,strstr(ptr,"\r\n")-ptr-6,"%s",ptr+7);
			if ((c->argv[1] = malloc(strlen(buffer)+1)) == NULL) {
				free(c->argv[0]);free(c->argv);
				break;
			}
			strncpy(c->argv[0],buffer,strlen(buffer));

			if ((ptr = strstr(resq,"LENGTH ")) == NULL) break;
			snprintf(buffer,strstr(ptr,"\r\n")-ptr-6,"%s",ptr+7);
			if ((c->argv[2] = malloc(strlen(buffer)+1)) == NULL) {
				free(c->argv[0]);free(c->argv[1]);
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

			if ((ptr = strstr(resq,"OFFSET ")) == NULL) break;
			snprintf(buffer,strstr(ptr,"\r\n")-ptr-6,"%s",ptr+7);
			if ((c->argv[1] = malloc(strlen(buffer)+1)) == NULL) {
				free(c->argv[0]);free(c->argv);
				break;
			}
			strncpy(c->argv[0],buffer,strlen(buffer));

			if ((ptr = strstr(resq,"LENGTH ")) == NULL) break;
			snprintf(buffer,strstr(ptr,"\r\n")-ptr-6,"%s",ptr+7);
			if ((c->argv[2] = malloc(strlen(buffer)+1)) == NULL) {
				free(c->argv[0]);free(c->argv[1]);
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


void sendRequesData(int listenfd, void *argv, int mask)
{
	int clientfd, fd, n;
	char RQFILE[MAXLINE] = "";
	char BUFF[MAXLINE] = "";
	char recvline[MAXLINE] = "";
	struct stat rqstat;

	char *ptr,*rqfptr;
	off_t offset;
	int size,len;
	
	socklen_t cliaddr_len;
	struct sockaddr_storage cliaddr;
	
	cliaddr_len = sizeof(cliaddr);
	clientfd = accept(listenfd,(SA *)&cliaddr,&cliaddr_len);
	
	strcpy(RQFILE,getOption((CF *)argv,"ServerPath"));
	rqfptr = RQFILE + strlen(RQFILE);

	while ((n = read(clientfd,recvline,MAXLINE)) > 0) {
		ptr = strstr(recvline,"GET ");
		snprintf(rqfptr,strstr(ptr,"\r\n")-ptr-3,"%s",ptr+4);

		ptr = strstr(recvline,"OFFSET ");
		snprintf(BUFF,strstr(ptr,"\r\n")-ptr-6,"%s",ptr+7);
		offset = atoi(BUFF);
		
		ptr = strstr(recvline,"SIZE ");
		snprintf(BUFF,strstr(ptr,"\r\n")-ptr-4,"%s",ptr+5);
		size = atoi(BUFF);
		
		stat(RQFILE,&rqstat);
		
		if (rqstat.st_size < offset) {
			close(clientfd);
			return;
		}
		
		len = rqstat.st_size - 
			offset > size ? size : rqstat.st_size - offset;

		if ((fd = open(RQFILE,O_RDONLY)) < 0) {
			close(clientfd);
			return;
		}

		sendfile(clientfd, fd, &offset, len);
		lseek(fd,offset,SEEK_SET);
		
		if ((n = read(fd,BUFF,len)) != len) {
			close(fd);
			close(clientfd);
			return;
		}
		
		write(clientfd,BUFF,len);
		close(fd);
	}

	close(clientfd);
}


int main(int argc, char **argv)
{
	int listenfd, connfd, n;
	EL *el;
	CF *conf;
	char *opName[] = {
		"Listen",
		"ServerPath",
		"ClusterManagerName",
		"ClusterManagerPort"};
	char *ptr;
	
	socklen_t addrlen;
	char BUFF[MAXLINE];
	struct sockaddr_storage cliaddr;
	
	if (argc > 2)
		err_quit("usage: node [path to config file] <--help>\n");
	
	if (argc == 2 && strcmp(argv[1],"--help") != 0)
		conf = cfileInit(argv[1],opName);
	else if (argc == 1)
		conf = cfileInit(DB_CONF,opName);

	connfd = tcpConnect(
		getOption(conf,"ClusterManagerName"),
		getOption(conf,"ClusterManagerPort"));
	
	if (connfd < 0)
		err_quit("error for connecting cluster manager.\n");

	strcpy(BUFF,"REG ");
	getExternalIp(BUFF + 4);
	strcat(BUFF,"\r\n");
	
	write(connfd,BUFF,strlen(BUFF));
	
	if ((n = read(connfd,BUFF,MAXLINE)) > 0) {
		BUFF[n] = '\0';
		if (strcmp(BUFF,"REG ERR\r\n") == 0)
			err_quit("error for registering node in cluster");
	} else
		err_quit("error for registering node in cluster");

	close(connfd);

	if ((ptr = getOption(conf,"Listen")) != NULL)
		listenfd = 
		tcpListen(getOption(conf,"ServerName"),ptr,&addrlen);
	else
		err_quit("error for opening listen port.\n");
	
	if ((el = el_open()) == NULL)
		err_quit("node error for creating epoll.\n");

	el_addFileEvent(el,listenfd,AE_READABLE,sendRequesData,(void *)conf);
	
	el_start(el,AE_BLOCK);

	el_close(el);
}	
	
