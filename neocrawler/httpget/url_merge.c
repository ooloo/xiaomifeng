#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int http_3xx_merge(const char *redirect, char *url, int maxLen)
{
    char *p, *pp, *ppp;
    char tmpurl[maxLen];

    assert(url && redirect);

    //      http://开头的url不需要处理，直接返回
    if (!strncasecmp(redirect, "http://", 7)
        || !strncasecmp(redirect, "https://", 8))
    {
        return 0;
    }

    strcpy(tmpurl, url);

    //      "/fight/cs/"
    if (!strncasecmp(redirect, "/", 1))
    {
        if ((p = strchr(tmpurl + 7, '/')) != NULL)
        {
            *p = 0;
        }
        snprintf(url, maxLen, "%s/%s", tmpurl, redirect);
    }
    //  "./chn/index.htm"
    else if (!strncasecmp(redirect, "./", 2))
    {
        pp = tmpurl + 7;
        while ((p = strchr(pp + 1, '/')) != NULL)
        {
            pp = p;
        }
        if (*pp == '/')
        {
            *pp = 0;
        }
        snprintf(url, maxLen, "%s/%s", tmpurl, redirect + 1);
    }
    //  "../chn/index.htm"
    else if (!strncasecmp(redirect, "../", 3))
    {
        pp = ppp = tmpurl + 7;
        while ((p = strchr(pp + 1, '/')) != NULL)
        {
            ppp = pp;
            pp = p;
        }
        if (*pp == '/')
        {
            *pp = 0;
        }
        if (*ppp == '/')
        {
            *ppp = 0;
        }
        snprintf(url, maxLen, "%s/%s", tmpurl, redirect + 2);
    }
    //      others
    else
    {
        pp = tmpurl + 7;
        while ((p = strchr(pp + 1, '/')) != NULL)
        {
            pp = p;
        }
        if (*pp == '/')
        {
            *pp = 0;
        }
        snprintf(url, maxLen, "%s/%s", tmpurl, redirect);
    }

    return 0;
}


/*-----------------------------------------
 * script跳转发现
 * eg:
 * 		<script>
 * 		location="nju/nju.htm";
 * 		</script>
 * 参数:
 * 		content	: html content
 * 结果:
 * 		resultUrl: nju/nju.htm 
 *----------------------------------------*/

int html_script_redirect(const char *url, const char *content, int contentLen, char *resultUrl)
{
	int flag,len,jsFlag;
	char *p,*pp,*segment,*start,*end;
	char *t,save;
	char redirect[MAX_URL_LEN];

	if(!content || !*content || contentLen >= MAX_SHORTCONTENT_LEN)
		return -1;

	if((char*)strcasestr(content,"<frame") != NULL)
		return -1;

	*resultUrl	= 0;
	*redirect 	= 0;
	jsFlag		= 0;
	segment		= (char*)content;

	if(!strcasecmp(url+(strlen(url)-3),".js"))
		jsFlag = 1;
	
	while(1)
	{
		if(jsFlag != 1)
		{
			start=(char*)strcasestr(segment,"<script");
			if(start == NULL)
				return -1;

			end=(char*)strcasestr(start,"</script>");
			if(end == NULL)
				return -1;
		}
		else
		{
			start = (char*)content;
			end = (char*)(content+contentLen);
			if(segment == end)
				return -1;
		}

		segment = end;
		save = *end;
		*end = 0;
		flag = 0;
		pp	 = NULL;
		t	 = NULL;
		
		p=(char*)strcasestr(start,"location");
		//if(jsFlag != 1)
		//	t=strcasestr(start,"src");

		if(p==NULL && t!=NULL)
		{
			p = t;
		}
		else if(p!=NULL && t!=NULL)
		{
			if(t-start < p-start)
				p = t;
		}
		
		if(p != NULL)	// add at 2006/6/26, for js location
		{
			if((!isspace(*(p-1)) && *(p-1) != '.' ) 
					|| (!isspace(*(p+8)) && *(p+8) != '.' && *(p+8) != '=')) // 8 == strlen(location)
			{
				p = NULL;
			}
			else
			{
				while(*p != '\0' && *p != '=' && *p != '(')
					p++;

				if(*p == '\0')
					p = NULL;
				else
				{
					p++; // skip '=' or '('

					while(*p != '\'' && *p != '\"' && *p != '\0')
					{
						if(isspace(*p) == 0)
						{
							p = NULL;
							break;
						}
						p++;
					}
					if(p != NULL)
					{ if(*p == '\0') p = NULL; }
				}
			}
		}
		
		if(p != NULL)
		{
			//while(*p!='\r' && *p!='\n' && *p!='\0')
			while(*p!='\0' && p-start<=contentLen)
			{
				if(*p == '\'' || *p== '\"')
				{
					if(pp==NULL)
					{
						flag++;
						pp=p;
					}
					else
					{
						if(*pp == *p)
							flag++;
							
						break;
					}
				}
				p++;
			}
			*end = save;

			if(flag == 2 && pp != NULL)
			{
				len=p-pp-1;
				if(len >= MAX_URL_LEN-1 || len < 0)
					return -1;

				strncpy(redirect,pp+1,len);
				redirect[len]='\0';

				break;
			}
		}
		else
		{
			*end = save;
		}
	}

	if(*redirect == '\0')
		return -1;

	if(strcasecmp(redirect+(strlen(redirect)-3),".js") && t!=NULL)
		return -1;

	return 0;
}
//--------------------------END-----------------------------
