#include <stdio.h>

int main(int argc,char **argv){
	int fileNo=0,count=0;
	char ch;

	if(argc!=2)
		err_quit("");
	if((fileNo=fopen(argv[1],"r+"))<0)
		err_quit("");
	while((ch=fgetc(fileNo))!=EOF){
		if(ch=='\n')
			count++;
	}
	printf("The total code line is:%d",count);
	return(0);
}
