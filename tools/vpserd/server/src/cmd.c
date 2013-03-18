#include "cmd.h"
#include "file.h"
#include <stdio.h>
#include <string.h>

/* Command format: <c>cp</c><a>mm cc</a> */

#define CMDBUF 1024

int 
getCmdName(const char *buf, const char *st, const char *et, char *cN)
{
	char *sp = strstr(buf,st) + strlen(st);
	char *ep = strstr(buf,et);
	
	if (sp > ep)
		return(-1);
		
	while (sp != ep) {
		*cN++ = *sp++;
	}
	
	cN++; *cN = '\0';
	
	return(0);
}

int 
getCmdArgv(const char *buf, const char *st, const char *et, char *cA)
{
	char *sp = strstr(buf,st) + strlen(st);
	char *ep = strstr(buf,et);
	
	if (sp > ep)
		return(-1);
		
	while (sp != ep) {
		*cA++ = *sp++;
	}
	
	cA++; *cA = '\0';
	
	return(0);
}

/* Start rm process. */

int _rm_Arg(const char *argv, char *arg) {
	/* _rm command only have one parameter. */
	return(getCmdArgv(argv, "<a>", "</a>", arg));
}

int rm_proc(const char *buf, int fileNo) {
	char arg[CMDBUF] = "";
	
	if (_rm_Arg(buf, arg) == -1)
		return(-1);
	
	if (_rm(arg, fileNo) == -1)
		return(-1);
	
	return(0);
}

/* End rm process. */


/* Start touch process. */

int _touch_Arg(const char *argv, char *arg) {
	/* _touch command only have one parameter. */
	return(getCmdArgv(argv, "<a>", "</a>", arg));
}

int touch_proc(const char *buf, int fileNo) {
	char arg[CMDBUF] = "";
	
	if (_touch_Arg(buf, arg) == -1)
		return(-1);
	
	if (_touch(arg, fileNo) == -1)
		return(-1);
	
	return(0);
}

/* End touch process. */


/* Start copy process. */

int _cp_Arg(const char *argv, char *arg1, char *arg2) {
	/* _cp command have two parameter. */
	char arg[CMDBUF] = "";
	
	/* tok is point to the space place. */
	char *tok;
	
	/* get all the parameter from the cache buf. */
	if (getCmdArgv(argv, "<a>", "</a>", arg) == -1)
		return(-1);
	
	/* the parameters is separated by space. */
	if ((tok = strchr(argv, ' ')) == NULL)
		return(-1);
		
	/* get the first parameter for _touch command. */
	char *p = arg;
	while (arg != tok) {
		*arg1++ = *p++;
	}
	tok++; arg1++; *arg1 = '\0';
	
	/* get the second parameter for _touch command. */
	while (tok != '\0') {
		*arg2++ = *tok++;
	}
	
	arg2++; *arg2 = '\0';
	
	return(0);
}

int cp_proc(const char *buf, int fileNo) {
	char arg1[CMDBUF] = "";
	char arg2[CMDBUF] = "";
	
	if (_cp_Arg(buf, arg1, arg2) == -1)
		return(-1);
	if (_cp(arg1, arg2, fileNo) == -1)
		return(-1);
	
	return(0);
}


/* End copy process. */















