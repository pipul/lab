#ifndef __SERVER_H_
#define __SERVER_H_




#define SERVER_CONF "server.conf"
#define REPLICAS 20
#define MAXLINE 1024
typedef struct sockaddr SA;



/* 
 * GET filename\r\n
 *
 *
 * PUT filename\r\n
 *
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










#endif
