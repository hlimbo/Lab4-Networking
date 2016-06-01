//sample: GET http://www.ics.uci.edu:80/~harris/test.html HTTP/1.1
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 8192
#define MAXLINE 8192
#define URL_SIZE 2048
//http/1.1 or http/1.0
//http:// or https://

struct Httprequest
{
	//required
	char method[50];
	char httpVer[20];
	char protocol[20];
	char hostname[500];
	char port[10];

	//optional
	char directory[500];
};


//return 0 on success
//return a negative value on error.
int parseRequest(const char* request, struct Httprequest* hrq)
{
	char url[URL_SIZE];
	char rCopy[SIZE];
	char* token;
	char* utoken;

	strncpy(rCopy,request,strlen(request));
	rCopy[strlen(request)] = '\0';

	token = strtok(rCopy," ");
	if(token == NULL)
		return -1;

	//method
	strncpy(hrq->method,token,strlen(token));
	printf("method: %s\n",hrq->method);
	token = strtok(NULL," ");
	if(token == NULL)
		return -2;

	//url
	strncpy(url,token,strlen(token));
	printf("url: %s\n",url);
	token = strtok(NULL," ");
	if(token == NULL)
		return -3;

	//http-ver
	strncpy(hrq->httpVer,token,strlen(token));
	printf("http version: %s\n",hrq->httpVer);

	//parsing the url. http://www.ics.uci.edu:80/~harris/test.html
	
	//protocol.
	if((utoken = strstr(url,"http://")) == NULL)
	{
		printf("no http found\n");
		if((utoken = strstr(url,"https://")) == NULL)
		{
			printf("no https found\n");
		}
		else
		{
			strncpy(hrq->protocol,"https://",8);
			utoken += 8;
		}
	}
	else
	{
		strncpy(hrq->protocol,"http://",7);
		utoken += 7;
	}

	//hostname
	if((utoken = strtok(utoken,":")) == NULL )
		return -4;

	strncpy(hrq->hostname,utoken,strlen(utoken));
	printf("hostname: %s\n",hrq->hostname);
	utoken += strlen(utoken) + 1;

	//port and directory
	if((utoken = strtok(utoken,"/")) == NULL)//if directory is null
	{
		printf("path is root\n");
		strncpy(hrq->port,utoken,strlen(utoken));
		strncpy(hrq->directory,"/",1);
		printf("port: %s\n",hrq->port);
	}
	else
	{
		strncpy(hrq->port,utoken,strlen(utoken));
		printf("port: %s\n",hrq->port);
		utoken += strlen(utoken);
		strncpy(hrq->directory,utoken,strlen(utoken));
		printf("directory: %s\n",hrq->directory);
	}




	return 0;
}

void printRequest(struct Httprequest* request)
{
	printf("method: %s\n",request->method);
	printf("httpver: %s\n",request->httpVer);
	printf("protocol: %s\n",request->protocol);
	printf("hostname: %s\n",request->hostname);
	printf("port: %s\n",request->port);
	printf("directory: %s\n",request->directory);
}

void formatRequest(struct Httprequest* request,char* requestFormat)
{
	if(strcmp(request->method,"GET") == 0)
	{
		sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVer,request->hostname);
	}
	else if(strcmp(request->method,"HEAD") == 0)
	{
		sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVer,request->hostname);
	}
	//else if(strcmp(request->method,"POST") == 0)//funky
	//{
	//	sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVer,request->hostname);
	//}
	//else if(strcmp(request->method,"PUT") == 0)//funky
	//{
	//	sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVer,request->hostname);
	//}
	//else if(strcmp(request->method,"DELETE") == 0)//funky
	//{
	//	sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVer,request->hostname);
	//}
	else if(strcmp(request->method,"CONNECT") == 0)
	{
		sprintf(requestFormat,"%s %s %s\r\n",request->method,request->hostname,request->httpVer);
	}
	else if(strcmp(request->method,"OPTIONS") == 0)//change hostname to "*" to refer to entire server.
	{
		sprintf(requestFormat,"%s %s %s\r\n",request->method,request->hostname,request->httpVer);
	}
	else if(strcmp(request->method,"TRACE") == 0)//debugging
	{
		sprintf(requestFormat,"%s %s %s\r\nHost: %s\r\n\r\n",request->method,request->directory,request->httpVer,request->hostname);
	}
}

int main(int argc,char** argv)
{
	struct Httprequest client_info;
	char requestFormat[SIZE];
	int status;
	char* sample = "GET http://www.ics.uci.edu:80/~harris/test.html HTTP/1.1";
	status = parseRequest(sample,&client_info);

	if(status == 0)
	{
		printf("Success!\n");
	//	printRequest(&client_info);
	//	formatRequest(&client_info,requestFormat);
	//	printf("%s",requestFormat);
	}


	return 0;
}