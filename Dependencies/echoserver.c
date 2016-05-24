#include "csapp.h"

void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	
	while((n = read(connfd,buf,sizeof(buf))) > 0)
	{
		printf("server received %d bytes\n", n);
		Write(connfd,buf,n);
	}
}

int main(int argc,char** argv)
{
	int listenfd, connfd, clientlen;
	char* port;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp;
	unsigned short client_port;

	port = argv[1];
	
	listenfd = open_listenfd(port);
	
	while(1)
	{
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		client_port = ntohs(clientaddr.sin_port);
		printf("server connected to %s (%s), port %u\n",hp->h_name,haddrp,client_port);
		printf("client len: %d\n",clientlen);
	
		puts("Host entry info:");
		printf("Host official name: %s\n",hp->h_name);
		printf("Host alias: %s\n",hp->h_aliases[0]);
		printf("Host addr_type: %d\n",hp->h_addrtype);
		printf("Host length: %d\n",hp->h_length);		

	
		echo(connfd);
		Close(connfd);
	}

	return 0;
}
