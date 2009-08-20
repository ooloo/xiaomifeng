#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tradedecode.h"


/*判断是否为十六进制字符。合法返回1，否则为0*/
static int isValidHexChar(char ch)
{
	return ( (ch>='0'&&ch<='9') || (ch>='a'&&ch<='f') || (ch>='A'&&ch<='F') ) ? 1 : 0;
}

/*URL特殊字符：% & < +*/

void decodeUrl(char *url)
{

	char ascii=0,ch,hex[3]="";
	int index=0,loop;

	if(url==NULL) 
		return;

	while(ch=url[index])
	{
		if(ch=='%')
		{
			hex[0]=url[index+1];
			hex[1]=url[index+2];

			sscanf(hex,"%x",&ascii);

			url[index]=ascii; /*Replace % With Ascii*/

			/*将ASCII等编码字符串转换成字符后，将后面的字符前移*/
			for(loop=index+1;url[loop];loop++) {
				url[loop]=url[loop+2];
				//if(url[loop]=='\0') break;
			}
		}
		index++;
	}
}



int isValidEncodeUrl(const char *url)
{
	char ch;
	int index=0,len;

	if(url==NULL) 
		return 0;

	len=strlen(url);

	while(ch=url[index])
	{
		if(ch=='%')
		{
			/*%(len-3) len-2 len-1*/
			if(index>len-3) 
				return 0;
			
			if(isValidHexChar(url[index+1]) && isValidHexChar(url[index+2]))
			{
				index+=2;
			}
			else 
				return 0;
		}
		index++;
	}
	return 1;
} 

/*
int main()
{
	char url[]= "ps2%D3%CE%CF%B7%CF%C2%D4%D8";
	
	printf("%s\n", url);
	
	if (isValidEncodeUrl(url) == 0)
		decodeUrl(url);

	printf("%s\n", url);
		
	return 0;
}
*/
