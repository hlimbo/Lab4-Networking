#include "csapp.h"

int main(int argc,char* argv[])
{
	int clientfd;
	char* port;
	char* host, buf[MAXLINE];
	rio_t rio;
	host = argv[1];
	port = argv[2];
	
	//can get a seg fault if host and port numbers not entered in correct format.
	//opens a proxy 
	clientfd = Open_clientfd(host,port);
	//initializes proxy stuff/
	Rio_readinitb(&rio,clientfd);
	printf("type: "); fflush(stdout);
	
	while(Fgets(buf,MAXLINE,stdin) != NULL)
	{
		//send line to server
		Rio_writen(clientfd,buf,strlen(buf));
		printf("MAXLENGTH: %d\n",MAXLINE);
		printf("we typed: %s\n",buf);
		printf("is %s == %s ? %d\n",buf,"QUIT",strcmp(buf,"QUIT\n"));
		if(strcmp(buf,"QUIT\n") == 0)
			break;
		//Receive line from server
		Rio_readlineb(&rio,buf,MAXLINE);
		printf("echo: ");
		Fputs(buf,stdout);
		printf("type:"); fflush(stdout);
	}
	
	//find a way to get server info.
	puts("Leaving server...");
	Close(clientfd);
	puts("Closing connection with server");
	return 0;
}
