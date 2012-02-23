#include "../include/_ts_configure.h"
#include "../include/_ts_handledict.h"
#include <stdio.h>

int main(int argc,char **argv)
{
	char *res=NULL;
	size_t i;

	if(_ts_loadconf()==-1)
		return(-1);

	//system("clear");
	printf("\n%s:\n",argv[1]);
	for(i=0;i<40;i++)
		printf("-");
	printf("\n");

	res=_ts_translate(argv[1]);

	if(res!=NULL)	
		printf("%s",res);
	else
		printf("该单词没有收录到该词典中\n");

	for(i=0;i<40;i++)
		printf("-");
	printf("\n\n");

	free(res);
	return(0);
}	
