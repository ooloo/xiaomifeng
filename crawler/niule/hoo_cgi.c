#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

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
			int ret;
			char *t, *s, *tt, url[1024], usrid[128];
			FILE *fp;
			if((t=strstr(p, "url=")) != NULL)
			{
				if((s=strstr(p, "usrid=")) == NULL)
				{
					strcpy(usrid, "guest");
					snprintf(url, 1024, "%s",  t+strlen("url="));
				}
				else
				{
					if(t > s)
					{
						snprintf(url, 1024, "%s",  t+strlen("url="));
						*t = 0;
						snprintf(usrid, 128, "%s",  s+strlen("usrid="));
						usrid[strlen(usrid) - 1] = 0;
					}
					else
					{
						snprintf(usrid, 128, "%s",  s+strlen("usrid="));
						*s = 0;
						snprintf(url, 1024, "%s",  t+strlen("url="));
						url[strlen(url) - 1] = 0;
					}
				}
			}
				
			fp = fopen("/tmp/url.list", "a+");
			assert(fp);

			fprintf(fp, "%s\t%s\n", usrid, url);
			fclose(fp);

			printf("%s\t%s\n", usrid, url);
		}
	}

	return 0;
}

