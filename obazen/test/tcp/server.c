#include <unp.h>

void sig_chld(int );
void str_echo(int );

int main()
{
	pid_t childpid;
	struct sockaddr_in cliaddr,seraddr;
	int clilen,listenfd,connfd;

	bzero(&cliaddr,sizeof(cliaddr));
	bzero(&seraddr,sizeof(seraddr));

	seraddr.sin_family=AF_INET;
	seraddr.sin_port=htons(SERV_PORT);
	seraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	bind(listenfd,(SA*)&seraddr,sizeof(seraddr));
	signal(SIGCHLD,sig_chld);
	listen(listenfd,LISTENQ);

	for(;;){
		clilen=sizeof(cliaddr);
		if((connfd=accept(listenfd,(SA *)&cliaddr,&clilen))<0){
			if(errno==EINTR)
				continue;
			else
				err_sys("accept error.\n");
		}
		if((childpid=fork())==0){
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
}

void str_echo(int sockfd){
	char buf[MAXLINE];
	int n;
again:
	if((n=read(sockfd,buf,MAXLINE))>0){
		write(sockfd,buf,MAXLINE);
		close(sockfd);
		//shutdown(sockfd,SHUT_RD);
		sleep(22);
	}
	if(n<0&&errno==EINTR)
		goto again;
	else if(n<0)
		err_sys("str_echo:read error.\n");
}

void sig_chld(int signo){
	pid_t pid;
	int stat;
	while((pid=waitpid(-1,&stat,WNOHANG))>0)
		printf("%d process close\n",pid);
	return;
}
