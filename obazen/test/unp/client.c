#include <unp.h>
#define NDG 2000
void dg_cli(FILE * ,int ,const SA *,socklen_t );



int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in seraddr;
	if(argc!=2)
		err_quit("usage:./client ipaddress.\n");
	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_family=AF_INET;
	seraddr.sin_port=htons(SERV_PORT);
	inet_pton(AF_INET,argv[1],&seraddr.sin_addr);
	
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	dg_cli(stdin,sockfd,(SA *)&seraddr,sizeof(seraddr));
	exit(0);
}

void dg_cli(FILE *fp,int sockfd,const SA *pseraddr,socklen_t serlen)
{
	//int n;
	int i;
	char send[MAXLINE];
	for(i=0;i<NDG;i++){
		sendto(sockfd,send,MAXLINE,0,pseraddr,serlen);
		//n=recvfrom(sockfd,recv,strlen(recv),0,NULL,NULL);
		//recv[0]=0;
		//fputs(recv,stdout);
	}
}
