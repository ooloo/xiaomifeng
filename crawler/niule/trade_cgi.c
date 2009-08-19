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
	int isocker,ContentLength;  /*数据长度*/ 
	int x=0,i=0; 
	char *p; 
	char *pRequestMethod;   /* METHOD属性值 */ 
	char InputBuffer[MAX_ITEM_LEN]={0};
	char recvBuff[4096];
	setvbuf(stdin,NULL,_IONBF,0);   /*关闭stdin的缓冲*/ 
	printf("Content-type: text/xml\n");  /*从stdout中输出，告诉Web服务器返回的信息类型*/
	printf("\n");                      /*插入一个空行，结束头部信息*/

	pRequestMethod = getenv("REQUEST_METHOD"); 
	if (pRequestMethod==NULL) {
		printf("REQUEST_METHOD error.\n");
		return 0; 
	} 
	if (strcmp(pRequestMethod,"POST")==0)
	{ 
		p = getenv("CONTENT_LENGTH");  /*从环境变量CONTENT_LENGTH中得到数据长度*/ 
		if (p!=NULL) { 
			ContentLength = atoi(p); 
		} else { 
			ContentLength = 0; 
		} 
		if (ContentLength>sizeof(InputBuffer)-1) { 
			ContentLength = sizeof(InputBuffer)-1; 
		} 
		i = 0; 
		while (i<ContentLength) {             /*从stdin中得到Form数据*/ 
			x = fgetc(stdin); 
			if (x==EOF) break; 
			InputBuffer[i++] = x; 
		} 
		InputBuffer[i] = '\0'; 
		ContentLength = i; 

	}
	else if (strcmp(pRequestMethod,"GET")==0) 
	{ 
		p = getenv("QUERY_STRING");   /*从环境变量QUERY_STRING中得到Form数据*/ 
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

