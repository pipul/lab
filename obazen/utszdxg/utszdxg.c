#include <unp.h>

#define httpd_port 80

void utsz_dxg(int );

int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in seraddr;
	
	if(argc!=3)
		err_quit("<usege>:./dxg ipaddress\n");

	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_family=AF_INET;
	seraddr.sin_port=htons(httpd_port);
	inet_pton(AF_INET,argv[1],&seraddr.sin_addr);
	sockfd=socket(AF_INET,SOCK_STREAM,0);

	if(connect(sockfd,(SA *)&seraddr,sizeof(seraddr))==0)
		printf("wellcome.\n");		
		//utsz_dxg(argv[2],sockfd);
	else
		exit(0);

	return(0);
}

void utsz_dxg(int sockfd)
{
	FILE *fp;
	fp=fopen("");
	char buf[MAXLINE];
	write(sockfd,token,strlen(token));
	if(read(sockfd,buf,MAXLINE)==0)
}
