#include "csapp.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

int main(int argc, char* argv[])
{
	//1. establish a connection (server)
	int listenfd, connfd, clientlen;
	char  *port;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp;
	unsigned short client_port;
	FILE* pLog;
	char strHolder[200];
	rio_t rio;
	rio_t srio;

	if (argc != 2)
	{
		fprintf(stderr, "Invalid arguments %s\n", argv[0]);
		exit(0);
	}

	port = argv[1];
	listenfd = Open_listenfd(port);

	while (1)
	{
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		client_port = ntohs(clientaddr.sin_port);
		
		
		printf("server connected to %s (%s), port %u\n", hp->h_name, haddrp, client_port);

		printf("official name: %s\n", hp->h_name);
		printf("Host alias: %s\n", hp->h_aliases[0]);
		printf("Host addr_type: %d\n", hp->h_addrtype);//AF_INET = 2
		printf("Host Length: %d\n", hp->h_length);
	
	
		Rio_readinitb(&rio,connfd);	
		Rio_readlineb(&rio,strHolder ,200);		
		
		char* hold = strtok(strHolder," ");
		//printf("the total is %s",strHolder);
		printf("the test is: %s \n",hold=strtok(NULL," "));
		

		int clientfd = Open_clientfd("www.ics.uci.edu","80");	
		Rio_readinitb(&srio,clientfd);
		//printf("about to send: %s\n",strHolder);
		Rio_writen(clientfd, "GET www.ics.uci.edu HTTP/1.1",200);
		Rio_readlineb(&srio, strHolder, 80);
		if(strlen(strHolder) != 0 &&  strHolder != NULL)
		{
			printf("from server %s",strHolder);
		}
		printf("it finished\n");
		pLog  = fopen("proxy.log", "w");
		fprintf(pLog, " %s %s ",haddrp,hp->h_name);
		
		
		
		fclose(pLog);
		Close(connfd);
	}


	return 0;

}
