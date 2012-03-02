#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "config.h"

#define BUFSIZE 1024

static int __validchar(char ch)
{
	if ('a'<=ch && 'z'>=ch)
		return(0);
	else if ('A'<=ch && 'Z'>=ch)
		return(0);
	else if ('0'<=ch && '9'>=ch)
		return(0);
	else if ('='==ch || '_'==ch || '-'==ch || '#'==ch)
		return(0);
	else
		return(-1);
}


static int _readOption(FILE *fp, const char *opName, char *value)
{
	char buf[BUFSIZE], *ptr = NULL, ch;
	int opNamelen;

	/*
	 * Set the file position indicator for the stream pointed to
	 * the beginning of the file.
	 */
	rewind(fp);
	
	while (NULL != fgets(buf,BUFSIZE,fp)) {
		if ((ptr = strstr(buf,opName)) == NULL)
			continue;
		if (buf[0] == '#')
			continue;
		snprintf(value,strstr(ptr,
			"\n")-ptr-strlen(opName),"%s",ptr+strlen(opName)+1);
		break;
	}

	return(0);
}

CF *cfileInit(cfile, argv)
	char *cfile;
	char **argv;
{
	FILE *cfp;
	int i, argc = 0;
	
	if (cfile) {
		if ((cfp = fopen(cfile,"r")) == NULL)
			return(NULL);
	} else {
		/* Using the default config file. */
		if ((cfp = fopen(CONFIGPATH,"r")) == NULL)
			return(NULL);
	}
	
	/*
	 * Calculate the number of option.
	 */
	while (argv[argc]!=NULL) {
		argc++;
	}
	
	/*
	 * Malloc memory for storing all the conf.
	 */
	CF *conf = (CF *)malloc(sizeof(CF) + argc*sizeof(struct option));
	conf->argc = argc;

	for (i = 0;i < argc; i++) {
		strcpy(conf->argv[i].name,argv[i]);
		_readOption(cfp,argv[i],conf->argv[i].value);
	}
	
	close(cfp);
	return(conf);
}

void cfileFree(CF *conf)
{
	free(conf);
}

char *getOption(CF *conf, char *opName)
{
	struct option *ptr = conf->argv;
	int i = 0;
	
	while (i < conf->argc) {
		if (strcmp(ptr->name,opName) == 0) {
			break;
		} else {
			ptr++;
			i++;
		}
	}
	
	if (i < conf->argc) {
		if (strlen(ptr->value) == 0)
			return(NULL);
		else
			return(ptr->value);
	} else
		return(NULL);
}
 

