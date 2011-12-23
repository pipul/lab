#ifndef __CONFIG_H_
#define __CONFIG_H_

#define CONFIGPATH "/etc/coutune/coutune.conf"
#define CONFIGLIMIT 80

struct option {
	char name[CONFIGLIMIT];
	char value[CONFIGLIMIT];
};

typedef struct CF {
	int argc;
	struct option argv[];
} CF;

CF *cfileInit(char *cfile, char **argv);
char *getOption(CF *conf, char *opName);
void cfileFree(CF *conf); 


#endif
