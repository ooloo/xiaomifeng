#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <strings.h>
#include "centerCache.h"

int g_basePort = 0;
int guideSendSocket = -1;
char guideHost[] = "127.0.0.1";

int guide_send(URLNODE_T *urlNode, char *url)
{
	int len;
	char *p,buf[2048];

	assert(urlNode && url);

	urlNode->urlLen = strlen(url);

	len = sizeof(URLNODE_T) + urlNode->urlLen;
	assert(len < 2048);

	memcpy(buf, urlNode, sizeof(URLNODE_T));
	memcpy(buf + sizeof(URLNODE_T), url, urlNode->urlLen);

	while(htcp_send(guideSendSocket, buf, len) == -1)
	{
		printf("guide send error, socker=%d\n", guideSendSocket);
		close(guideSendSocket);
		guideSendSocket = -1;

		sleep(1);
		while((guideSendSocket = htcp_connect(guideHost, g_basePort + 47)) == -1)
		{
			sleep(1);
			printf("htcp_connect guideSendSocket error.\n");
		}
		printf("htcp_connect guideSendSocket success.\n");
	}

	return 0;
}

int main(int argc, char **argv)
{
	int len;
	char buf[2048],url[MAX_URL_LEN];
	FILE *fp=NULL;
	URLNODE_T urlNode;

	if(argc != 4)
	{
		printf("usage: %s baseport MANUAL|NORMAL file\n", argv[0]);
		exit(1);
	}

	g_basePort = atoi(argv[1]);
	assert(g_basePort > 1024 && g_basePort < 65535);

	fp = fopen(argv[3], "r");
	if(fp == NULL)
	{
		printf("open (%s) error.\n", argv[3]);
		exit(1);
	}

	bzero(&urlNode, sizeof(URLNODE_T));

	if(strcasecmp(argv[2], "MANUAL") == 0)
		urlNode.urlType = MANUAL;
	else
		urlNode.urlType = NORMAL;

	while(fgets(buf, 2048, fp) != NULL)
	{
		if(strlen(buf) >= MAX_URL_LEN || strncmp(buf, "http://", 7) != 0)
		{
			printf("url error: %s", buf);
			continue;
		}

		sscanf(buf, "%s", url);
		if(strchr(url + 7, '/') == NULL)
			strcat(url, "/");

		printf("seed: %s\n", url);

		guide_send(&urlNode, url);
	}

	sleep(5);
	close(guideSendSocket);

	return 0;
}
