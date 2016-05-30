#include "csapp.h"

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
	char strHolder[MAXLINE];
	char headerCopy[MAXLINE];
	rio_t rio;
	rio_t srio;
	ssize_t readBytes;

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
		
		
		printf("server connected:  %s (%s), port %u\n", hp->h_name, haddrp, client_port);
		//Host information.
		printf("Host official name: %s\n", hp->h_name);
		printf("Host alias: %s\n", hp->h_aliases[0]);
		printf("Host addr_type: %d\n", hp->h_addrtype);//AF_INET = 2
		printf("Host Length: %d\n", hp->h_length);
	
	
		Rio_readinitb(&rio,connfd);	
		Rio_readlineb(&rio,strHolder ,MAXLINE);		
		
		char* hold = strtok(strHolder," ");
		//printf("the total is %s",strHolder);
//		printf("the test is: %s \n",hold=strtok(NULL," "));
		

		int clientfd = Open_clientfd("www.ics.uci.edu","80");	
		Rio_readinitb(&srio,clientfd);
		//printf("about to send: %s\n",strHolder);
//		printf("writing\n");
		
//		Rio_writen(clientfd,"HEAD /~harris/test.html  HTTP/1.1\r\n",MAXLINE);
	//	Rio_readlineb(&srio,strHolder,MAXLINE);
	//	printf("%s",strHolder);
		//if HTTP 1.1 bad request 400
		//if HTTP 1.0 its ok.
		//
		//use HTTP METHODS described in tutorialspoint.
		//proxy should parse command to obtain
		////1. hostname.
		////2. directory.
//		Rio_writen(clientfd,"HEAD /~harris/test.html  HTTP/1.0\r\nhost: www.ics.uc.edu\r\n\r\n",200);
		Rio_writen(clientfd,"GET /~harris/test.html HTTP/1.0\r\nhost: www.ics.uci.edu\r\n\r\n",200);
		
		hp = Gethostbyname("www.google.com");
		printf("website official name: %s\n", hp->h_name);
		printf("header length: %d\n",hp->h_length);
		struct in_addr* internetAddress = (struct in_addr*)(hp->h_addr_list[0]);
		haddrp = inet_ntoa(*internetAddress);
		printf("website IP: %s\n",haddrp);
		printf("before read\n");
	//	Rio_readlineb(&srio, strHolder,200);
		Rio_readnb(&srio,strHolder,MAXLINE);
		
		memcpy(headerCopy,strHolder,MAXLINE);
		char* snippet = strstr(headerCopy,"Date:");
		char* sizeInBytes = strtok(snippet,"\n");
		printf("%s\n",sizeInBytes);
		snippet = strstr(headerCopy,"Content-Length:");
		sizeInBytes = strtok(snippet,"\n");
	 	printf("%s\n",snippet);
		printf("after read\n\n\n");
		
		printf("%s",strHolder);
		printf("it finished\n");
		pLog  = fopen("proxy.log", "a+");
		fprintf(pLog, " %s %s\n ",haddrp,hp->h_name);
		
		
		
		fclose(pLog);
		Close(connfd);
	}


	return 0;

}
