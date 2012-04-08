#include <stdio.h>
#include <dirent.h>
#include <apue.h>
#define NAME_MAX 255

int main(int argc,char *argv[]){
	struct dirent *dir;
	DIR *dp;
	int i;
	char path[NAME_MAX],*fullpath;

	if(argc!=2){
		printf("Usage:./readir <pathname>\n");
		exit(0);
	}
	if((dp=opendir(argv[1]))==NULL)
		exit(0);
	bzero(path,sizeof(path));
	fullpath=path;
	strcpy(fullpath,argv[1]);
	fullpath=fullpath+strlen(fullpath);
	*fullpath++='/';
	*fullpath=0;
	
	while((dir=readdir(dp))!=NULL){
		if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0)
			continue;
		//strcpy(path,dir->d_name);
		printf("%s\n",dir->d_name);
	}

	return(0);

}
