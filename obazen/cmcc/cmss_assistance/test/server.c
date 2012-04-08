#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc,char *argv[])
{
	int sockfd,new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sin_size,portnumber;
	char hello[]="Hello!Are you Fine?\n";
	
	if(argc!=2){
		fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
		exit(1);
	}
	
	if((portnumber=atoi(argv[1]))<0){
		fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
		exit(1);
	}
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		fprintf(stderr,"Socket Error:%s\n\a",strerror(errno));
		exit(1);
	}
	
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port=htons(portnumber);
	
	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr))==-1)
	{
		fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
		exit(1);
	}
	
	if(listen(sockfd,20)==-1)
	{
		fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
		exit(1);
	}
	
	while(1)
	{
		sin_size=sizeof(struct sockaddr_in);
		if((new_fd=accept(sockfd,(struct sockaddr*)&client_addr,&sin_size))==-1)
		{
			fprintf(stderr,"Accept error:%s\n\a",strerror(errno));
			exit(1);
		}
		fprintf(stderr,"Server get connection from %d\n",inet_ntoa(client_addr.sin_addr));
		if(write(new_fd,hello,strlen(hello))==-1)
		{
			fprintf(stderr,"Write Error:%s\n",strerror(errno));
			exit(1);
		}
		close(new_fd);
	}
	close(sockfd);
	exit(0);
}
