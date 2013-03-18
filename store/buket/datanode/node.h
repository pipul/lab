#ifndef __NODE_H_
#define __NODE_H_


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


#define DB_PATH "db/"
#define DB_CONF "node.conf"
#define MAXLINE 1024
typedef struct sockaddr SA;



#endif
