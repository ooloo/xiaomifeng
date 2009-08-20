#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "neopub.h"
#include "Hootcp.h"
#include "tradedecode.h"

int main() 
{
	int isocker,ContentLength;  /*���ݳ���*/ 
	int x=0,i=0; 
	char *p; 
	char *pRequestMethod;   /* METHOD����ֵ */ 
	char InputBuffer[MAX_ITEM_LEN]={0};
	char recvBuff[4096];
	setvbuf(stdin,NULL,_IONBF,0);   /*�ر�stdin�Ļ���*/ 
	printf("Content-type: text/xml\n");  /*��stdout�����������Web���������ص���Ϣ����*/
	printf("\n");                      /*����һ�����У�����ͷ����Ϣ*/

	pRequestMethod = getenv("REQUEST_METHOD"); 
	if (pRequestMethod==NULL) {
		printf("REQUEST_METHOD error.\n");
		return 0; 
	} 
	if (strcmp(pRequestMethod,"POST")==0)
	{ 
		p = getenv("CONTENT_LENGTH");  /*�ӻ�������CONTENT_LENGTH�еõ����ݳ���*/ 
		if (p!=NULL) { 
			ContentLength = atoi(p); 
		} else { 
			ContentLength = 0; 
		} 
		if (ContentLength>sizeof(InputBuffer)-1) { 
			ContentLength = sizeof(InputBuffer)-1; 
		} 
		i = 0; 
		while (i<ContentLength) {             /*��stdin�еõ�Form����*/ 
			x = fgetc(stdin); 
			if (x==EOF) break; 
			InputBuffer[i++] = x; 
		} 
		InputBuffer[i] = '\0'; 
		ContentLength = i; 

	}
	else if (strcmp(pRequestMethod,"GET")==0) 
	{ 
		p = getenv("QUERY_STRING");   /*�ӻ�������QUERY_STRING�еõ�Form����*/ 
		if(strlen(p) >= MAX_ITEM_LEN)
			p[MAX_ITEM_LEN] = 0;

		printf("<?xml version=\"1.0\" encoding=\"GBK\" ?>\n<root>\n");

		if (p!=NULL)
		{ 
			strncpy(InputBuffer,p,sizeof(InputBuffer)); 
			if((isocker = htcp_connect("127.0.0.1", 9627)) != -1)
			{
				htcp_send(isocker, InputBuffer, strlen(InputBuffer) + 1);
				while(htcp_recv(isocker, recvBuff, sizeof(recvBuff)) != -1)
				{
					printf("%s\n", recvBuff);
				}
				close(isocker);
			}
		}

		printf("</root>\n");
	}

	return 0;
}

