#include <assert.h>
#include <string>
#include <iconv.h>
#include <openssl/md5.h>
#include "utils.h"
#include "predefine.h"
#include "CFilter.h"
#include "CRawData.h"
#include "charset-detector.h"

#define MAX_HTML_LEN (16*1024*1024)

char charset[64]={0};

void str_replace(char *pInput, char *pOutput, char *pSrc, char *pDst)
{
	char    *pi, *po, *p;
	int     nSrcLen, nDstLen, nLen;

	pi = pInput;    
	po = pOutput;
	nSrcLen = strlen(pSrc);
	nDstLen = strlen(pDst);

	p = strstr(pi, pSrc);
	if(p)
	{
		while(p)
		{
			nLen = (int)(p - pi);
			memcpy(po, pi, nLen);
			memcpy(po + nLen, pDst, nDstLen);
			pi = p + nSrcLen;
			po = po + nLen + nDstLen;
			p = strstr(pi, pSrc);
		}
		strcpy(po, pi);
	}
	else
	{
		strcpy(po, pi);
	}
}

int convert2gbk(char *str)
{
	const char *detectedCharset;
	void *detector;

	detector = init_charset_detector();
	detectedCharset = detect_buffer_charset(detector, str, strlen(str));
	if(detectedCharset)
	{
		snprintf(charset, 64, "%s//ingore", detectedCharset);
		release_charset_detector(detector);
	}
	else
	{
		release_charset_detector(detector);
		return -1;
	}

	if (strcasestr(charset, "utf" )== 0 &&
		strcasestr(charset, "big" )== 0 )
	{   
		return -1;
	}

	iconv_t cd;
	size_t bufsize, inlen, oleft;
	char *pin, *pout, *outbuff;

	//cd = iconv_open((const char *)"gbk//ignore", (const char *)charset);
	cd = iconv_open((const char *)"gbk//ingore", (const char *)charset);
	if (cd == (iconv_t)-1) {
		//printf("iconv_open(%s, %s) failed\n", "gbk", tree_->charset_);
	}
	else
	{
		bufsize = (strlen(str) + 1024) * 2;
		outbuff=(char*)malloc(bufsize);
		assert(outbuff);

		bzero(outbuff, bufsize);
		pin = str;
		pout = outbuff;
		inlen = strlen(str);
		oleft = bufsize;

		//printf("title start\n");
		if(iconv(cd, &pin, &inlen, &pout, &oleft) != (size_t)(-1))
		{
			*(outbuff + (bufsize - oleft)) = 0;
			strcpy(str, outbuff);
		}
		else
		{
			*(outbuff + (bufsize - oleft)) = 0;
			strcpy(str, outbuff);
		}
		//printf("iconv title: %s\n", outbuff);
		free(outbuff);
	}

	return 0;
}

int get_html(char *rulefile, char *url, char *src)
{
	FILE *fp;
	int ret;
	char *p, tmpfile[128];
	unsigned long long key[2];

	if(*url == 0)
		return -1;

	assert(MD5((const unsigned char*)url, strlen(url), (unsigned char*)key));
	sprintf(tmpfile, "%llx.html", key[0]);

	sprintf(src, "wget -U \"Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)\" --timeout=30 -O %s \"%s\"", tmpfile, url);
	if(system(src) != 0)
		return -1;

	fp = fopen(tmpfile, "r");
	assert(fp);

	ret = fread(src, MAX_HTML_LEN-2, 1, fp);

	fclose(fp);
	unlink(tmpfile);

	if(ret < 1)
		return -1;

	return 0;
}

int main(int argc, char **argv)
{
	int ret;
	FILE *fp;
	char *src, *start=NULL, *end=NULL;
	char url[1024]={0};
	char tmpurl[4096]={0};

	if(argc != 3)
	{
		printf("%s rulefile link\n", argv[0]);
		exit(1);
	}

	src = (char*)calloc(MAX_HTML_LEN, 1);
	assert(src);

	ret = get_html(argv[1], argv[2], src);
	if(ret == -1)
		return 0;

	strcpy(url, argv[2]);

	CRuleMgr rulemgr;
	if (!rulemgr.loadRuleFromToFile(argv[1]))
	{
		std::cout<<"Can't open XML file!"<<std::endl;
		return 0;
	}

	CFilter* pFilter = new CFilter(rulemgr);

	RuleSets rule1;
	ret = rulemgr.getRuleSets(0, rule1);
	if(ret <= 0)
	{
		std::cout<<"Can't get start rule!"<<std::endl;
		return 0;
	}

	RuleSets rule;
	ret = rulemgr.getRuleSets(1, rule);
	if(ret <= 0)
	{
		std::cout<<"Can't get content rule!"<<std::endl;
		return 0;
	}

	RULESET *a;
	a = &rule1.front().m_ruleset;

	start = strstr(src, a->strBegin.c_str());
	if(start == NULL)
		start = src;

	end = strstr(start, a->strEnd.c_str());
	if(end != NULL)
		*end = 0;

	PAGELAYER pagelayer;
	pagelayer.nCurrIndex = 0;
	pagelayer.nCurrLayerIndex = 1;
	pagelayer.nFatherIndex = 1;
	pagelayer.sAddress = "";
	pagelayer.nRuleIndex = 0;

	pFilter->put(start, strlen(start), pagelayer);

	pFilter->process();

	CDataSets result;
	result = pFilter->getData();

	// --------------- output --------------------
	CData *dat;
	RULESET *b;

	dat = result.pop();
	b = &rule.front().m_ruleset;
	
	while (dat)
	{
		strcpy(src, dat->getData().c_str());
		char *t;
		t=strrchr(src, '<');
		if(t!=NULL)
			*t = 0;
		//printf("src : %s\n", src);

		if(b->strName.compare("url") == 0 || b->strName.compare("link") == 0)
		{
			if(b->strAssistantdata.compare("NULL") != 0)
				snprintf(tmpurl, 4096, "%s%s", b->strAssistantdata.c_str(), src);
			else
				snprintf(tmpurl, 4096, "%s", src);

			resovle_relative_URL(url, tmpurl, src);
		}
		else if(b->bStatic != true)
		{
			//convert2gbk(src);
		}

		std::cout<< b->strName << ": "<< src <<std::endl;

		b = b->pBrother;
		if(!b)
		{
			std::cout<< std::endl;
			b = &rule.front().m_ruleset;
		}
		dat = result.pop();
	}

	delete pFilter;

	//std::cout<<"neoparse has finished!"<<std::endl;
	return 0;
}

