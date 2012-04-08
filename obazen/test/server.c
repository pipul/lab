#include <unp.h>

void str_echo(int );
void sig_chld(int );

int main(int argc,char **argv)
{
	int listenfd,connfd,errno,sockfd;
	int client[FD_SETSIZE],nready;
	int maxcl,maxfd,i,n;
	char buf[MAXLINE];
	fd_set sockset,tempset;
	pid_t childpid,pid;
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	listenfd=socket(AF_INET,SOCK_STREAM,0);
	bind(listenfd,(SA *)&servaddr,sizeof(servaddr));

	signal(SIGCHLD,sig_chld);
	listen(listenfd,LISTENQ);
	for(i=0;i<FD_SETSIZE;i++){
		client[i]=-1;;
	}
	FD_ZERO(&sockset);
	FD_SET(listenfd,&sockset);
	maxfd=listenfd;

	for(;;){
		tempset=sockset;
		nready=select(maxfd+1,&tempset,NULL,NULL,NULL);
		if(FD_ISSET(listenfd,&tempset)){
			clilen=sizeof(cliaddr);
			connfd=accept(listenfd,(SA *)&cliaddr,&clilen);
			for(i=0;i<FD_SETSIZE;i++)
				if(client[i]<0){
					client[i]=connfd;
					break;
				}
			if(i==FD_SETSIZE)
				err_quit("too many clients.\n");
			FD_SET(connfd,&sockset);
			if(connfd>maxfd)
				maxfd=connfd;
			if(i>maxcl)
				maxcl=i;
			if(--nready<=0)
				continue;
		}

		for(i=0;i<=maxcl;i++){
			if((sockfd=client[i])<0)
				continue;
			if(FD_ISSET(sockfd,&tempset)){
				if((n=read(sockfd,buf,MAXLINE))==0){
					close(sockfd);
					FD_CLR(sockfd,&sockset);
					client[i]=-1;
				}else{
					write(sockfd,buf,n);
				}
			}
			if(--nready<=0)
				break;
		}
	}


	/*
	 * First Edition
	 *

	int listenfd,connfd,errno;
	pid_t childpid,pid;
	socklen_t clilen;

	pid=getpid();
	printf("The server process id is %d\n",pid);
	struct sockaddr_in cliaddr,servaddr;

	listenfd=socket(AF_INET,SOCK_STREAM,0);
	printf("%d\n",listenfd);
	signal(SIGCHLD,sig_chld);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
	listen(listenfd,LISTENQ);
	for(;;){
		clilen=sizeof(cliaddr);
		if((connfd=accept(listenfd,(SA *)&cliaddr,&clilen))==-1){
			printf("%d\n",connfd);
			strerror(errno);
			exit(0);
		}
		printf("%d\n",connfd);
		if((childpid=fork())==0){
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}

	 *
	 */
}

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	pid_t pid=getpid();
again:
	while((n=read(sockfd,buf,MAXLINE))>0){
		writen(sockfd,buf,n);
		printf("%d\n",pid);
	}
	if(n<0&&errno==EINTR)
		goto again;
	else if(n<0)
		err_sys("str_echo:read error");
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	pid=wait(&stat);
	/*
	while((pid=waitpid(-1,&stat,WNOHANG))>0)
	{
		printf("child %d terminated\n",pid);
	}*/
	printf("%d\n",pid);
	return;
}
