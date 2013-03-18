#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>



#define BUFSIZE 1024
#define LOGFILE "oddns.log"

int getPrimaryIp(char *ipv4, size_t ip_len)
{
	int sock;
	const char *GoogleDnsIp = "8.8.8.8";
	const uint16_t GoogleDnsPort = 53;
	struct sockaddr_in serv, sockname;
	socklen_t socklen;

	if (ip_len <= 16)
		return(-1);
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		return(-1);
	
	memset(&serv, 0, sizeof(struct sockaddr_in));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(GoogleDnsIp);
	serv.sin_port = htons(GoogleDnsPort);

	if (-1 == connect(sock,(const struct sockaddr *)&serv,sizeof(struct sockaddr_in)))
		return(-1);

	if (-1 == getsockname(sock,(struct sockaddr *)&sockname, &socklen))
		return(-1);
	
	if (-1 == inet_ntop(AF_INET,&sockname.sin_addr,ipv4,ip_len))
		return(-1);

	close(sock);
	return(0);
}
	
	

int getexternalIp(char *ipv4)
{
	int n;
	struct addrinfo hints, *res, *ressave;
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(NULL,"80",&hints,&res)) !=0)
		return(-1);

	ressave = res;

	do {
		sprintf(ipv4,"%s\n",inet_ntoa(((struct sockaddr_in *)(res->ai_addr))->sin_addr));
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		return(-1);
	freeaddrinfo(ressave);
	return(0);
}


int tcpConnect(const char *host, const char *serv)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host,serv,&hints,&res)) != 0)
		return(-1);
	ressave = res;

	do {
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (sockfd < 0)
			continue;
		if (connect(sockfd,res->ai_addr,res->ai_addrlen) == 0)
			break;
		close(sockfd);
	} while ((res = res->ai_next) != NULL);


	/* Must check the result from while or do~while */

	if (res == NULL)
		return(-1);

	freeaddrinfo(ressave);
	return(sockfd);
}



int updateDns(char *domain, char *ipv4)
{

	int sockfd;
	FILE *logfp;
	int rd;

	char UA[BUFSIZE] = "";
	char recdata[BUFSIZE] = "";

	if ((logfp = fopen(LOGFILE,"a+")) == NULL)
		return(-1);
	
	while ((sockfd = tcpConnect("ddns.oray.com","80")) <= 0) {
		fputs("Can't connect to this server:ddns.oray.com:80\n",
		logfp);
		sleep(60);
	}

	strcpy(UA,"GET /ph/update?hostname=");
	strcat(UA,domain);
	strcat(UA,"&myip=");
	strcat(UA,ipv4);
	strcat(UA," HTTP/1.0\r\n");
	strcat(UA,"Host: ddns.oray.com\r\n");
	strcat(UA,"Connection: close\r\n");
	strcat(UA,"Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\r\n");
	strcat(UA,"Cache-Control: no-cache\r\n");
	strcat(UA,"Authorization: Basic cGlwdWxvcmc6YXNkZmFzZGY=\r\n");
	strcat(UA,"User-Agent: Oray\r\n\r\n");	
	strcat(UA,"Accept-Language: de,en;q=0.7,en-us;q=0.3\r\n\r\n");

	if ((rd = write(sockfd,UA,strlen(UA))) != strlen(UA)) {
		close(sockfd);
		fclose(logfp);
		return(-1);
	}

	while ((rd = read(sockfd,recdata,BUFSIZE)) <= 0);
	recdata[rd] = '\0';

	close(sockfd);
	fclose(logfp);
	return(0);
}

int getipv4(char *ipv4)
{
	int sockfd;
	FILE *logfp;
	int rd;

	char UA[BUFSIZE] = "";
	char recdata[BUFSIZE] = "";

	if ((logfp = fopen(LOGFILE,"a+")) == NULL)
		return(-1);

	while ((sockfd = tcpConnect("ddns.oray.com","80")) <= 0) {
		fputs("Can't connect to this server:oddns.oray.com:80\n",
		logfp);
		sleep(60);
	}

	strcpy(UA,"GET /checkip HTTP/1.1\r\n");
	strcat(UA,"Host: ddns.oray.com\r\n");
	strcat(UA,"Connection: close\r\n");
	strcat(UA,"Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\r\n");
	strcat(UA,"Cache-Control: no-cache\r\n");
	strcat(UA,"Accept-Language: de,en;q=0.7,en-us;q=0.3\r\n\r\n");

	if ((rd = write(sockfd,UA,strlen(UA))) != strlen(UA)) {
		close(sockfd);
		fclose(logfp);
		return(-1);
	}

	while ((rd = read(sockfd,recdata,BUFSIZE)) <= 0);
	recdata[rd] = '\0';
	close(sockfd);

	/* Pick up the really publicNetword ipaddress. */
	strncpy(ipv4,strstr(recdata,
		"Address: ") + 9,
		strstr(recdata,"</body>")-strstr(recdata,"Address: ") - 9);

	close(sockfd);
	fclose(logfp);
	return(0);
}



int main(int argc, char **argv)
{

	char domain[BUFSIZE] = "pipulorg.vicp.net";
	char ipv4new[BUFSIZE] = "", ipv4old[BUFSIZE] = "";

	if (argc != 1)
		/* error for oddns server start! */
		exit(0);
	
CHECK:
	/*
	 * First:
	 * Get the pubblic netword ipaddress.
	 * METHON: GET ORAY
	 *
	 */

	while (getipv4(ipv4new) == -1);


	/*
	 * Second:
	 * Update the domain's A record if the server
	 * ipaddress was change.
	 *
	 */
	
	if (strcmp(ipv4new,ipv4old) != 0) {
		while (updateDns(domain,ipv4new) == 1);
		strcpy(ipv4old,ipv4new);
	}

	sleep(60);
	goto CHECK;

	return(0);
}

