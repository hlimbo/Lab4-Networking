#define DIRSIZE 350
#define HOSTSIZE 250

#include "csapp.h"

struct httprequest
{
	char method[10]; //should be GET
	char directory[DIRSIZE];
	char hostname[HOSTSIZE];
};

int checkArgs(int argc)
{
	if(argc != 2)
	{
		fprintf(stderr,"Invalid arguments\n");
		return - 1;
	}
	
	return 0;
}

//use: 
//type ./PROXY [PORT NUMBER]
int main(int argc,char** argv)
{
	//insufficient args leave program.
	if(checkArgs(argc) == -1)
	{
		return 0;
	}

	//proxy server 
	int listenfd;
	char* listen_port;
	
	//proxy connection to web browser or telnet info
	int connfd;
	int connlen;
	struct sockaddr_in connaddr;
	struct hostent* conn_hp;
	char* conn_addrp;
	unsigned short conn_port;
	rio_t conn_rio;
	
	//client (web browser or telnet) 
	int clientfd;
	struct hostent* client_hp;
	struct httprequest client_info;
	struct in_addr* client_in_addr;
	char* client_ipAddress;
	ssize_t bytesRead;
	rio_t client_rio;

	//http header
	char header[MAXLINE];
	//http request read from stream
	char request[MAXLINE];	
	//logging
	FILE* pLog;

	//http versions
	char* httpver1 = "HTTP/1.0";
	char* httpver2 = "HTTP/1.1";
			
	listen_port = argv[1];
	listenfd = Open_listenfd(listen_port);
	
	while(1)
	{
		puts("Welcome to Proxy Server");
		puts("Press q to quit or press any other key to continue.");	
		char key = getchar();
		if(key == 'q')
			break;
		
		//establishes a connection between client and proxy
		connlen = sizeof(connaddr);
		connfd = Accept(listenfd,(SA *)&connaddr, &connlen);
		conn_hp = Gethostbyaddr((const char*)&connaddr.sin_addr.s_addr, sizeof(connaddr.sin_addr.s_addr), AF_INET);
		conn_addrp = inet_ntoa(connaddr.sin_addr);
		conn_port = ntohs(connaddr.sin_port);
		
		printf("%s (%s:%u) connected to the proxy\n",conn_hp->h_name,conn_addrp,conn_port);
		
		//initializes and reads request from browser client
		Rio_readinitb(&conn_rio,connfd);
		Rio_readlineb(&conn_rio,request,MAXLINE);
		
		printf("Request read as: %s\n",request);
		
		//format request read. example: www.code.tutsplus.com/tutorials/http-headers-for-dummies--net-8039
		char* token;
		char requestCopy[MAXLINE];
		memcpy(requestCopy,request,MAXLINE);
		token = strtok(requestCopy,"/");
		if(token == NULL)
		{
			printf("Aborting... token is NULL\n");
			Close(connfd);
			break;
		}

		strncpy(client_info.method,"GET",3);
		strncpy(client_info.hostname,token,strlen(token));
		token = strchr(requestCopy,'/');
		if(token == NULL)
		{
			strncpy(client_info.directory,"/",1);
		}
		else
		{
			strncpy(client_info.directory,token,strlen(token));
		}

		char formattedRequest[MAXLINE];
		sprintf(formattedRequest,"%s %s %s\r\nhost: %s\r\n\r\n",client_info.method,client_info.directory,httpver2,client_info.hostname);		

		//open webpage from proxy, initialize, and write a HTTP request to it.
		char* http_port = "80";
		clientfd = Open_clientfd(client_info.hostname,http_port);
		Rio_readinitb(&client_rio,clientfd);
		Rio_writen(clientfd,formattedRequest,MAXLINE);
		
		client_hp = Gethostbyname(client_info.hostname);
		client_in_addr = (struct in_addr*)(client_hp->h_addr_list[0]);
		client_ipAddress = inet_ntoa(*client_in_addr);
		
		bytesRead = Rio_readnb(&client_rio,header,MAXLINE);
		
		//obtaining the date from the header.
		char headerCopy[MAXLINE];
		memcpy(headerCopy,header,MAXLINE);
		char* snippet;
		char date[300];
		snippet = strstr(headerCopy,"Date:");
		if(snippet == NULL)
		{
			snippet = strstr(headerCopy,"date:");
			if(snippet == NULL)
			{
				printf("Could not find the date in the html header\n");
				strncpy(date,"Date not found",20);		
			}
			else
			{
				snippet = strtok(snippet,"\r\n");
				if(snippet == NULL)
				{
					printf("strtok could not find the r or n\n");
					strncpy(date, "Date not found",20);
				}
				else
				{
					strncpy(date,snippet,strlen(snippet));
				}
			}
		}
		else
		{
			snippet = strtok(snippet,"\r\n");
			if(snippet == NULL)
			{
				printf("strtok could not find the r or n\n");
				strncpy(date,"Date not found",20);
			}
			else
			{
				strncpy(date,snippet,strlen(snippet));
			}
		}
	
		printf("it finished!\n");
		pLog = fopen("proxy.log","a+");
	//	fprintf(pLog,"%s %d\n",client_hp->h_name,bytesRead);
		fprintf(pLog,"%s : %s %s %d\n",date,client_ipAddress,client_hp->h_name,bytesRead);
		
		fclose(pLog);
		Close(clientfd);
		Close(connfd);							
		
	}

	
	Close(listenfd);
	return 0;
}
