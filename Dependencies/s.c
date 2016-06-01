#define DIRSIZE 400
#define HOSTSIZE 400

#include "csapp.h"

struct httprequest
{
	char method[30]; //should be GET
	char httpType[50];
	char httpVersion[50];
	char directory[DIRSIZE];
	char hostname[HOSTSIZE];
	char port[20];
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
int parser(char* string, struct httprequest* req);
void printRequest(struct httprequest* request);
void formatRequest(struct httprequest* request,char* requestFormat);
void clear(struct httprequest* req);

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
        clear(&client_info);
        printRequest(&client_info);
	//	puts("Welcome to Proxy Server");
	//	puts("Press q to quit or press any other key to continue.");	
	//	char key = getchar();
	//	if(key == 'q')
	//		break;
		
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
        parser(request,&client_info);
		
		printf("\n\nCLIENT INFO DATA:\n\tmethod:%s\n\thttpType:%s\n\thttpversion:%s\n\tdirectory:%s\n\thostname:%s\n\tport:%s\n\n",
		                   client_info.method,client_info.httpType,client_info.httpVersion,
                           client_info.directory,client_info.hostname,client_info.port);
		char formattedRequest[MAXLINE];
		//sprintf(formattedRequest,"%s %s %s\r\nHost: %s\r\n\r\n",client_info.method,client_info.directory,client_info.httpVersion,client_info.hostname);		
		formatRequest(&client_info,formattedRequest);
        printf("%s\n",formattedRequest);
		if(client_info.hostname[strlen(client_info.hostname) - 1] == '\n')
		{
			printf("Host name has a new line removing it\n");
			client_info.hostname[strlen(client_info.hostname) - 1] = '\0';
			printf("removal: %s\n",client_info.hostname);
		}
		//open webpage from proxy, initialize, and write a HTTP request to it.
		char* http_port = "80";
		char* https_port = "443";
		//clientfd = Open_clientfd(client_info.hostname,client_info.port);
		clientfd = Open_clientfd(client_info.hostname,http_port);
		if (clientfd < 0 )
		{
			clientfd = Open_clientfd(client_info.hostname,https_port);
		}
		Rio_readinitb(&client_rio,clientfd);
		Rio_writen(clientfd,formattedRequest,MAXLINE);
		
		client_hp = Gethostbyname(client_info.hostname);
		client_in_addr = (struct in_addr*)(client_hp->h_addr_list[0]);
		client_ipAddress = inet_ntoa(*client_in_addr);
		
	//	bytesRead = Rio_readnb(&client_rio,header,MAXLINE);
		bytesRead = recv(client_rio.rio_fd,header,MAXLINE,0);	
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
				
		printf("%s\n",header);
		pLog = fopen("proxy.log","a+");
	//	fprintf(pLog,"%s %d\n",client_hp->h_name,bytesRead);
		fprintf(pLog,"%s : %s %s %d\n",date,client_ipAddress,client_hp->h_name,bytesRead);
	//	
		Rio_writen(connfd,header,MAXLINE);

	//	fflush(stdout);	
		fclose(pLog);
		Close(clientfd);
		Close(connfd);							
		
	}

	
	Close(listenfd);
	return 0;
}

int parser(char* string, struct httprequest* req)
{
     char holder[MAXLINE];
     int i;
     int spaces = 0;
     int j = 0;
     (*req).directory[0] = '/';
     for(i = 0; string[i] != '\0' && string[i] != '\n'; ++i)
     {
           if(string[i] == ' ')
           {
              spaces = 1;
              i++;        
              j=0;     
           }
           
           if (spaces == 0)
           {
              req->method[j] = string[i];
              ++j;
           }
           else if(spaces == 1)
           {
               int k =0;
               if(string[i+k] == 'h')
               {  holder[k] = string[i+k];++k;
                   if(string[i+k] == 't')
                   {holder[k] = string[i+k];++k;
                       if(string[i+k] == 't')
                       {holder[k] = string[i+k];++k;
                          if(string[i+k] == 'p')
                          {holder[k] = string[i+k];++k;
                             if(string[i+k] == 's')
                             {holder[k] = string[i+k];++k;
                                  if(string[i+k] == ':')
                                  {holder[k] = string[i+k];++k;
                                       if(string[i+k] == '/')
                                       {holder[k] = string[i+k];++k;
                                          if(string[i+k] == '/')
                                          {holder[k] = string[i+k];++k;
                                               strncpy((*req).httpType,holder,k);
                                               i += k;
                                           }   
                                       } 
                                   } 
                               } 
                               else if(string[i+k] == ':')
                               {holder[k] = string[i+k];++k;
                                  if(string[i+k] == '/')
                                   {holder[k] = string[i+k];++k;
                                       if(string[i+k] == '/')
                                       {holder[k] = string[i+k];++k;
                                          strncpy((*req).httpType,holder,k); 
                                          i += k;
                                       }  
                                   }   
                               }  
                           }   
                       }  
                   }  
               }
               int l = 0;
               while(string[i] != '\0' && string[i] != ' ' && string[i] != ':' && string[i] != '/')
               {
                 (*req).hostname[l] = string[i];
                 ++l;++i;
               }
               if(string[i] == ':')
               {
                   l=0;
                   ++i;
                   while(string[i] != '\0' && string[i] != ' ')
                   {
                       (*req).port[l] = string[i];
                       ++l;++i;
                   } 
               }  
               if(string[i] == '/')
               {
                   l=0;
                   while(string[i] != '\0' && string[i] != ' ')
                   {
                       (*req).directory[l] = string[i];
                       ++l;++i;
                   } 
               }   
               if(string[i] == '\0')
               {
                   return -1;                  
               }
               j = 0;
               ++spaces;
           }
           else if(spaces == 2)
           {
                if (string[i] != '\0' && string[i] != '\n' && string[i] != '\0')
                {
                     (*req).httpVersion[j] = string[i];   
                     ++j;          
                }
                
           }      
     }
     return 1;
}

void printRequest(struct httprequest* request)
{
	printf("method: %s\n",request->method);
	printf("httpver: %s\n",request->httpVersion);
	printf("protocol: %s\n",request->httpType);
	printf("hostname: %s\n",request->hostname);
	printf("port: %s\n",request->port);
	printf("directory: %s\n",request->directory);
}

void formatRequest(struct httprequest* request,char* requestFormat)
{
	if(strcmp(request->method,"GET") == 0)
	{
		sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVersion,request->hostname);
	}
	else if(strcmp(request->method,"HEAD") == 0)
	{
		sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVersion,request->hostname);
	}
	//else if(strcmp(request->method,"POST") == 0)//funky
	//{
	//	sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVersion,request->hostname);
	//}
	//else if(strcmp(request->method,"PUT") == 0)//funky
	//{
	//	sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVersion,request->hostname);
	//}
	//else if(strcmp(request->method,"DELETE") == 0)//funky
	//{
	//	sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVersion,request->hostname);
	//}
	else if(strcmp(request->method,"CONNECT") == 0)
	{
		sprintf(requestFormat,"%s %s %s\r\n",request->method,request->hostname,request->httpVersion);
	}
	else if(strcmp(request->method,"OPTIONS") == 0)//change hostname to "*" to refer to entire server.
	{
		sprintf(requestFormat,"%s %s %s\r\n",request->method,request->hostname,request->httpVersion);
	}
	else if(strcmp(request->method,"TRACE") == 0)//debugging
	{
		sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVersion,request->hostname);
	}
}

void clear(struct httprequest* req)
{
    memset(req->method,'\0',sizeof(req->method));
    memset(req->httpType,'\0',sizeof(req->httpType));
    memset(req->httpVersion,'\0',sizeof(req->httpVersion));
    memset(req->directory,'\0',sizeof(req->directory));
    memset(req->hostname,'\0',sizeof(req->hostname));
    memset(req->port,'\0',sizeof(req->port));
}
