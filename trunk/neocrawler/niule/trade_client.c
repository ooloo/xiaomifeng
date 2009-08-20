#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include "zlib.h"
#include "Hootcp.h"

int searchSock;

int node_send(char *buff, int buffLen)
{
	if(htcp_send(searchSock, buff, buffLen) == -1)
	{
		close(searchSock);
		return -1;
	}
	return 0;
}

int node_recv(char *buff, int buffLen)
{
	int len;
	if((len=htcp_recv(searchSock, buff, buffLen)) == -1)
	{
		close(searchSock);
		return -1;
	}
	return len;
}

int main(int argc, char **argv)
{
	int len;
	char html[512*1024];

	if(argc != 2)
	{
		printf("%s type\n", argv[0]);
		exit(1);
	}

	if((searchSock = htcp_connect("127.0.0.1", 9627)) == -1)
	{   
		return 0;
	}

	len = atoi(argv[1]);
	htcp_send(searchSock, (void*)&len, 4);

	return 0;
}
