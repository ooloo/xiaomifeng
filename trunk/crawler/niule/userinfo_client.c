#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "Hootcp.h"
#include "neopub.h"

int main(int argc, char **argv)
{
	int len;
	char buf[MAX_ITEM_LEN];
	FILE *fp;
	int searchSock;

	if(argc != 2)
	{
		printf("%s filename", argv[0]);
		exit(1);
	}

	while((searchSock = htcp_connect("127.0.0.1", 9727)) == -1)
	{
		sleep(1);
	}

	fp = fopen(argv[0], "r");
	assert(fp);

	while(fgets(buf, MAX_ITEM_LEN, fp) != NULL)
	{
		if(htcp_send(searchSock, buf, strlen(buf) + 1) == -1)
		{
			printf("send failed. (%s)\n", buf);
			break;
		}
	}

	close(searchSock);
	
	return 0;
}
