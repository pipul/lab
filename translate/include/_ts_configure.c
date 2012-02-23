#include "_ts_configure.h"
#include <fcntl.h>
#include <string.h>

size_t _ts_loadconf()
{

	/*
	 * key=1 represent this line is a notation
	 */

	size_t fd=-1,n=0,key=0;

	/*
	 * load/read the configure from file to buf and analyze rule
	 * pbuf is easy to analyze the rule one bytes by one bytes
	 */

	char buf[BUFFSIZE],*pbuf;

	/*
	 * save the option name with value to opt_name and opt_value
	 * when analyze each line,of course one line can only contain
	 * a option ! remask !
	 */

	char opt_name[NAME_MAX],opt_value[NAME_MAX],*opt_n,*opt_v;

	bzero(buf,BUFFSIZE);

	if((fd=open("/etc/translate/ts.conf",O_RDONLY))==-1)
		return(-1);

	while((n=read(fd,buf,BUFFSIZE))>0){
		
		pbuf=buf;

		while(pbuf[0]!='\0'){

			/*
			 * once analyze each line,flush all the buf
			 */

			bzero(opt_name,NAME_MAX);
			bzero(opt_value,NAME_MAX);
			opt_n=opt_name;
			opt_v=opt_value;
			
			/*
			 * if key=1 means this one line is a notation
			 * anotate it
			 */

			if(pbuf[0]=='#'){
				key=1;
				pbuf++;
			}else if(pbuf[0]=='\n'||pbuf[0]=='\0'){
				pbuf++;
				continue;
			}

			switch(key){
			
			case 0:
			/*
			 * key=0 ,read the option name from file
			 */

				while(pbuf[0]!='='){
					opt_n[0]=pbuf[0];
					opt_n++;
					pbuf++;
				}

				pbuf++;

			/* 
			 * also,save the option value
			 */

				while(pbuf[0]!='\n'&&pbuf[0]!='\0'){
					opt_v[0]=pbuf[0];
					opt_v++;
					pbuf++;
				}

				if(pbuf[0]=='\n')
					pbuf++;
			
			/*
			 * save the option name and value from buf
			 */

			if(strcmp(opt_name,"_ts_dict_list")==0)
				strcpy(_ts_dict_list,opt_value);
			else if(strcmp(opt_name,"_ts_default_dict")==0)
				strcpy(_ts_default_dict,opt_value);

				break;
			case 1:
			/*
			 * key=1 ,filter the anotation line
			 */

				while(pbuf[0]!='\0'&&pbuf[0]!='\n'){
					pbuf++;
				}
				key=0;	//reset the key value

				if(pbuf[0]=='\n')
					pbuf++;
				break;
			}
		}
	}
	close(fd);
	return(0);
}

size_t _ts_initconf(char *option_name,char *option_value)
{
	return(0);
}
