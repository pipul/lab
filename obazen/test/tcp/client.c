#include <unp.h>

void str_cli(FILE *,int );

int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in seraddr;
	
	if(argc!=2)
		err_quit("<usege>:./client ipaddress\n");

	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_family=AF_INET;
	seraddr.sin_port=htons(SERV_PORT);
	inet_pton(AF_INET,argv[1],&seraddr.sin_addr);
	sockfd=socket(AF_INET,SOCK_STREAM,0);

	if(connect(sockfd,(SA *)&seraddr,sizeof(seraddr))==0)
		str_cli(stdin,sockfd);
	else
		exit(0);

	return(0);
}

void str_cli(FILE *fp,int sockfd)
{
	char send[MAXLINE],recv[MAXLINE];
	while((fgets(send,MAXLINE,fp)!=NULL)){
		write(sockfd,send,strlen(send));
		if(read(sockfd,recv,MAXLINE)==0)
			err_quit("str_cli:read error.\n");
		fputs(recv,stdout);
	}
}
