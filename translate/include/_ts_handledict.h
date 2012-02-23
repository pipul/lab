#ifndef _TS_HANDLEDICT_H_INCLUDE
#define _TS_HANDLEDICT_H_INCLUDE

#include <limits.h>

#ifndef NAME_MAX
	#define NAME_MAX 256
#endif
#ifndef PATH_MAX
	#define PATH_MAX 4096
#endif

#define DICTPATH "/home/laodong/.translate/Oxford Advanced"
#define NODESIZE 8192
#define BUFFSIZE 4096

char *_ts_translate(char * );
char *_ts_getdictfile(char * );
char *_ts_processword(char * ,char * );

#endif
