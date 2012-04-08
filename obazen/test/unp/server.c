#include <unp.h>

void dg_echo(int ,SA *,socklen_t );
static void sig_int();
static int count;


int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in seraddr,cliaddr;

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_family=AF_INET;
	seraddr.sin_port=htons(SERV_PORT);
	seraddr.sin_addr.s_addr=htons(INADDR_ANY);

	bind(sockfd,(SA *)&seraddr,sizeof(seraddr));
	dg_echo(sockfd,(SA *)&cliaddr,sizeof(cliaddr));
}

static void sig_int()
{
	printf("server has recv %d",count);
	exit(0);
}

void dg_echo(int sockfd,SA *pcliaddr,socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];
	Signal(SIGINT,sig_int);

	for(;;){
		len=clilen;
		n=recvfrom(sockfd,mesg,MAXLINE,0,pcliaddr,&len);
		//sendto(sockfd,mesg,n,0,pcliaddr,len);
		count++;
	}
}
