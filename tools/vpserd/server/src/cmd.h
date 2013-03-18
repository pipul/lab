#ifndef _CMD_H_
#define _CMD_H_

/* parse the command name and argumentation from the buf. */

int 
getCmdName(const char *buf, const char *st, const char *et, char *cN);

int 
getCmdArgv(const char *buf, const char *st, const char *et, char *cA);

int _rm_Arg(const char *argv, char *arg);
int rm_proc(const char *buf, int fileNo);

int _touch_Arg(const char *argv, char *arg);
int touch_proc(const char *buf, int fileNo);

int _cp_Arg(const char *argv, char *arg1, char *arg2);
int cp_proc(const char *buf, int fileNo);



#endif
