#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define DEBUG

#define MAXLINE 1024


/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void err_msg(const char *fmt, ...);



/*
 * Nonfatal error related to a system call.
 * Print a message and return.
 */
void err_ret(const char *fmt, ...);


/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void err_sys(const char *fmt, ...);


/*
 * Fatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and terminate.
 */
void err_exit(int error, const char *fmt, ...);


/*
 * Fatal error related to a system call.
 * Print a message, dump core, and terminate.
 */
void err_dump(const char *fmt, ...);




/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void err_quit(const char *fmt, ...);


#ifdef DEBUG
#define __DEBUG(x...) do {                                  				\
        fprintf(stderr, "[%d] %s(line:%d) ", (int)getpid(), __FUNCTION__, __LINE__); 	\
        fprintf(stderr, ##x);                               				\
		fprintf(stderr, "\n");							\
    } while(0)
#else
	#define __DEBUG(x...)
#endif




#endif
