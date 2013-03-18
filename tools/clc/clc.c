#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <apue.h>
#include <sys/utsname.h>
#include <dirent.h>

#define NAME_MAX 255

size_t _clc_reDir(char * ,char * );
size_t _clc_cfilel(char * );
size_t _clc_result(char * ,size_t );
size_t _clc_checkOpt(size_t ,char ** );
size_t _clc_unRecOpt(size_t ,char ** );
size_t _clc_version(void );
size_t _clc_helpInfo(void );

static size_t clc[NAME_MAX];
static char _r_suffix[NAME_MAX][NAME_MAX];
static char _w_suffix[NAME_MAX][NAME_MAX];

size_t main(size_t argc,char **argv){

	size_t i=0;
	
	bzero(&_r_suffix,sizeof(_r_suffix));
	bzero(&_w_suffix,sizeof(_w_suffix));

	if(argc==2){
		if(strcmp(argv[1],"--version")==0){
			_clc_version();
		}else if(strcmp(argv[1],"--help")==0){
			_clc_helpInfo();
		}else{
			_clc_unRecOpt(argc-2,argv+2);
		}
	}else if(argc==3&&strcmp(argv[2],"*")==0){
		_r_suffix[0][0]='.';
		_clc_result(_r_suffix[0],_clc_reDir(argv[1],_r_suffix[0]));
	}else if(argc>=3){
		_clc_checkOpt(argc-2,argv+2);
		while(_r_suffix[i][0]!='\0'){
			_clc_result(_r_suffix[i],_clc_reDir(argv[1],_r_suffix[i]));
			i++;
		}
	}

	return(0);
}

size_t _clc_checkOpt(size_t argc,char **argv){
	size_t r_count=0,i,j,k,t,len;
	for(i=0,j=0,k=0;i<argc;i++){
		if(strlen(argv[i])<=NAME_MAX){
			strcpy(_r_suffix[j],argv[i]);
			j++;
			r_count++;
		}else{
			for(t=0;t<(len=(strlen(argv[i])>>1))&&t<NAME_MAX-2;t++){
				_w_suffix[k][t]=argv[i][t];
			}
			k++;
			//printf("%s... ",_w_suffix[k]);
		}
	}
	_r_suffix[j][0]='\0';
	_w_suffix[k][0]='\0';
	return(r_count);
}

size_t _clc_result(char *suffix,size_t count){
	if(suffix!=NULL)
		printf("The total %30s Code lines is %d.\n",suffix,count);
	return(0);
}

size_t _clc_unRecOpt(size_t argc,char **argv){
	size_t i;
	printf("clc:unrecognized option ");
	for(i=0;i<argc;i++){
		printf("'%s' ",argv[i]);
	}
	printf("\nTry `clc --help' for more information\n\n");
	exit(0);
}

size_t _clc_version(){
	struct utsname sysName;
	if(uname(&sysName)==-1)
		bzero(&sysName,sizeof(sysName));
	printf("clc (%s) 1.0.1\n",sysName.sysname);
	printf("Copyright (C) 2011 Free Software Foundation,Inc.\n");
	printf("This is free software; see the source for copying conditions.\n");
	printf("There is NO warranty; not even fo MERCHANTABILLITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
	exit(0);
}

size_t _clc_helpInfo(){
	printf("Usage: clc <pathName> <suffix...>\n");
	printf("pathName:\n");
	printf("	./		Current directory\n");
	printf("	../		Parent directory\n");
	printf("	pathName/.../	Relative directory\n");
	printf("	/.../.../...	Absolute directory\n");
	printf("\nsuffix:\n");
	printf("	.c	Count the _c_source file\n");
	printf("	.cpp	Count the _cpp_source file\n");
	printf("	.php	Count the _php_source file\n");
	printf("	*	Count the All source file\n");
	printf("	...	Like this...\n");
	exit(0);
}

size_t _clc_reDir(char *pathName,char *suffix){
	char path[NAME_MAX]="";
	char *sp,*fpath=path,ch;
	size_t fileNo=0,count=0;
	DIR *dp;
	struct dirent *dir;
	
	strcpy(fpath,pathName);
	fpath=fpath+strlen(path);
	*fpath++='/';

	if((dp=opendir(pathName))==NULL)
		return(0);
	while((dir=readdir(dp))!=NULL){
		if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0)
			continue;
		*fpath=0;
		strcpy(fpath,dir->d_name);
		// printf("%s %d \n", dir->d_name, strlen(dir->d_name));
		if((sp=strstr(dir->d_name,suffix))!=NULL)
			count+=_clc_cfilel(path);
		else
			count+=_clc_reDir(path,suffix);
	}
	closedir(dp);	
	return(count);
}

size_t _clc_cfilel(char *fileName){
	size_t count=0;
	char ch;
	FILE *fp;
	if((fp=fopen(fileName,"r"))<0)
		err_quit("cfilel error.\n");
	if(access(fileName,R_OK)==0){
		while((ch=fgetc(fp))!=EOF){
			if(ch=='\n')
				count++;
		}
		fclose(fp);
		return(count);
	}else
		return(0);
}
