/**
*@file tree_visit_for_link.cpp
*@brief  extract links from Dom Tree
************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree_visitor_for_link.h"
#include "URL.h"
#include "utils.h"



/* internal data structure for extract link */
typedef struct _link_data_t {
	link_t *link;
	int available;
	int end;
	char *anchor;
	int anchor_available;
	int anchor_end;
	char in_anchor;
	char in_option;
	char need_space;


    char base_host[1024];
    char base_URL[1024];

    int base_URL_end;  /* base URL ending with slash. */ 

} link_data_t;



int tree_visitor_for_link::resovle_url( const char *url, 
	char * result, void  * data_p)
{
	URL  url_parsed;

	if(strlen(url) >= 1024)
		return -1;

	link_data_t * data = (link_data_t*)data_p;

	url_parsed.setURL(url);

	if ( url_parsed.isAbsoluteURL() )
	{

		//strcpy( result, url);
		url_parsed.getCannonURL(result);
		return 0; /* noting to do.  */

	}




	char * path =  url_parsed.getPath();

	if ( path == NULL )
		return -1;

	if ( path[0] == '/' ) {

		 snprintf(result, 1024, "%s%s", data->base_host, path);
	} else {

		 if (data->base_URL_end == 1)
		 	snprintf(result, 1024, "%s%s", data->base_URL, path);
		 else
		 	snprintf(result, 1024, "%s/%s", data->base_URL, path);

	}

	myurltrim(result);

	return 1;
}


tree_visitor_for_link::tree_visitor_for_link(html_tree *pTree)
{
	tree = pTree; 

}



/*
 * get value of attribute name
 */ 
char *tree_visitor_for_link::get_attribute_value(html_tag_t *html_tag, char *name)
{
	html_attribute_t *attribute;
	for (attribute=html_tag->attribute; attribute!=NULL; attribute=attribute->next) {
		if (strcmp(attribute->name, name) == 0) {
			return attribute->value;
		}
	}
	return NULL;
}

/*
 * extract link from TAG_(A, AREA, IMG, LINK, FRAME, IFRMAE, EMBED)
 * and extract text from child tags of TAG_A as its anchor text
 */ 
int tree_visitor_for_link::pre_visit(html_tag_t *html_tag, void *result, int min_font)
{
	link_data_t *link_data;
	char *src;

	char URL_buffer[2048+1];
	char URL_buffer_encode[2048+1];


	link_data = (link_data_t *)result;
		
	if (link_data->available == link_data->end) {
		return VISIT_FINISH;
	}
	//assert(link_data->available < link_data->end);

	
	//anchor text
	if (html_tag->tag_type == TAG_PURETEXT && (link_data->in_anchor == 1 || link_data->in_option == 1)) 
	{
		/* copy text to the last link data. */
		snprintf(link_data->link[link_data->available -1].text, MAX_TEXT_LEN, "%s", html_tag->text);
	
	//	printf( "TAG_PURETEXT: %s \n",html_tag->text);
	
		return VISIT_NORMAL;
	}
	
	src = get_attribute_value(html_tag, "href");
	//no any link, javascript link, inner link.
	if (src == NULL || strncmp( src, "javascript:", 11)== 0 || strncmp( src, "#", 1 )== 0 ) {
		return VISIT_NORMAL;
	}

//	printf( "RAW LINKS: %s \n",src);

	int ret = resovle_url(src, URL_buffer,  link_data ) ;
	if ( ret == 1 || ret == 0 ) {

		reencode_escapes_URL( URL_buffer, URL_buffer_encode);

		src = URL_buffer_encode;

//	printf( "RAW LINKS2: %s \n",src);

		 URL  url_cannon;
		 url_cannon.setURL(src);
		 url_cannon.getCannonURL(src);

	}
	else if ( ret < -1 )
		return VISIT_NORMAL;
			

	//extract link
        //	assert (src != NULL);
	strcpy(link_data->link[link_data->available].url, src);

	link_data->link[link_data->available].text[0] = '\0';
	
	//prepare anchor text
	if (html_tag->tag_type == TAG_A) {
		link_data->anchor = link_data->link[link_data->available].text;
		link_data->anchor_available = 0;
		link_data->anchor_end = MAX_TEXT_LEN - 1;
		link_data->in_anchor = 1;
	} else if (html_tag->tag_type == TAG_OPTION) {
		link_data->anchor = link_data->link[link_data->available].text;
		link_data->anchor_available = 0;
		link_data->anchor_end = MAX_TEXT_LEN - 1;
		link_data->in_option = 1;
	}
	

	link_data->available++;
	return VISIT_NORMAL;
}

/*
 * finish visit TAG_A or TAG_OPTION
 */ 
int tree_visitor_for_link::post_visit(html_tag_t *html_tag, void *result)
{
	link_data_t *link_data;

	link_data = (link_data_t *)result;
	if (html_tag->tag_type == TAG_A) {
		link_data->in_anchor = 0;
	} else if (html_tag->tag_type == TAG_OPTION) {
		link_data->in_option = 0;
	}
	return VISIT_NORMAL;
}



/*
 * extract link from html_tree
 */ 
int tree_visitor_for_link::extract_link( link_t *link, int num)
{

    //    html_tree_t *html_tree = tree->get_tree();
	link_data_t link_data;

    	URL  base_URL_;

	link_data.end = num;
	link_data.available = 0;
	link_data.link = link;
	link_data.need_space = 0;
	link_data.in_anchor = 0;
	link_data.in_option = 0;

	link_data.base_URL_end = 0;

 if ( tree-> base_url_ && strlen ( tree-> base_url_) )
 {
	base_URL_.setBaseURL(tree->base_url_); 
        char *port;

        port = base_URL_.getPort();
	
        if ( port && ( atoi(port) != 80 ) )
                sprintf( link_data.base_host, "http://%s:%s", base_URL_.getHost(), port );
        else
                sprintf( link_data.base_host, "http://%s", base_URL_.getHost() );


        sprintf( link_data.base_URL, "%s%s", link_data.base_host, 
		base_URL_.getPath() );

	int len = strlen (link_data.base_URL );
	
	if ( link_data.base_URL[len-1] == '/')
		link_data.base_URL_end = 1;	
//	printf( "BASE LINKS: %s ; %s\n",tree-> base_url_, link_data.base_URL);
 } else {
	link_data.base_host[0]='\0';
	link_data.base_URL[0]='\0';

 }


	if (!tree->html_tree_visit(&pre_visit, &post_visit, &link_data)) {
		return -1;
	}
	//assert(link_data.available <= link_data.end);
	return link_data.available;
}



