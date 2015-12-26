#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "utils.h"
#include "html_parser.h"

using namespace std;
html_parser parser;

typedef struct
{
	int linkNum;
	int pageType;
	char links[4096][1024];
}LINK_T;

void linkExtract_init()
{
	if ( parser.parse_init() < 0 )
	{
		fprintf(stdout, "parser init failed!\n");
		exit(1);
	}
	return;
}

int linkExtract_produce(char *pageUrl, char *myhtml, int htmlLen, void *links)
{
	int maxLinkNum=4096;
	LINK_T *reslinks;

	reslinks = (LINK_T*)links;

	parser.parse(pageUrl, myhtml, htmlLen);
	//cout<<"mylinks Size : "<<parser.links_num_<< "\t" << url << "\n";

	reslinks->pageType = parser.page_type;

	maxLinkNum = maxLinkNum < parser.links_num_ ? maxLinkNum : parser.links_num_;

	reslinks->linkNum = maxLinkNum;
	for(int index = 0;index<maxLinkNum;index++)
	{
		//cout<<"Index: "<<index<<"LINK: "<< parser.links_[index].url <<"\n"<<endl;
		snprintf(reslinks->links[index], 1024, "%s", parser.links_[index].url);
	}
	
	parser.parse_reset();

	return 0;
}

int textExtract_produce(char *pageUrl, char *myhtml, int htmlLen, char *title, char *context)
{
	parser.parse(pageUrl, myhtml, htmlLen);
	//cout<<"mylinks Size : "<<parser.links_num_<< "\t" << url << "\n";

	snprintf(title, 1024, "%s", parser.title_);
	snprintf(context, 512*1024, "%s", parser.content_);

	parser.parse_reset();

	return 0;
}


#ifdef __cplusplus
extern "C" {
#endif
	void mylinkExtract_init()
	{
		linkExtract_init();
	}

	int mylinkExtract_produce(char *pageUrl, char *myhtml, int htmlLen, void *links)
	{
		return linkExtract_produce(pageUrl, myhtml, htmlLen, links);
	}

	int mytextExtract_produce(char *pageUrl, char *myhtml, int htmlLen, char *title, char *context)
	{
		return textExtract_produce(pageUrl, myhtml, htmlLen, title, context);
	}

	int mylink_format(const char *base_url, const char *url, char *result)
	{
		return resovle_relative_URL(base_url, url, result);
	}
#ifdef __cplusplus
}
#endif

