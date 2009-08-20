/**
*@file tree_visit_for_content.cpp
*@brief  extract content from Dom Tree
************************************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tree_visitor_for_content.h"


typedef struct _node_attribute_t {
	int content_len;
	int anchor_len;
	char need;
} node_attribute_t;

typedef struct _content_data_t {
	node_attribute_t **block_array;
	char visit_title;
	char need_space;
	char *content;
	int available;
	int end;

	char in_anchor;
	unsigned int *filter_len;
	int pure_text_len;
	int anchor_text_len;
	int anchor_count;
	int label_count;

} content_data_t;


/*
 * extract PURETEXT, but skip first TITLE
 */ 
int tree_visitor_for_content::pre_visit(html_tag_t *html_tag, void *result, int min_font)
{
	content_data_t *content_data;
	char *src;
	char *space_str = " ";
	
#ifdef _FILTER_CONTENT_
	_html_attribute_t *aaa;	
	unsigned int i_flag = 0;
	memset( &aaa, 0, sizeof( html_attribute_t ) );
#endif
	
	content_data = (content_data_t *)result;
	
	if (html_tag->tag_type == TAG_TITLE && content_data->visit_title == 0) {
//		content_data->visit_title = 1;
		return VISIT_SKIP_CHILD;
		
	} else if (html_tag->tag_type == TAG_PURETEXT) {
		
#ifdef _FILTER_CONTENT_		
		if( html_tag->text == NULL )
		{
			return VISIT_SKIP_CHILD;
		}
		aaa=html_tag->attribute;	
		while( aaa != NULL )
		{
			if( !( strcmp( aaa->name, "b_hx" ) ) )
			{
				i_flag = 1;
				break;
			}
			aaa = aaa->next;
		}
		
		if( ( strstr( html_tag->text, "¡£" ) != NULL ) || ( strstr( html_tag->text, "£¬" ) != NULL ) ) 
		{
			src =	html_tag->text;
			content_data->label_count++;
		}	else if( i_flag )
					{
						src =	html_tag->text; 
					}
					else if( content_data->in_anchor == 0 )
						{
							src =	html_tag->text;
						}
						else if( content_data->in_anchor == 1 )
								{
									aaa=html_tag->attribute;	
									while( aaa != NULL )
									{
										
										if( ( strcmp( aaa->name, "font" ) == 0) && ( aaa->value != NULL ) )
										{
												if( ( atoi(aaa->value) > min_font ) )
												{
													src =	html_tag->text;
													break;
												}
												else
												{
														*content_data->filter_len += strlen( html_tag->text );
#ifdef _FTXT_PRINT_
														fprintf(stdout,"ftxt: %s\n", html_tag->text );
#endif
														//content_data->anchor_text_len += strlen( html_tag->text );
														return VISIT_SKIP_CHILD;
												}
										}
										aaa = aaa->next;
									}
									src =	html_tag->text;
								}	
								else
								{
									src =	html_tag->text;
								}
#else
		src =	html_tag->text;
#endif

		int len = strlen(src);

		if ( content_data->in_anchor == 1  )
			content_data->anchor_text_len += len;
		else
			content_data->pure_text_len += len;


		if ( content_data->in_anchor == 1 && len < 11 )
			src = NULL;

	} else if (html_tag->tag_type == TAG_OPTION){
		return VISIT_SKIP_CHILD;
	} else if (html_tag->tag_type == TAG_TD || html_tag->tag_type == TAG_BR || 
			html_tag->tag_type == TAG_P) {
		src = space_str;
	} else {
		src = NULL;
	}
	if (src != NULL) {
		if(content_data->available != content_data->end)
		{
			strcpy(content_data->content+content_data->available, src);
			content_data->available += strlen(src);
		}

	}
			
	//prepare anchor text
	if (html_tag->tag_type == TAG_A) {
		content_data->in_anchor = 1;
		content_data->anchor_count++;
	}

	return VISIT_NORMAL;
}

/*
 * finish visit TAG_A or TAG_OPTION
 */ 
int tree_visitor_for_content::post_visit(html_tag_t *html_tag, void *result)
{
	content_data_t *content_data;

	content_data = (content_data_t *)result;
	if (html_tag->tag_type == TAG_A) {
		content_data->in_anchor = 0;
	} 
	return VISIT_NORMAL;
}



/*
 * extract content from html_tree
 */ 
int tree_visitor_for_content::extract_content(unsigned int *filter_len, char *content, int size)
{
//    html_tree_t *html_tree = tree->get_tree();
	content_data_t content_data;
	
//	assert(content != NULL);
//	assert(size > 0);
	*filter_len = 0;
	*content = '\0';
	content_data.visit_title = 0;
	content_data.need_space = 0;
	content_data.available = 0;
	content_data.content = content;
	content_data.filter_len = filter_len;
	content_data.end = size - 1;

	content_data.in_anchor = 0;
	content_data.label_count = 0;
	content_data.pure_text_len = 0;
	content_data.anchor_text_len = 0;
	content_data.anchor_count = 0;

	if (!  tree->html_tree_visit(&pre_visit, &post_visit, &content_data)) {
                    return -1;
        }
        
	tree->page_type = 1;
			
	if( ( content_data.anchor_count > 600 ) || 
			( content_data.label_count < 3 ) || 
	( ( content_data.pure_text_len < 3072 ) && ( content_data.anchor_text_len*10/(content_data.pure_text_len+1) > 9 ) ) )
	{
		tree->page_type = 0;
	}
/*
	if ( //content_data.pure_text_len  >= 4096 ||
		( content_data.anchor_text_len/(content_data.pure_text_len+1) < 5 && 
		content_data.anchor_count < 100 ) ||
		( content_data.anchor_text_len/(content_data.pure_text_len+1) <1  && 
		content_data.anchor_count < 200 )	)
*/
	return 1;
}
