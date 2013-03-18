#ifndef _TS_CONFIGURE_H_INCLUDE
#define _TS_CONFIGURE_H_INCLUDE

/*
 *Get the Option from configure file. default: /etc/newwords/ts.conf
 */

#include <limits.h>
#include <apue.h>

#define DICTSIZE 2048
#define BUFFSIZE 4096

#ifndef NAME_MAX
	#define NAME_MAX 14
#endif

static char _ts_dict_list[BUFFSIZE];
static char _ts_default_dict[NAME_MAX];

size_t _ts_loadconf(void );
size_t _ts_initconf(char * ,char * );

#endif
