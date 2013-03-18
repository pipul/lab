#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>

#define BUFFSIZE 4096

static size_t handfile(FILE * ,FILE * );

int main(int argc,char **argv)
{
	char filename[BUFFSIZE],buf[BUFFSIZE];
	FILE *fp,*nfp;
	DIR *dp;
	size_t leek=0;
	struct dirent *dir;

	if((dp=opendir("./"))==NULL)
		return(0);
	while((dir=readdir(dp))!=NULL){
		if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0)
			continue;
		strcpy(filename,dir->d_name);
		leek=strlen(filename);
		filename[leek]='1';
		filename[leek+1]='\0';
		nfp=fopen(filename,"w");
		fp=fopen(dir->d_name,"r");
		printf("%s ",dir->d_name);
		handfile(nfp,fp);
	}
}

static size_t handfile(FILE *dest,FILE *src)
{
	char buf[BUFFSIZE],*pbuf;
	size_t leek=0;
	size_t count=0;

	while(fgets(buf,BUFFSIZE,src)!=NULL){
		pbuf=buf;
		leek=strlen(pbuf);
		while(*pbuf==' '){
			pbuf++;
			leek--;
		}

		while(pbuf[leek-2]==' '||pbuf[leek-2]=='\r'){
			pbuf[leek-1]='\0';
			pbuf[leek-2]='\n';
			leek--;
		}
		if(strlen(pbuf)==leek)
			fputs(pbuf,dest);
		count++;
	}
	printf("%d\n",count);
	return(0);

}
