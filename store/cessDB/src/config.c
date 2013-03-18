#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
	char buf[BUFSIZE], *ptr, ch;

	/*
	 * Set the file position indicator for the stream pointed to
	 * the beginning of the file.
	 */
	rewind(fp);
	
ITER:
	ch = fgetc(fp);
	ptr = buf;
	while (!feof(fp) && ch!='\n') {
		if (__validchar(ch)==0)
			*ptr++ = ch;
		ch = fgetc(fp);
	}

	*ptr++ = '\0';
	ptr = buf + strlen(opName);
	if (buf[0] == '#' || *ptr++ != '=') {
		if (!feof(fp))
			goto ITER;
	} else if (strstr(buf,opName) == buf) {
		strcpy(value,ptr);
	} else {
		if (!feof(fp))
			goto ITER;
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
	
	if (i < conf->argc)
		return(ptr->value);
	else
		return(conf->argv[conf->argc].value);
}


/*
 * Test Case
 */
int main(int argc, char **argv)
{
	CF *conf;
	conf = cfileInit(argv[1],&argv[2]);
	
	int i;
	char *ptr;
	if (conf) {
		for (i=0;i<conf->argc;i++) {
			ptr = getOption(conf,argv[i]);
			printf("%s\n",ptr);
		}
	}
	
	cfileFree(conf);
	return(0);
}

 

