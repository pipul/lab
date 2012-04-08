#include <unp.h>

void str_cli(FILE *,int );

int main(int argc,char **argv)
{
	int sockfd[10],i;
	struct sockaddr_in servaddr;

	if(argc!=2)
		err_quit("usage:client <IPaddress>");

		bzero(&servaddr,sizeof(&servaddr));
		servaddr.sin_family=AF_INET;
		servaddr.sin_port=htons(SERV_PORT);
		inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	
		sockfd[i]=socket(AF_INET,SOCK_STREAM,0);
	for(i=0;i<2;i++){
		if(connect(sockfd[i],(SA *)&servaddr,sizeof(servaddr))==0);
			printf("%d\n",sockfd[i]);
	}

	str_cli(stdin,sockfd[0]);

	exit(0);
}

void str_cli(FILE *fp,int sockfd)
{
	int maxfdp1,stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;

	stdineof=0;
	FD_ZERO(&rset);
	for(;;){
		if(stdineof==0)
			FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdp1=max(fileno(fp),sockfd)+1;
		select(maxfdp1,&rset,NULL,NULL,NULL);

		if(FD_ISSET(sockfd,&rset)){
			if((n=read(sockfd,buf,MAXLINE))==0){
				if(stdineof==1)
					return;
				else
					err_quit("str_cli:server terminated prematurely.");
			}
			write(fileno(stdout),buf,n);
		}

		if(FD_ISSET(fileno(fp),&rset)){
			if((n=read(fileno(fp),buf,MAXLINE))==0){
				stdineof=1;
				shutdown(fileno(fp),SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			write(sockfd,buf,n);
		}
	}


	/* char send[MAXLINE],revc[MAXLINE];
	int maxfdp1;
	fd_set rset;
	FD_ZERO(&rset);

	for(;;){
		FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdp1=max(fileno(fp),sockfd)+1;
		select(maxfdp1,&rset,NULL,NULL,NULL);

		if(FD_ISSET(sockfd,&rset)){
			if(readline(sockfd,revc,MAXLINE)==0)
				err_quit("str_cli:recv error.");
			fputs(revc,stdout);
		}

		if(FD_ISSET(fileno(fp),&rset)){
			if(fgets(send,MAXLINE,fp)==0)
				return;
			writen(sockfd,send,sizeof(send));
		}
	}*/
	
	/*
	while(fgets(buf,MAXLINE,fp)!=NULL){
		writen(sockfd,buf,MAXLINE);
	}
	bzero(buf,MAXLINE);
	if(readline(sockfd,buf,MAXLINE)==0){
		if(errno==ECONNRESET)
			err_quit("str_cli:recv RESET");
		else if(errno==EPIPE)
			err_quit("str_cli:recv EPIPE");
		else
			err_quit("str_lic:recv OTHER");
	}
	fputs(buf,stdout);*/
}
