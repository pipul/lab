#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h"


/* 
 * GET filename\r\n
 * OFFSET 234\r\n
 * LENGTH 1024\r\n
 *
 * PUT filename\r\n
 * OFFSET 234\r\n
 * LENGTH 1024\r\n
 *
 * DEL filename\r\n
 *
 */



#define CMD_GET 0x1
#define CMD_PUT 0x2
#define CMD_DEL 0X4
#defime MAXLINE 1024

typedef struct command {
	int c_name;		/* command name */
	int argc;		/* command's argument number */
	char *argv[];		/* command arguments */
				/* command handle process */
	int (*proc)(int , char **);
} CMM;


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

int _get_command_proc(int argc, char **argv, int sockfd);
	
int _put_command_proc(int argc, char **argv, int sockfd);
int _del_command_proc(int argc, char **argv, int sockfd);

