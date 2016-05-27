#include "csapp.h"

int main(int argc,char* argv[])
{
	//1. establish a connection (server)
	int listenfd, connfd, clientlen;
	char  *port;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp;
	unsigned short client_port;

	if(argc != 2)
	{
		fprintf(stderr,"Invalid arguments %s\n",argv[0]);
		exit(0);
	}

	port = argv[1];
	listenfd = Open_listenfd(port);
	
	while(1)
	{
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		client_port = ntohs(clientaddr.sin_port);
		
		printf("server connected to %s (%s), port %u\n",hp->h_name, haddrp, client_port);

		printf("official name: %s\n", hp->h_name);
		printf("Host alias: %s\n",hp->h_aliases[0]);
		printf("Host addr_type: %d\n",hp->h_addrtype);//AF_INET = 2
		printf("Host Length: %d\n", hp->h_length);:

		Close(connfd);
	} 	


	return 0;

}
