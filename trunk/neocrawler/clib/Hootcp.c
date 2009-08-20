#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Hootcp.h"


unsigned long long pubkey=0x7F7F7F7FL;

int htcp_listen(int port, int queue)
{
	int fd;
	int on = 1;
	struct sockaddr_in sin;

	if (port <= 0 || port > 65535)
	{
		return -1;
	}

	if (queue <= 0)
	{
		queue = 5;
	}

	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "TCP listen failed. port[%d] queue[%d]\n", port, queue);
		return -1;
	}
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	struct linger linger = { 0 };
	linger.l_onoff = 1;
	linger.l_linger = 60;
	setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *) &linger, sizeof(linger));

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(port);
	if (bind(fd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		fprintf(stderr, "TCP listen failed. port[%d] queue[%d]\n", port, queue);
		htcp_close(fd);
		return -1;
	}

	if (listen(fd, queue) < 0)
	{
		fprintf(stderr, "TCP listen failed. port[%d] queue[%d]\n", port, queue);
		htcp_close(fd);
		return -1;
	}

	return fd;
}

int htcp_accept(int fd, struct sockaddr *sockAddr, socklen_t *addrLen)
{
	int clientFD;
	int keepAlive = 1;

	if(signal(SIGPIPE,SIG_IGN)==SIG_ERR)
	{
		exit(1);
	}

	for(;;)
	{
		clientFD=accept(fd,sockAddr,addrLen);
		if(clientFD==-1)
		{
			if(errno==EINTR)
				continue;
			else
			{
				fprintf(stderr,"Accept failed, error message is '%s'.\n",strerror(errno));
				return -1;
			}
		}

		break;
	}
	struct linger linger = { 0 };
	linger.l_onoff = 1;
	linger.l_linger = 60;
	setsockopt(clientFD, SOL_SOCKET, SO_LINGER, (const char *) &linger, sizeof(linger));

	setsockopt(clientFD, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
	return clientFD;
}

int htcp_connect(const char *host, int port)
{
	int fd, on = 1, keepAlive = 1;
	struct sockaddr_in sin;
	socklen_t sinlen = sizeof(sin);

	if(!host)
		return -1;

	if(signal(SIGPIPE,SIG_IGN)==SIG_ERR)
	{
		exit(1);
	}

	if( (fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "TCP connect failed: host[%s] port[%d]\n", host, port);
		return -1;
	}
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	//struct linger linger = { 0 };
	//linger.l_onoff = 1;
	//linger.l_linger = 60;
	//setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *) &linger, sizeof(linger));

	bzero(&sin, sinlen);
	sin.sin_family = AF_INET;
	inet_pton(AF_INET, host, &(sin.sin_addr));
	sin.sin_port = htons(port);

	if(connect(fd, (struct sockaddr*)&sin, sinlen) == -1)
	{
		fprintf(stderr, "TCP connect failed: host[%s] port[%d]\n", host, port);
		htcp_close(fd);
		return -1;
	}
	if (getsockname(fd, (struct sockaddr*)&sin, &sinlen) == -1)
	{
		fprintf(stderr, "TCP connect failed: fail to getsockname(%d)\n", fd);
		htcp_close(fd);
		return -1;
	}
	if (ntohs(sin.sin_port) == port)
	{
		fprintf(stderr, "TCP connect failed: sock tmp port == peer listen port.\n");
		htcp_close(fd);
		return -1;
	}
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));

	return fd;
}

int htcp_close(int fd)
{
	int ret;
	while ( (ret = close(fd)) == -1 && errno == EINTR) ;
	if (ret == -1)
		fprintf(stderr, "Close socket '%d' failed, error message is '%s'.\n", fd, strerror(errno));
	return ret;
}

int _htcp_send(int fd, void *buf, size_t len)
{
	int left = len;
	int count;
	char *p = (char*)buf;

	while(left>0)
	{
		count = send(fd,p,left,0);
		if(count==-1)
		{
			if(errno == EINTR)
				continue;

			fprintf(stderr,"Fail to send data, error message is '%s'.\n",strerror(errno));
			return -1;
		}
		else if(count==0)
		{
			fprintf(stderr,"Fail to send data.");
			return -1;
		}

		left-=count;
		p+=count;
	}

	return left;
}

int _htcp_recv(int fd, void *buf, size_t len)
{
	int left = len;
	int count;
	char *p = (char*)buf;

	while (left>0)
	{
		count = recv( fd, p, left ,0);
		if(count < 0)
		{
			if (errno ==EINTR) 
				continue;

			fprintf(stderr,"Fail to recv data, error message is '%s'.\n",strerror(errno));
			return -1;
		}
		else if( count==0 ) 
			break;

		p+=count;
		left-=count;
	}
	return (len-left);
}

int htcp_send(int fd, char *buf, int len)
{
	if(!buf)
	{
		return -1;
	}

	if(_htcp_send(fd,&pubkey,sizeof(unsigned long long))==-1)
	{
		return -1;
	}

	if(_htcp_send(fd,&len,sizeof(int))==-1)
	{
		return -1;
	}

	if(_htcp_send(fd,buf,len)==-1)
	{
		return -1;
	}	

	return len;
}

int htcp_recv(int fd, char *buf, int maxLen)
{
	int bufLen,count;
	unsigned long long tmpkey;

	if(!buf)
	{
		return -1;
	}

	if(_htcp_recv(fd,&tmpkey,sizeof(unsigned long long))!=sizeof(unsigned long long))
	{
		return -1;
	}

	if(tmpkey != pubkey)
	{
		fprintf(stderr, "Warning ,found wrong pack header\n");
		return -1;
	}

	if(_htcp_recv(fd,&bufLen,sizeof(int))!=sizeof(int))
	{
		return -1;
	}

	assert(bufLen>=0);
	if(bufLen>maxLen)
	{
		fprintf(stderr,"ur receive buffer is too small, ursize:%d needsize:%d.\n",bufLen,maxLen);
		return -1;
	}

	if((count=_htcp_recv(fd,buf,bufLen))==-1)
	{
		return -1;
	}

	if(count!=bufLen)
	{
		fprintf(stderr,"recv data error, recvsize:%d buffsize:%d.\n",count,bufLen);
		return -1;
	}

	return bufLen;
}

//-------------------------------------end------------------------------
