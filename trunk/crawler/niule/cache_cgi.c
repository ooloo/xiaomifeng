#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "zlib.h"
#include "tradebdb.h"

int main() 
{
	int ContentLength;  /*���ݳ���*/ 
	int x=0,i=0; 
	char *p; 
	char *pRequestMethod;   /* METHOD����ֵ */ 
	char InputBuffer[2048];
	setvbuf(stdin,NULL,_IONBF,0);   /*�ر�stdin�Ļ���*/ 
	printf("Content-type: text/html\n");  /*��stdout�����������Web���������ص���Ϣ����*/
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

		if (p!=NULL)
		{
			int datLen;
			char *t, *tt;
			char dat[1024*1024];
			char html[1024*1024];
			unsigned long long key;
			uLong htmlLen;
			t = strstr(p, "url=");
			if(t != NULL)
			{
				t += strlen("url=");
				tt = strchr(t, '&');
				if(tt != NULL)
					*tt = 0;

				printf("%s\n\n", t);

				neo_bdbenv_init("/home2/snapshot/");
				key = url2id(t);
				assert(key);

				if(neo_bdb_bin_search_t(&key, dat, &datLen) == 0)
				{
					htmlLen = 1024*1024;
					assert(uncompress((Bytef*)html, &htmlLen, (Bytef*)dat, datLen) == Z_OK);

					html[htmlLen] = 0;

					printf("%s\n", html);
				}
			}
		}
	}

	return 0;
}

