#include "_ts_handledict.h"
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>


char *_ts_translate(char *word)
{
	char *_ts_df_name=NULL,*_ts_df_n=NULL;
	char *_ts_result=NULL;

	if((_ts_df_name=(char *)malloc(PATH_MAX))==NULL)
		return _ts_result;
	else{
		memset(_ts_df_name,0,PATH_MAX);
	}

	if((_ts_result=(char *)malloc(NODESIZE))==NULL)
		return _ts_result;
	else{
		memset(_ts_result,0,NODESIZE);
	}

	if((_ts_df_n=_ts_getdictfile(word))==NULL){
		free(_ts_df_n);
		return(_ts_result);
	}else{
		strcpy(_ts_df_name,_ts_df_n);
		free(_ts_df_n);
	}	

	return(_ts_result=_ts_processword(_ts_df_name,word));


}

char *_ts_getdictfile(char *word)
{
	char *_ts_df_name=NULL,*_ts_df_leeks=NULL;
	size_t fileNo=0;
	DIR *dp;
	struct dirent *dir;

	if((_ts_df_name=malloc(PATH_MAX))==NULL)
		return(_ts_df_name);
	else
		_ts_df_leeks=_ts_df_name;
	
	strcpy(_ts_df_leeks,DICTPATH);
	_ts_df_leeks+=strlen(DICTPATH);
	*_ts_df_leeks++='/';

	if((dp=opendir(_ts_df_name))==NULL){
		free(_ts_df_name);
		return(NULL);
	}

	while((dir=readdir(dp))!=NULL){
		if(dir->d_name[0]==word[0]){
			strcpy(_ts_df_leeks,dir->d_name);
			_ts_df_leeks+=strlen(dir->d_name);
			*_ts_df_leeks++='\0';
			break;
		}else
			continue;
	}

	closedir(dp);
	
	if(dir==NULL)
		return(NULL);
	else
		return(_ts_df_name);

}

char *_ts_processword(char *dictfile,char *word)
{
	size_t token=0;
	char buf[BUFFSIZE],*result=NULL;
	char *pbuf;
	FILE *fp;
	
	result=(char *)malloc(NODESIZE);
	memset(result,0,BUFFSIZE);
	
	if((fp=fopen(dictfile,"r"))==NULL)
		return(NULL);

	while(fgets(buf,BUFFSIZE,fp)!=NULL){
		pbuf=buf;
		pbuf+=strlen(pbuf)-1;
		*pbuf='\0';
		if(strcmp(buf,word)==0){
			fgets(result,BUFFSIZE,fp);
			token=1;
			break;
		}else{
			fgets(buf,BUFFSIZE,fp);
			memset(buf,0,BUFFSIZE);
		}
	}
	
	if(token==1)
		return(result);
	else
		return(NULL);

}










