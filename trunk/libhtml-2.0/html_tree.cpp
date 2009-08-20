/**
*@file html_tree.cpp
*@brief  To build a html Dom Tree From the input html file
************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#include "html_tree.h"

#include "block_manager.h"

#define START_TAG 1
#define PURE_TEXT 2
#define END_TAG 3

#define HTML_NODE_UNIT_NUM 500
#define HTML_ATTRIBUTE_UNIT_NUM 1000

#define MAX_HTML_STACK_DEPTH 10000

#ifdef _FILTER_CONTENT_

#include <stack>		
#define  MAX_FOR_FONT_SIZE 10 
_font_size_t data[MAX_FOR_FONT_SIZE];
typedef stack<_font_size_t> STACK_INT;
STACK_INT st_font_tag;
STACK_INT st_font_css;

#include <ctype.h>
#include <iterator>
#define STYLE_SIZE 10000
unsigned int iCssNumber = 0;
vector <_css_font_t> css_font(0);

_css_font_t	name_value;

static int Compstr(const void *css_f1, const void *css_f2) {
	css_font_t *css_f_1 = (css_font_t *) css_f1;
	css_font_t *css_f_2 = (css_font_t *) css_f2;
	return strcmp(css_f_1->name, css_f_2->name);
}
#endif


static unsigned char TO_LOW[256]=
{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
   112,113,114,115,116,117,118,119,120,121,122, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
   112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
   128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
   144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
   160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255     };




static int IS_SPACE[256]=
{
    1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static int isspace(unsigned char ch)
{
    return IS_SPACE[ch];
}

char html_tree::tag_char[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, //!  %  /
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, //?  
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //A-Z
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,	0, 0, 0, 0, 
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //a-z
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/*
 *constructor for html_tree
 *
 */
html_tree::html_tree(int max)
{
    max_page_len = max;
    tree = NULL;

    base_url_[0] = '\0';
    memset( charset_,  0, 200);  

    base_url_[ MAX_URL_LEN+1] = '\0';

    html_tree_create();

}


html_tree::~html_tree()
{

	html_tree_clean();
	html_tree_del();

}

/*
 * creat html_tree struct
 * if success, return 0
 * if failed, return 1
 */ 
int html_tree::html_tree_create()
{
	html_tree_t *html_tree = NULL;
	
	//assert(max_page_len > 0);
	if (max_page_len <= 0)
		goto FAILED;

	//calloc html_tree
	html_tree = (html_tree_t *)calloc(sizeof(html_tree_t), 1);
	if (html_tree == NULL) {
	    // do log
		goto FAILED;
	}

	//init and next_input_char
	html_tree->next_input_char = NULL;
	
	//init next_input_node
	html_tree->next_input_node = NULL;

	//next tag code
	html_tree->next_tag_code = 1;

	//renew stack depth
	html_tree->stack_depth = 0;
	
	//init root
	memset(&html_tree->root, 0, sizeof(html_node_t));
	html_tree->root.html_tag.tag_type = TAG_ROOT;
	html_tree->root.html_tag.tag_code = 0;

#ifdef _FILTER_CONTENT_

	html_tree->font.font_size_depth = I_MAX;
	html_tree->css_depth.font_size_depth = I_MAX;
	min_font = 911;
	iCssNumber = 0;
	html_tree->strong_hx.i_strong_depth = I_MAX;
	
#endif

	//malloc nodes heap
	html_tree->nodes_heap = block_manager::block_heap_create(sizeof(html_node_t), HTML_NODE_UNIT_NUM);
	if (html_tree->nodes_heap == NULL) {
	    // do log
		goto FAILED;
	}
	
	//malloc attributes heap
	html_tree->attributes_heap = block_manager::block_heap_create(sizeof(html_attribute_t), HTML_ATTRIBUTE_UNIT_NUM);
	if (html_tree->attributes_heap == NULL) {
	    //do log
		goto FAILED;
	}
	
	//malloc string heap
	html_tree->string_heap = (char *)calloc(max_page_len+1, 1);
	if (html_tree->string_heap == NULL) {
	    //do log
		goto FAILED;
	}
	html_tree->available_buffer = html_tree->string_heap;
	html_tree->string_heap_size = max_page_len+1;
	
	//create tag type dict
	html_tree->tag_type_dict = create_tag_type_dict();
	if (html_tree->tag_type_dict == NULL) {
	    //do log
		goto FAILED;
	}

	//create child parent dict
	html_tree->child_parent_dict = create_child_parent_dict(html_tree->tag_type_dict);
	if (html_tree->child_parent_dict == NULL) {
	    //do log
		goto FAILED;
	}
	
	tree = html_tree;
	return 0;
			
FAILED:
	if (html_tree != NULL)
		html_tree_del_inner(html_tree);
	return 1;
}

/*
 * create a dict to translate tag name to tag type
 * note: html_tag_info_array is global array defined in html_dtd.cpp
 */ 
hash_dict_t  *html_tree::create_tag_type_dict()
{
	int i;
	int tag_num;
	hash_node_t node;
	hash_dict_t *dict;

	//create dict
	tag_num = html_tag_info_size;
	dict = hash_create(tag_num);
	if (dict == NULL)
		return NULL;
	
	//add all tags
	for (i=0; i<tag_num; i++) {
		creat_sign_md64((const char*)html_tag_info_array[i].name, strlen(html_tag_info_array[i].name), &node.sign1, &node.sign2);
		node.value = (html_tag_info_array[i].type);
		if (hash_insert(dict, &node) != 1) {
			hash_delete(dict);
			return NULL;
		}
	}
	return dict;
}

/*
 * create a dict to record all child parent relations of tags
 * note: html_tag_info_array is global array defined in html_dtd.cpp
 */ 
hash_dict_t * html_tree::create_child_parent_dict(hash_dict_t *tag_type_dict)
{
	int i,j;
	int tag_num;
	hash_dict_t *dict;
	hash_node_t node;
	int value1, value2;

	//create dict
	tag_num = html_tag_info_size;
	dict = hash_create(20*tag_num);
	if (dict == NULL)
		return NULL;
	
	//add all child parent pairs
	for (i=0; i<tag_num; i++) {
		creat_sign_md64(html_tag_info_array[i].name, strlen(html_tag_info_array[i].name), &node.sign1, &node.sign2);
		assert(hash_search(tag_type_dict, &node) == 1);
		value1 = node.value;
		
		for (j=0; html_tag_info_array[i].parents[j] != NULL; j++) {
			creat_sign_md64(html_tag_info_array[i].parents[j], 
							strlen(html_tag_info_array[i].parents[j]), &node.sign1, &node.sign2);
			assert(hash_search(tag_type_dict, &node) == 1);
			value2 = node.value;
			
			node.sign1 = value1;
			node.sign2 = value2;
			node.value = (u_int)-1;
			if (hash_insert(dict, &node) != 1) {
				return NULL;
			}
		}
	}
	return dict;
}

/*
 * set information of html_tree to initial state
 * free memory allocated by html_tree_parse
 */ 
void html_tree::html_tree_clean()
{

    	//renew nodes_heap
	block_manager::block_heap_renew(tree->nodes_heap);
	
	//renew attribute heap
	block_manager::block_heap_renew(tree->attributes_heap);
	
	//renew string heap
	tree->string_heap[0] = '\0';
	tree->available_buffer = tree->string_heap;
	
	//renew next char
	tree->next_input_char = NULL;
	
	//renew next input node
	tree->next_input_node = NULL;

	//renew tag code
	tree->next_tag_code = 1;
	
	//renew stack depth
	tree->stack_depth = 0;
	
	//renew root
	memset(&tree->root, 0, sizeof(html_node_t));
	tree->root.html_tag.tag_type = TAG_ROOT;
	tree->root.html_tag.tag_code = 0;
	
#ifdef _FILTER_CONTENT_

	tree->font.font_size_depth = I_MAX;
	tree->css_depth.font_size_depth = I_MAX;
	min_font = 911;
	iCssNumber = 0;
	tree->strong_hx.i_strong_depth = I_MAX;
	
#endif

}

/*
 * parse page into html_tree
 * html_tree must has been created and in initial state
 * if success, return 1
 * if failed, return 0
 */ 
int html_tree::html_tree_parse(const char * url, char *page, int page_size)
{
	int ret ;

  memset( charset_,  0, 200);  

  if ( !url)
		base_url_[0] = '\0';
	else
		strncpy(base_url_, url, MAX_URL_LEN);

	if (page_size > tree->string_heap_size) {
	    //do log
		return 0;
	}

	if (!is_clean_tree()) {
	    //do log
		return 0;
	}

#ifdef _FILTER_CONTENT_
	css_font.clear();
#endif
	
	tree->next_input_char = page;

	if (!html_tree_scan(tree)) {
	    //do log
		return 0;
	}
	
	ret =  html_tree_parse_sub_tree(tree, &tree->root);

	process_head_info();

	return ret;
}

/*
 * test if it is a clean tree
 */ 
int html_tree::is_clean_tree()
{
	if (tree->nodes_heap != NULL 
		&& !block_manager::is_clean_heap(tree->nodes_heap)) {
		return 0;
	}
	if (tree->attributes_heap != NULL 
		&& !block_manager::is_clean_heap(tree->attributes_heap)) {
		return 0;
	}
	if (tree->string_heap != NULL 
		&& tree->string_heap[0] != '\0') {
		return 0;
	}
	if (tree->string_heap != NULL 
		&& tree->available_buffer != tree->string_heap) {
		return 0;
	}
	if (tree->next_input_char != NULL) {
		return 0;
	}
	if (tree->next_input_node != NULL) {
		return 0;
	}
	if (tree->next_tag_code != 1) {
		return 0;
	}
	if (tree->stack_depth != 0) {
		return 0;
	}
	if (tree->root.parent != NULL) {
		return 0;
	}
	if (tree->root.next != NULL) {
		return 0;
	}
	if (tree->root.child != NULL) {
		return 0;
	}
	if (tree->root.html_tag.tag_name != NULL) {
		return 0;
	}
	if (tree->root.html_tag.attribute != NULL) {
		return 0;
	}
	if (tree->root.html_tag.text != NULL) {
		return 0;
	}
	if (tree->root.html_tag.tag_type != TAG_ROOT) {
		return 0;
	}
	if (tree->root.html_tag.tag_code != 0) {
		return 0;
	}
	return 1;
}

/*
 * before call this function, next_input_node has been prepared 
 * after quit this function, next_input_node will have been prepared
 */ 
int html_tree::html_tree_parse_sub_tree(html_tree_t *html_tree, html_node_t *html_node)
{
	html_node_t *child;
	int token_type;
	
	html_tree->stack_depth++;

#ifdef _FONT_SIZE
		html_tree->font_size_depth++;
#endif

	if (html_tree->stack_depth > MAX_HTML_STACK_DEPTH) {
	    //do log
		return 1;
	}
	
	while(1) {

		if (html_tree->next_input_node == NULL) {
			return 1;
		}
		
		token_type = get_node_token_type(html_tree->next_input_node);
		
		if (token_type == END_TAG) {
			/* 
			 * if is end tag of this node, end this subtree, prepare next node
			 */ 
			if (is_this_end_tag(html_node, html_tree->next_input_node)) {
				return html_tree_scan(html_tree);
			}
			/*
			 * if is parent end tag, end this subtree, return
			 */ 
			if (is_ancestor_end_tag(html_node, html_tree->next_input_node)) {
				return 1;
			}
			/*
			 * ignore this end tag, prepare next node, continue
			 */ 
			if (!html_tree_scan(html_tree)) {
				return 0;
			}
		} else {
			
			assert(token_type == PURE_TEXT || token_type == START_TAG);

			/*
			 * if is not child node, return 1
			 */ 
			child = html_tree->next_input_node;
			if (!is_child_tag(html_tree, html_node, child)) {
				return 1;
			}
			
			child->html_tag.tag_code = html_tree->next_tag_code++;
			append_child(html_node, child);
			
			/*
			 * if it is child node, prepare next node, recur
			 */ 
			if (!html_tree_scan(html_tree)) {
			    //do log
				return 0;
			}
				
			if (!html_tree_parse_sub_tree(html_tree, child)) {
				return 0;
			}
			html_tree->stack_depth--;
#ifdef _FONT_SIZE
	html_tree->font_size_depth--;
#endif
		}
	}
}

/*
 * get token type of html_node returned from html_tree_scan
 */ 
int html_tree::get_node_token_type(html_node_t *html_node)
{
	if (html_node->html_tag.tag_name == NULL) {
		assert(html_node->html_tag.tag_type == TAG_PURETEXT);
		return PURE_TEXT;
	}
	if (html_node->html_tag.tag_name[0] == '/') {
		assert(html_node->html_tag.tag_type == TAG_UNKNOWN);
		return END_TAG;
	}
	assert(tag_char[ (unsigned char)html_node->html_tag.tag_name[0] ] == 1);
	return START_TAG;
}

/*
 * if child node and parent node can be child parent pairs 
 * according child_parent_dict
 * if it is unknown tag, then child tag always is a child
 * of parent node
 */ 
int html_tree::is_child_tag(html_tree_t *html_tree, html_node_t *parent, html_node_t *child)
{
	hash_node_t node;

	if (parent->html_tag.tag_type == TAG_ROOT) {
		return 1;
	}
	// these tags  have no child
	if (parent->html_tag.tag_type == TAG_COMMENT || 
			parent->html_tag.tag_type == TAG_DOCTYPE || 
			parent->html_tag.tag_type == TAG_STYLE || 
			parent->html_tag.tag_type == TAG_SCRIPT || 
			parent->html_tag.tag_type == TAG_PURETEXT || 
			parent->html_tag.tag_type == TAG_BR	||
			parent->html_tag.tag_type == TAG_HR ||
			parent->html_tag.tag_type == TAG_WBR	||
			parent->html_tag.tag_type == TAG_COL	||
			parent->html_tag.tag_type == TAG_LINK	||
			parent->html_tag.tag_type == TAG_PARAM	||
			parent->html_tag.tag_type == TAG_IMG	||
			parent->html_tag.tag_type == TAG_COMMENT2	||
			parent->html_tag.tag_type == TAG_SPACER	||
			parent->html_tag.tag_type == TAG_BGSOUND	||
			parent->html_tag.tag_type == TAG_SOUND	
	   ) {
		return 0;
	}
	
	if (child->html_tag.tag_type == TAG_UNKNOWN || child->html_tag.tag_type == TAG_PURETEXT ||
			child->html_tag.tag_type == TAG_COMMENT || child->html_tag.tag_type == TAG_DOCTYPE || 
			child->html_tag.tag_type == TAG_STYLE) {
		return 1;
	}
	node.sign1 = child->html_tag.tag_type;
	node.sign2 = parent->html_tag.tag_type;
	return hash_search(html_tree->child_parent_dict, &node);
}

/*
 * add child to parent
 */ 
void html_tree::append_child(html_node_t *parent, html_node_t *child)
{
	html_node_t **last_child;
	
	child->parent = parent;
	child->next = NULL;
	child->child = NULL;

	for (last_child = &parent->child; *last_child != NULL; last_child = &(*last_child)->next)
		;
	*last_child = child;
}

/*
 * scan next token from next_input_char to next_input_node
 * detail information is saved in node_heap, attributes_heap and string_heap 
 * next_input_char is also updated
 * if success, return 1
 * if failed, return 0
 */ 
int html_tree::html_tree_scan(html_tree_t *html_tree)
{
	html_tag_t *html_tag;

	if (html_tree->next_input_char == NULL || *html_tree->next_input_char == '\0') {
		html_tree->next_input_node = NULL;
		return 1;
	}

	html_tree->next_input_node = (html_node_t *)block_manager::block_heap_allocate(html_tree->nodes_heap);
	if (html_tree->next_input_node == NULL) {
	    //do log
		return 0;
	}
	html_tag = &html_tree->next_input_node->html_tag;

	memset(html_tag, 0, sizeof(html_tag_t));
	
	//END TAG
	if (html_tree->next_input_char[0] == '<' && html_tree->next_input_char[1] == '/') {
		html_tree->next_input_char++;
		//tag name
		html_tag->tag_name = html_tree->available_buffer;
		while(!isspace((unsigned char)*html_tree->next_input_char) && *html_tree->next_input_char != '>') {
			*html_tree->available_buffer++ = TO_LOW[(unsigned char)*html_tree->next_input_char++];
		}
		*html_tree->available_buffer++ = '\0';

		html_tree->next_input_char = strchr(html_tree->next_input_char, '>');
		if (html_tree->next_input_char != NULL) {
			html_tree->next_input_char++;
		}
		html_tag->tag_type = TAG_UNKNOWN;
		return 1;
	//START TAG
	} else if (html_tree->next_input_char[0] == '<' && tag_char[(unsigned char)html_tree->next_input_char[1]] == 1) {
		html_tree->next_input_char++;
		
		//comment
		if (strncmp(html_tree->next_input_char, "!--", 3) == 0) {
			html_tree_scan_comment(html_tree);
			html_tag->tag_type = TAG_COMMENT;
			return 1;
		} 
	
		//doctype
		if (strncasecmp(html_tree->next_input_char, "!doctype ", 9) == 0) {
			html_tree_scan_doctype(html_tree);
			html_tag->tag_type = TAG_DOCTYPE;
			return 1;
		}
		
		//tag name
		html_tag->tag_name = html_tree->available_buffer;
		while(!isspace((unsigned char)*html_tree->next_input_char) && *html_tree->next_input_char != '>') {
			*html_tree->available_buffer++ = TO_LOW[(unsigned char)*html_tree->next_input_char++];
		}
		*html_tree->available_buffer++ = '\0'; 
		
#ifdef _FILTER_CONTENT_

	if( ( strcmp( html_tree->next_input_node->html_tag.tag_name, "h3" ) == 0 )
	 	||( strcmp( html_tree->next_input_node->html_tag.tag_name, "h2" ) == 0 ) 
	 	||( strcmp( html_tree->next_input_node->html_tag.tag_name, "h1" ) == 0 )
	 	||( strcmp( html_tree->next_input_node->html_tag.tag_name, "b" ) == 0 )
	 	||( strcmp( html_tree->next_input_node->html_tag.tag_name, "strong" ) == 0 ) 
	  )
	
	{
		html_tree->strong_hx.i_strong_depth = html_tree->stack_depth;
		html_tree->strong_hx.p_strong_value = html_tree->next_input_node->html_tag.tag_name;
	}
#endif
		
		//attributes
		if (html_tree_scan_attributes(html_tree) == 0) {
			return 0;
		}
		
		//tag end
		html_tree->next_input_char = strchr(html_tree->next_input_char, '>');
		if (html_tree->next_input_char != NULL) {
			html_tree->next_input_char++;
		}
		
		//text of script tag
		if (strcmp(html_tag->tag_name, "script") == 0) {
			html_tree_scan_script_text(html_tree);
			html_tag->tag_type = TAG_SCRIPT;
			return 1;
		} 
		//text of style tag
		if(strcmp(html_tag->tag_name, "style") == 0) {
			html_tree_scan_style_text(html_tree);
			html_tag->tag_type = TAG_STYLE;
			return 1;
		}
		
		html_tag->tag_type = get_tag_type(html_tree, html_tag->tag_name);
		return 1;
	//PURE TEXT
	} else {
		//text
		html_tag->text = html_tree->available_buffer;
		while (html_tree->next_input_char[0] != '\0' && 
				!(html_tree->next_input_char[0] == '<' && tag_char[(unsigned char)html_tree->next_input_char[1]]) ){
			*html_tree->available_buffer++ = *html_tree->next_input_char++;
		}
		*html_tree->available_buffer++ = '\0';	
		html_tag->tag_type = TAG_PURETEXT;
		
#ifdef _FILTER_CONTENT_

		if( ( I_MAX > html_tree->font.font_size_depth ) || ( I_MAX > html_tree->css_depth.font_size_depth ) ) 
		{
			html_tree_fontsize_attributes(html_tree);	
		}

	if( html_tree->stack_depth >= html_tree->strong_hx.i_strong_depth )
	{
		html_tree_strong_attributes(html_tree);
	}
	else if( html_tree->strong_hx.i_strong_depth != I_MAX )
	{
		html_tree->strong_hx.i_strong_depth = I_MAX;
	}
#endif
		
		return 1;
	}
	assert(0);
}


#ifdef _FILTER_CONTENT_
//add b_hx attribute.
int html_tree::html_tree_strong_attributes(html_tree_t *html_tree)
{
	html_attribute_t *html_attribute;
	
	html_attribute = (html_attribute_t *)block_manager::block_heap_allocate(html_tree->attributes_heap);
		if (html_attribute == NULL) {
			return 0;
		}
	memset(html_attribute, 0, sizeof(html_attribute_t));
			//insert
	html_attribute->next = html_tree->next_input_node->html_tag.attribute;
	html_tree->next_input_node->html_tag.attribute = html_attribute;
			
			//name and value
				
	html_attribute->name = "b_hx";
	html_attribute->value = html_tree->strong_hx.p_strong_value;
	return 1;
}

//add font attribute.
int html_tree::html_tree_fontsize_attributes(html_tree_t *html_tree)
{
	html_attribute_t *html_attribute;
	
	html_attribute = (html_attribute_t *)block_manager::block_heap_allocate(html_tree->attributes_heap);
		if (html_attribute == NULL) {
			return 0;
		}
	memset(html_attribute, 0, sizeof(html_attribute_t));
			//insert
	html_attribute->next = html_tree->next_input_node->html_tag.attribute;
	html_tree->next_input_node->html_tag.attribute = html_attribute;
			
			//name and value
				
	html_attribute->name = "font";
	
	//First
	if( (html_tree->stack_depth >= html_tree->font.font_size_depth) && (html_tree->stack_depth >= html_tree->css_depth.font_size_depth) ) 
	{
		if( html_tree->css_depth.font_size_depth >= html_tree->font.font_size_depth )
		{																					// html_tree->stack_depth < html_tree->font.font_size_depth
			if( !( st_font_css.empty() ) )
			{
				html_tree->css_depth = st_font_css.top();
				strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->css_depth.font_size );
				html_attribute->value = html_tree->next_input_node->html_tag.font_value;
			}
		}	else if ( html_tree->stack_depth >= html_tree->font.font_size_depth )
			{ 
				   																		// html_tree->stack_depth < html_tree->css_depth.font_size_depth
				if( !( st_font_tag.empty() ) )
				{
					html_tree->font = st_font_tag.top(); 
					strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->font.font_size );
					html_attribute->value = html_tree->next_input_node->html_tag.font_value;
				}
			}	
	} 
	//Second
	else if( (html_tree->stack_depth < html_tree->font.font_size_depth) && (html_tree->stack_depth >= html_tree->css_depth.font_size_depth) ) 
	{
		if( !( st_font_tag.empty() ) )
		{
			while( !( st_font_tag.empty() ) )
			{	
				st_font_tag.pop();
				if( !( st_font_tag.empty() ) )
				{
					html_tree->font = st_font_tag.top();
					if( html_tree->stack_depth >= html_tree->font.font_size_depth )
					{
						break;
					}
				}
			}
		}
		
		if( st_font_tag.empty() )				
		{
			if( I_MAX != html_tree->font.font_size_depth )
			{
				strcpy( html_tree->font.font_size,"999" );
				html_tree->font.font_size_depth = I_MAX;
			}
		}
			
		if( (html_tree->stack_depth < html_tree->font.font_size_depth) && (html_tree->stack_depth >= html_tree->css_depth.font_size_depth) ) 
		{
			if( !( st_font_css.empty() ) )
			{
				html_tree->css_depth = st_font_css.top(); 
				strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->css_depth.font_size );
				html_attribute->value = html_tree->next_input_node->html_tag.font_value;
			}
		}
		else
		{
			if ( html_tree->css_depth.font_size_depth >= html_tree->font.font_size_depth )
			{ 
				if( !( st_font_css.empty() ) )
				{
					html_tree->css_depth = st_font_css.top();
					strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->css_depth.font_size );
					html_attribute->value = html_tree->next_input_node->html_tag.font_value;
				}
			}
			else if( !( st_font_tag.empty() ) )
			{
				html_tree->font = st_font_tag.top(); 
				strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->font.font_size );
				html_attribute->value = html_tree->next_input_node->html_tag.font_value;
			}
		}
	}
	//Third
	else if( (html_tree->stack_depth >= html_tree->font.font_size_depth) && (html_tree->stack_depth < html_tree->css_depth.font_size_depth) ) 
	{
		if( !( st_font_css.empty() ) )
		{
			while( !( st_font_css.empty() ) )
			{	
				st_font_css.pop();
				if( !( st_font_css.empty() ) )
				{
					html_tree->css_depth = st_font_css.top();
					if( html_tree->stack_depth >= html_tree->css_depth.font_size_depth )
					{
						break;
					}
				}
			}
		}
		
		if( st_font_css.empty() )				
		{
			if( I_MAX != html_tree->css_depth.font_size_depth )
			{
				strcpy( html_tree->css_depth.font_size,"999" );
				html_tree->css_depth.font_size_depth = I_MAX;
			}
		}
			
		if( (html_tree->stack_depth >= html_tree->font.font_size_depth) && (html_tree->stack_depth < html_tree->css_depth.font_size_depth) ) 
		{
			if( !( st_font_tag.empty() ) )
			{
				html_tree->font = st_font_tag.top(); 
				strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->font.font_size );
				html_attribute->value = html_tree->next_input_node->html_tag.font_value;
			}
		}
		else
		{
			if ( html_tree->css_depth.font_size_depth >= html_tree->font.font_size_depth )
			{ 
				if( !( st_font_css.empty() ) )
				{
					html_tree->css_depth = st_font_css.top();
					strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->css_depth.font_size );
					html_attribute->value = html_tree->next_input_node->html_tag.font_value;
				}
			}
			else if( !( st_font_tag.empty() ) )
			{
				html_tree->font = st_font_tag.top(); 
				strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->font.font_size );
				html_attribute->value = html_tree->next_input_node->html_tag.font_value;
			}
		}
	}
	//Forth
	else
	{
		if( !( st_font_css.empty() ) )
		{
			while( !( st_font_css.empty() ) )
			{	
				st_font_css.pop();
				if( !( st_font_css.empty() ) )
				{
					html_tree->css_depth = st_font_css.top();
					if( html_tree->stack_depth >= html_tree->css_depth.font_size_depth )
					{
						break;
					}
				}
			}
		}
		
		if( st_font_css.empty() )				
		{
			if( I_MAX != html_tree->css_depth.font_size_depth )
			{
				strcpy( html_tree->css_depth.font_size,"999" );
				html_tree->css_depth.font_size_depth = I_MAX;
			}
		}

		if( !( st_font_tag.empty() ) )
		{
			while( !( st_font_tag.empty() ) )
			{	
				st_font_tag.pop();
				if( !( st_font_tag.empty() ) )
				{
					html_tree->font = st_font_tag.top();
					if( html_tree->stack_depth >= html_tree->font.font_size_depth )
					{
						break;
					}
				}
			}
		}
		
		if( st_font_tag.empty() )				
		{
			if( I_MAX != html_tree->font.font_size_depth )
			{
				strcpy( html_tree->font.font_size,"999" );
				html_tree->font.font_size_depth = I_MAX;
			}
		}
		
		if( !( st_font_css.empty() ) && !( st_font_tag.empty() ) )
		{
			if ( html_tree->css_depth.font_size_depth >= html_tree->font.font_size_depth )
			{ 
				if( !( st_font_css.empty() ) )
				{
					html_tree->css_depth = st_font_css.top();
					strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->css_depth.font_size );
					html_attribute->value = html_tree->next_input_node->html_tag.font_value;
				}
			}
			else if( !( st_font_tag.empty() ) )
			{
				html_tree->font = st_font_tag.top(); 
				strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->font.font_size );
				html_attribute->value = html_tree->next_input_node->html_tag.font_value;
			}
		}
		else if( !( st_font_css.empty() ) )
			{
				html_tree->css_depth = st_font_css.top();
				strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->css_depth.font_size );
				html_attribute->value = html_tree->next_input_node->html_tag.font_value;
			}
			else if( !( st_font_tag.empty() ) )
			{
				html_tree->font = st_font_tag.top(); 
				strcpy( html_tree->next_input_node->html_tag.font_value, html_tree->font.font_size );
				html_attribute->value = html_tree->next_input_node->html_tag.font_value;
			}
//		else
//		{
//			strcpy( html_tree->next_input_node->html_tag.font_value, "999" );
//			html_attribute->value = html_tree->next_input_node->html_tag.font_value;
//		}
				
	} //end if

	if( html_attribute->value != NULL )
	{
		int i_tmp = 0;
		i_tmp = atoi( html_attribute->value );
		if(  min_font > i_tmp )
			min_font = i_tmp;
	}	
	return 1;
}
#endif


/*
 * scan comment  end tag
 * return: 1: standard comment <!-- ... -->
 * 		   0: nonstandard comment <!-- ...>
 */ 
int html_tree::scan_comment_inner (html_tree_t *html_tree)
{
	int stand_flag = 0;
	char *p1 = NULL, *p2 = NULL;
	
	if (*html_tree->next_input_char == '>')
	{
		html_tree->next_input_char ++;
		return 0;
	}
	
	while (*html_tree->next_input_char != '\0' && strncmp(html_tree->next_input_char, "-->", 3) != 0) {
		if (stand_flag == 0 && *html_tree->next_input_char == '>')
		{
			stand_flag = 1;
			p1 = html_tree->next_input_char;
			p2 = html_tree->available_buffer;			
		}
		*html_tree->available_buffer++ = *html_tree->next_input_char++;
	}
	
	// standard comment, end with "-->"
	if (*html_tree->next_input_char != '\0') {
		html_tree->next_input_char += 3;
		return 1;
	}else if (stand_flag == 1 && *html_tree->next_input_char == '\0')	
	// nonstandard comment, find the first '>' after "<!--"
	{
		html_tree->next_input_char = p1;
		html_tree->available_buffer = p2;
		html_tree->next_input_char ++;
	}	
	return 0;
}

/*
 * scan comment tag 
 * notice: save comment name "!--" as "!-" , because:
 * 		last '-' is to reserve one byte for tag name end ('\0'), 
 * 		otherwise, html_tree->string_heap will be overflow in some condition
 */
void html_tree::html_tree_scan_comment(html_tree_t *html_tree)
{
	html_tree->next_input_node->html_tag.tag_name = html_tree->available_buffer;
	*html_tree->available_buffer++ = *html_tree->next_input_char++;
	*html_tree->available_buffer++ = *html_tree->next_input_char++;
//	*html_tree->available_buffer++ = *html_tree->next_input_char++;
	*html_tree->next_input_char++;
	*html_tree->available_buffer++ = '\0'; 

	assert(strcmp(html_tree->next_input_node->html_tag.tag_name, "!-") == 0);
	html_tree->next_input_node->html_tag.text = html_tree->available_buffer;
	scan_comment_inner (html_tree);
	*html_tree->available_buffer++ = '\0';
}

/*
 * scan doctype tag
 * notice: doctype's text maybe NULL, but not null string, if html segment is "<!doctype>" or "<!doctype >"
 */ 
void html_tree::html_tree_scan_doctype(html_tree_t *html_tree)
{
	//tag name
	html_tree->next_input_node->html_tag.tag_name = html_tree->available_buffer;
	while(!isspace((unsigned char)*html_tree->next_input_char) && *html_tree->next_input_char != '>') {
		*html_tree->available_buffer++ = TO_LOW[(unsigned char)*html_tree->next_input_char++];
	}
	*html_tree->available_buffer++ = '\0'; 
	assert(strcmp(html_tree->next_input_node->html_tag.tag_name, "!doctype") == 0);

	//text
	if (isspace((unsigned char)*html_tree->next_input_char) && *html_tree->next_input_char != '\0')
		html_tree->next_input_char++;
	
	if (*html_tree->next_input_char == '>')
		html_tree->next_input_node->html_tag.text = NULL;
	else
	{
		html_tree->next_input_node->html_tag.text = html_tree->available_buffer;
		while (*html_tree->next_input_char != '\0' && *html_tree->next_input_char != '>') {
			*html_tree->available_buffer++ = *html_tree->next_input_char++;
		}
		*html_tree->available_buffer++ = '\0';
	}
	    
	if (*html_tree->next_input_char != '\0') {
		html_tree->next_input_char++;
	}
}

/*
 * scan text of script tag
 */ 
void html_tree::html_tree_scan_script_text(html_tree_t *html_tree)
{
	/* flag 0: no </script> in quote ; 1: has </script> in quote
	 * input_p: pointer of html_tree->next_input_char where flag is been set
	 * available_p: pointer of html_tree->available_buffer where flag is been set
	 */
	int flag = 0;
	char *input_p = NULL, *available_p = NULL; 
	
	assert(strcmp(html_tree->next_input_node->html_tag.tag_name, "script") == 0);
	assert(html_tree->next_input_node->html_tag.text == NULL);
	
	html_tree->next_input_node->html_tag.text = html_tree->available_buffer;

	/* find normal end of script : </script> or the end of html '\0' */
	while (html_tree->next_input_char != NULL && *html_tree->next_input_char != '\0' && 
			strncasecmp(html_tree->next_input_char, "</script", 8) != 0) {
		/* 1. skip comment *
		if (strncmp (html_tree->next_input_char, "<!--", 4) == 0)
		{
			int tmp = scan_comment_inner (html_tree);
			/ * nonstandard comment and the end of comment is </script>, then go back to the </script>, 
			 * and the while statement will stop to finish the script. 
			 * /
			if (tmp == 0 && (html_tree->next_input_char - 9) && 
						strncasecmp (html_tree->next_input_char - 9, "</script", 8) == 0)
			{
				html_tree->next_input_char -= 9; // pointer is next to ">" of </script>
				html_tree->available_buffer -= 8;// poiter is to ">" of </script>
				break;
			}
			/ * after handle the comment, the poiter is next to the end of comment,
			 * then continue to handle script
			 * /
			continue;
		}
		*/

		/* 2. handle quote : ' and " */
		if ((*html_tree->next_input_char == '"' || *html_tree->next_input_char == '\'') && 
				*(html_tree->next_input_char-1) != '\\') {
			char end_quote = *html_tree->next_input_char;
			*html_tree->available_buffer++ = *html_tree->next_input_char++;
			// try to the end of quote
			while (*html_tree->next_input_char != '\0' && *html_tree->next_input_char != end_quote) {
				//ignore \/ , \" , \'
				if (*html_tree->next_input_char == '\\') {
					*html_tree->available_buffer++ = *html_tree->next_input_char++;
					if (*html_tree->next_input_char != '\0') {
						*html_tree->available_buffer++ = *html_tree->next_input_char++;
					}
				} else {
					// find the first </script> after quote, set flag.
					if(flag == 0 && strncasecmp (html_tree->next_input_char, "</script", 8) == 0)
					{
						flag = 1;
						input_p = html_tree->next_input_char;
						available_p = html_tree->available_buffer;
					}
					*html_tree->available_buffer++ = *html_tree->next_input_char++;
				}
			}
			// find the end of quote
			if (*html_tree->next_input_char == end_quote) {
				*html_tree->available_buffer++ = *html_tree->next_input_char++;
			}
		}else{// start without quote: go on to check the end of script
			*html_tree->available_buffer++ = *html_tree->next_input_char++;
		}
		// quote error somewhere which not a pair, trace back to the position setting flag.
		if (*html_tree->next_input_char == '\0' && flag == 1)
		{
			html_tree->available_buffer = available_p;
			html_tree->next_input_char = input_p;
		}
	}
	
	*html_tree->available_buffer++ = '\0';
	
}

/*
 * scan text of style tag
 */ 
void html_tree::html_tree_scan_style_text(html_tree_t *html_tree)
{
	assert(strcmp(html_tree->next_input_node->html_tag.tag_name, "style") == 0);
	assert(html_tree->next_input_node->html_tag.text == NULL);
	
	html_tree->next_input_node->html_tag.text = html_tree->available_buffer;
	while (html_tree->next_input_char != NULL && *html_tree->next_input_char != '\0' && 
		strncasecmp(html_tree->next_input_char, "</style", 7) != 0) {
		*html_tree->available_buffer++ = *html_tree->next_input_char++;
	}
	*html_tree->available_buffer++ = '\0';
#ifdef _FILTER_CONTENT_
	if( ( strlen(html_tree->next_input_node->html_tag.text) ) < STYLE_SIZE && 
		( html_tree->next_input_node->html_tag.text != NULL ) ) 
	{
		html_tree_scan_style_text_analyze_font(html_tree);
	}
#endif
}
	
#ifdef _FILTER_CONTENT_
void html_tree::html_tree_scan_style_text_analyze_font(html_tree_t *html_tree)
{
	_css_font_t css_font_tmp;
	char *p1 = 0;
	char *p2 = 0;
	char *p3 = 0;
	char *p4 = 0;
	char *p5 = 0;
	char *p6 = 0;
	char *p7 = 0;
	char *p8 = 0;
	char *p9 = 0;
	char text[STYLE_SIZE] = {0};
	char p_name[101] = {0};
	unsigned int space_flag = 0;
	unsigned int i_flag = 0;
	unsigned int i = 0, j = 0,k = 0;
	
	for ( i=0; html_tree->next_input_node->html_tag.text[i] != '\0'; i++ )
   {
		text[i] = tolower( html_tree->next_input_node->html_tag.text[i] );
   }
  text[i] = 0;
  
	memset( &css_font_tmp, 0, sizeof(_css_font_t) );

	if( text != NULL ) 
	{
		p1 = strtok( text, "}" );
	}

//loop_start	
	for( i=0; p1 != NULL && i == 0; i++ ) 
	{
		p4 = strstr( p1, "/*" );
		if( p4 != NULL )
		{
			p5 = strstr( p4, "*/" );
			if( p5 != NULL ) 
			{
				p5 = p5+2;
			}
			else
			{
				for( i=0; i<1500; i++ )
				{
					p1 = strtok( NULL, "}" );
					if( p1 == NULL )
					{
						break;
					}
					p5 = NULL;
					p5 = strstr( p1, "*/" );
					if( p5 != NULL ) 
					{
						p5 = p5+2;
						break;
					}
				}
			}
		} 
		if ( p5 != NULL )	p1 = p5;
		
		if( p1 != NULL ) 
		{		
			p2 = strstr( p1, "font-size:" );
		}
				
		if( p2 != NULL) 
		{
 			p6 = strstr(p1, "<!--" );
			if ( p6 != NULL )
			{
				p1 = p6+4;
			}
		
			p7 = strstr(p1, "-->" );
			if ( p7 != NULL )
			{
				p1 = p7+3;
			}

 			for( i=10,k=0; p2[i] != '\0' && ( isdigit( p2[i] ) || isblank( p2[i] ) ); i++) 
 			{
 				if( isblank( p2[i] ) ) continue;
 				css_font_tmp.value[k] = p2[i];
 				k++;
 				if( k > 2 ) break;
 			}
 			css_font_tmp.value[k] = '\0';
 			
 			int gao_tmp = atoi( css_font_tmp.value );    
 			if( gao_tmp <= 1 )
 			{
 				continue;
 			}			
 
 
			
			for( i=0,j=0; ( p1[i] != '{' ) && ( p1[i] != '\0' ); i++) 
			{
				if( p1[i] == ',' ) 
				{
					p_name[j] = '\0';
					for( k=0; k < iCssNumber; k++) 
					{ 
						if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
						{
							strcpy( css_font[k].value,css_font_tmp.value );	
							j = 0;
							break;
						}
					}
					space_flag = 0;
					for( k=0; k < iCssNumber; k++) 
					{ 
						if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
						{
							strcpy( css_font[k].value,css_font_tmp.value );	
							j = 0;
							break;
						}
					}
					i_flag = 0;			
					if ( j != 0 ) 
					{
						strcpy( css_font_tmp.name, p_name );
						css_font.push_back(css_font_tmp);
						++iCssNumber;
						memset( &css_font_tmp, 0, sizeof(_css_font_t) );
					}
					j = 0;
					continue;
				}
				
				if( p1[i] == ':' )
				{
					p8 = 0;
					p9 = 0;
					p3 = 0;
					p3 = p1+i;                             
					if( p3 != NULL )
					{
						p8 = strstr( p3, "," );
            p9 = strstr( p3, "{" ); 
					}
					if( p8 != NULL && p9 != NULL )                    
					{
						if( strlen(p8) > strlen(p9) )
							i = strlen( p1 ) - strlen( p8 ) - 1;
					}
					else if( p9 != NULL )
						{
							i = strlen( p1 ) - strlen( p9 ) - 1;
						}
						else
						{
							break;                               
						}
				}
				
				if( !isspace( p1[i] ) )
				{
					if( i_flag && j > 1 )
					{
						p_name[j] = ' ';
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
						p_name[j] = p1[i];
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
						i_flag = 0;
					}
					else
					{
						p_name[j] = p1[i];
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
					}
				}
				else
				{
					if( j > 1 )
					{
						i_flag = 1;
						space_flag += 1;
					}
					if( space_flag > 1 )
				{
					p8 = 0;
					p9 = 0;
					p3 = 0;
					p3 = p1+i;                             
					if( p3 != NULL )
					{
						p8 = strstr( p3, "," );
            p9 = strstr( p3, "{" ); 
					}
					if( p8 != NULL && p9 != NULL )                    
					{
						if( strlen(p8) > strlen(p9) )
							i = strlen( p1 ) - strlen( p8 ) - 1;
					}
					else if( p9 != NULL )
						{
							i = strlen( p1 ) - strlen( p9 ) - 1;
						}
						else
						{
							break;                               
						}
				}
					continue;
				}
			}   
			p_name[j] = '\0';
			for( k=0; k < iCssNumber; k++) 
			{ 
				if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
				{
					strcpy( css_font[k].value,css_font_tmp.value );	
					j = 0;
					break;
				}
			}
			space_flag = 0;
			for( k=0; k < iCssNumber; k++) 
			{ 
				if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
				{
					strcpy( css_font[k].value,css_font_tmp.value );	
					j = 0;
					break;
				}
			}			
			i_flag = 0;
			if ( j != 0 ) {
				strcpy( css_font_tmp.name, p_name );
				css_font.push_back(css_font_tmp);
				++iCssNumber;
				memset( &css_font_tmp, 0, sizeof(_css_font_t) );
			}
			j = 0;
		} else {
			p2 = 0;
			if( p1 != NULL ) 
			{
				p2 = strstr( p1, "font:" );	
			}
			if( p2 != NULL ) 
			{
 				p6 = strstr(p1, "<!--" );
				if ( p6 != NULL )
				{
					p1 = p6+4;
				}
		  	
				p7 = strstr(p1, "-->" );
				if ( p7 != NULL )
				{
					p1 = p7+3;
				}
 				for( i=5,k=0; p2[i] != '\0' && ( isdigit( p2[i] ) || isblank( p2[i] ) ); i++) {
 				if( isblank( p2[i] ) ) continue;
 				css_font_tmp.value[k] = p2[i];
 				k++;
 				if( k > 2 ) break;
 			}
 			css_font_tmp.value[k] = '\0';
 			
 			int gao_tmp = atoi( css_font_tmp.value );    
 			if( gao_tmp <= 1 )
 			{
 				continue;
 			} 			
 
			
			for( i=0,j=0; ( p1[i] != '{' ) && ( p1[i] != '\0' ); i++) 
			{
				if( p1[i] == ',' ) 
				{
					p_name[j] = '\0';
					for( k=0; k < iCssNumber; k++) 
					{ 
						if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
						{
							strcpy( css_font[k].value,css_font_tmp.value );	
							j = 0;
							break;
						}
					}
					space_flag = 0;
					i_flag = 0;						
					if ( j != 0 ) {
						strcpy( css_font_tmp.name, p_name );
						css_font.push_back(css_font_tmp);
						++iCssNumber;
						memset( &css_font_tmp, 0, sizeof(_css_font_t) );
					}
					j = 0;
					continue;
				}
				
				if( p1[i] == ':' )
				{
					p8 = 0;
					p9 = 0;
					p3 = 0;
					p3 = p1+i;                             
					if( p3 != NULL )
					{
						p8 = strstr( p3, "," );
            p9 = strstr( p3, "{" ); 
					}
					if( p8 != NULL && p9 != NULL )                    
					{
						if( strlen(p8) > strlen(p9) )
							i = strlen( p1 ) - strlen( p8 ) - 1;
					}
					else if( p9 != NULL )
						{
							i = strlen( p1 ) - strlen( p9 ) - 1;
						}
						else
						{
							break;                               
						}
				}
				
				if( !isspace( p1[i] ) )
				{
					if( i_flag && j > 1 )
					{
						p_name[j] = ' ';
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
						p_name[j] = p1[i];
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
						i_flag = 0;
					}
					else
					{
						p_name[j] = p1[i];
						j++;
						if( j >= 100 ) 
						{  
							j = 0;
							continue;
						}
					}
				}
				else
				{
					if( j > 1 )
					{
						i_flag = 1;
						space_flag += 1;
					}
					if( space_flag > 1 )
				{
					p8 = 0;
					p9 = 0;
					p3 = 0;
					p3 = p1+i;                             
					if( p3 != NULL )
					{
						p8 = strstr( p3, "," );
            p9 = strstr( p3, "{" ); 
					}
					if( p8 != NULL && p9 != NULL )                    
					{
						if( strlen(p8) > strlen(p9) )
							i = strlen( p1 ) - strlen( p8 ) - 1;
					}
					else if( p9 != NULL )
						{
							i = strlen( p1 ) - strlen( p9 ) - 1;
						}
						else
						{
							break;                               
						}
				}
					continue;
				}
			}
			p_name[j] = '\0';
			for( k=0; k < iCssNumber; k++) 
			{ 
				if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
				{
					strcpy( css_font[k].value,css_font_tmp.value );	
					j = 0;
					break;
				}
			}
			space_flag = 0;
			i_flag = 0;
			if ( j != 0 ) {
				strcpy( css_font_tmp.name, p_name );
				css_font.push_back(css_font_tmp);
				++iCssNumber;
				memset( &css_font_tmp, 0, sizeof(_css_font_t) );
			}
			j = 0;
		}
	 }
	}
//loop_end

//loop_start	
	while( p1 != NULL ) 
	{
		p1 = strtok( NULL, "}" );
		if( p1 != NULL )
		{	
			p4 = 0;
			p5 = 0;
  		
			p4 = strstr( p1, "/*" );
			if( p4 != NULL )
			{
				p5 = strstr( p4, "*/" );
				if( p5 != NULL ) 
				{
					p5 = p5+2;
				}
				else
				{
					for( i=0; i<1500; i++ )
					{
						p1 = strtok( NULL, "}" );
						if( p1 == NULL )
						{
							break;
						}
						p5 = NULL;
						p5 = strstr( p1, "*/" );
						if( p5 != NULL ) 
						{
							p5 = p5+2;
							break;
						}
					}
				}
			}
			if ( p5 != NULL )	p1 = p5;
		}
		
		p2 = 0;
		if( p1 != NULL ) 
		{		
			p2 = strstr( p1, "font-size:" );
		}
				
		if( p2 != NULL ) 
		{
			p6 = strstr(p1, "<!--" );
			if ( p6 != NULL )
			{
				p1 = p6+4;
			}
		
			p7 = strstr(p1, "-->" );
			if ( p7 != NULL )
			{
				p1 = p7+3;
			}
 			for( i=10,k=0; p2[i] != '\0' && ( isdigit( p2[i] ) || isblank( p2[i] ) ); i++) 
 			{
 				if( isblank( p2[i] ) ) continue;
 				css_font_tmp.value[k] = p2[i];
 				k++;
 				if( k > 2 ) break;
 			}
 			css_font_tmp.value[k] = '\0';
 			
 			int gao_tmp = atoi( css_font_tmp.value );    
 			if( gao_tmp <= 1 )
 			{
 				continue;
 			}			
 
 
			
			for( i=0,j=0; ( p1[i] != '{' ) && ( p1[i] != '\0' ); i++ ) 
			{
				if( p1[i] == ',' ) 
				{
					p_name[j] = '\0';
					for( k=0; k < iCssNumber; k++) 
					{ 
						if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
						{
							strcpy( css_font[k].value,css_font_tmp.value );	
							j = 0;
							break;
						}
					}
					space_flag = 0;
					i_flag = 0;						
					if ( j != 0 ) 
					{
						strcpy( css_font_tmp.name, p_name );
						css_font.push_back(css_font_tmp);
						++iCssNumber;
						memset( &css_font_tmp, 0, sizeof(_css_font_t) );
					}
					j = 0;
					continue;
				}
				
				if( p1[i] == ':' )
				{
					p8 = 0;
					p9 = 0;
					p3 = 0;
					p3 = p1+i;                             
					if( p3 != NULL )
					{
						p8 = strstr( p3, "," );
            p9 = strstr( p3, "{" ); 
					}
					if( p8 != NULL && p9 != NULL )                    
					{
						if( strlen(p8) > strlen(p9) )
							i = strlen( p1 ) - strlen( p8 ) - 1;
					}
					else if( p9 != NULL )
						{
							i = strlen( p1 ) - strlen( p9 ) - 1;
						}
						else
						{
							break;                               
						}
				}
				
				if( !isspace( p1[i] ) )
				{
					if( i_flag && j > 1 )
					{
						p_name[j] = ' ';
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
						p_name[j] = p1[i];
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
						i_flag = 0;
					}
					else
					{
						p_name[j] = p1[i];
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
					}
				}
				else
				{
					if( j > 1 )
					{
						i_flag = 1;
						space_flag += 1;
					}
					if( space_flag > 1 )
				{
					p8 = 0;
					p9 = 0;
					p3 = 0;
					p3 = p1+i;                             
					if( p3 != NULL )
					{
						p8 = strstr( p3, "," );
            p9 = strstr( p3, "{" ); 
					}
					if( p8 != NULL && p9 != NULL )                    
					{
						if( strlen(p8) > strlen(p9) )
							i = strlen( p1 ) - strlen( p8 ) - 1;
					}
					else if( p9 != NULL )
						{
							i = strlen( p1 ) - strlen( p9 ) - 1;
						}
						else
						{
							break;                               
						}
				}
					continue;
				}
			}   
			p_name[j] = '\0';
			for( k=0; k < iCssNumber; k++) 
			{ 
				if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
				{
					strcpy( css_font[k].value,css_font_tmp.value );	
					j = 0;
					break;
				}
			}
			space_flag = 0;
			i_flag = 0;
			if ( j != 0 ) {
				strcpy( css_font_tmp.name, p_name );
				css_font.push_back(css_font_tmp);
				++iCssNumber;
				memset( &css_font_tmp, 0, sizeof(_css_font_t) );
			}
			j = 0;
		} 
		else 
		{
			p2 = 0;
			if( p1 != NULL ) 
			{
				p2 = strstr( p1, "font:" );	
			}
			if( p2 != NULL )
			{
				p6 = strstr(p1, "<!--" );
				if ( p6 != NULL )
				{
					p1 = p6+4;
				}
		  	
				p7 = strstr(p1, "-->" );
				if ( p7 != NULL )
				{
					p1 = p7+3;
				}
 				for( i=5,k=0; p2[i] != '\0' && ( isdigit( p2[i] ) || isblank( p2[i] ) ); i++) {
 				if( isblank( p2[i] ) ) continue;
 				css_font_tmp.value[k] = p2[i];
 				k++;
 				if( k > 2 ) break;
 				}
 				css_font_tmp.value[k] = '\0';
 			
 				int gao_tmp = atoi( css_font_tmp.value );    
 				if( gao_tmp <= 1 )
 				{
 					continue;
 				} 			
				for( i=0,j=0; ( p1[i] != '{' ) && ( p1[i] != '\0' ); i++) 
				{
					if( p1[i] == ',' ) 
					{
						p_name[j] = '\0';
						for( k=0; k < iCssNumber; k++) 
						{ 
							if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
							{
								strcpy( css_font[k].value,css_font_tmp.value );	
								j = 0;
								break;
							}
						}
						space_flag = 0;
						i_flag = 0;					
						if ( j != 0 ) {
							strcpy( css_font_tmp.name, p_name );
							css_font.push_back(css_font_tmp);
							++iCssNumber;
							memset( &css_font_tmp, 0, sizeof(_css_font_t) );
						}
						j = 0;
						continue;
					}
				
				if( p1[i] == ':' )
				{
					p8 = 0;
					p9 = 0;
					p3 = 0;
					p3 = p1+i;                             
					if( p3 != NULL )
					{
						p8 = strstr( p3, "," );
            p9 = strstr( p3, "{" ); 
					}
					if( p8 != NULL && p9 != NULL )                    
					{
						if( strlen(p8) > strlen(p9) )
							i = strlen( p1 ) - strlen( p8 ) - 1;
					}
					else if( p9 != NULL )
						{
							i = strlen( p1 ) - strlen( p9 ) - 1;
						}
						else
						{
							break;                               
						}
				}
				
				if( !isspace( p1[i] ) )
				{
					if( i_flag && j > 1 )
					{
						p_name[j] = ' ';
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
						p_name[j] = p1[i];
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
						i_flag = 0;
					}
					else
					{
						p_name[j] = p1[i];
						j++;
						if( j >= 100 ) 
						{
							j = 0;
							continue;
						}
					}
				}
				else
				{
					if( j > 1 )
					{
						i_flag = 1;
						space_flag += 1;
					}
					if( space_flag > 1 )
				{
					p8 = 0;
					p9 = 0;
					p3 = 0;
					p3 = p1+i;                             
					if( p3 != NULL )
					{
						p8 = strstr( p3, "," );
            p9 = strstr( p3, "{" ); 
					}
					if( p8 != NULL && p9 != NULL )                    
					{
						if( strlen(p8) > strlen(p9) )
							i = strlen( p1 ) - strlen( p8 ) - 1;
					}
					else if( p9 != NULL )
						{
							i = strlen( p1 ) - strlen( p9 ) - 1;
						}
						else
						{
							break;                               
						}
				}
					continue;
				}
			}
			p_name[j] = '\0';
			for( k=0; k < iCssNumber; k++) 
			{ 
				if( ( strcasecmp( css_font[k].name, p_name ) == 0 ) ) 
				{
					strcpy( css_font[k].value,css_font_tmp.value );	
					j = 0;
					break;
				}
			}
			space_flag = 0;
			i_flag = 0;
			if ( j != 0 ) {
				strcpy( css_font_tmp.name, p_name );
				css_font.push_back(css_font_tmp);
				++iCssNumber;
				memset( &css_font_tmp, 0, sizeof(_css_font_t) );
			}
			j = 0;
		}
	 }
	}
//loop_end
	//if( iCssNumber >= 1 )	{
	qsort( css_font[0].name, iCssNumber, 66, Compstr );
	//}
#ifdef _FONT_PRINT_	
	for ( k=0; k < css_font.size(); k++ ) {	printf("css_font[%d]	%s=%s \n",k,css_font[k].name, css_font[k].value); }	
#endif
}
#endif

/*
 * get tag type according to tag type dict
 */ 
html_tag_type_t html_tree::get_tag_type(html_tree_t *html_tree, char *tag_name)
{
	hash_node_t node;

	creat_sign_md64(tag_name, strlen(tag_name), &node.sign1, &node.sign2);
	if (hash_search(html_tree->tag_type_dict, &node)) {
		return (html_tag_type_t)node.value;
	} else {
		return TAG_UNKNOWN;
	}
}

/* 
 * scan all name value pairs 
 */ 
int html_tree::html_tree_scan_attributes(html_tree_t *html_tree)
{
	html_attribute_t *html_attribute;
	
	/* flag 0: no ">" in quote ; 1: has ">" in quote
	 * input_p: pointer of html_tree->next_input_char where flag is been set
	 * available_p: pointer of html_tree->available_buffer where flag is been set
	 *
	int flag = 0;
	char *input_p = NULL, *available_p = NULL;
	*/
	
	//space
	while (*html_tree->next_input_char != '\0' && isspace((unsigned char)*html_tree->next_input_char)) {
		html_tree->next_input_char++;
	}
	while (*html_tree->next_input_char != '\0' && *html_tree->next_input_char != '>') {
		html_attribute = (html_attribute_t *)block_manager::block_heap_allocate(html_tree->attributes_heap);
		if (html_attribute == NULL) {
//do log
			return 0;
		}
		memset(html_attribute, 0, sizeof(html_attribute_t));
		
		//insert
		html_attribute->next = html_tree->next_input_node->html_tag.attribute;
		html_tree->next_input_node->html_tag.attribute = html_attribute;
		
		//name
		html_attribute->name = html_tree->available_buffer;
		while (*html_tree->next_input_char != '\0' && *html_tree->next_input_char != '>' && 
				*html_tree->next_input_char != '=' && !isspace((unsigned char)*html_tree->next_input_char)) {
			*html_tree->available_buffer++ = TO_LOW[(unsigned char)*html_tree->next_input_char++];
		}
		*html_tree->available_buffer++ = '\0';
			
		//space
		while (*html_tree->next_input_char != '\0' && isspace((unsigned char)*html_tree->next_input_char)) {
			html_tree->next_input_char++;
		}
		//=
		if (*html_tree->next_input_char == '=') {
			html_tree->next_input_char++;
			//space
			while (*html_tree->next_input_char != '\0' && isspace((unsigned char)*html_tree->next_input_char)) {
				html_tree->next_input_char++;
			}
			//value
			html_attribute->value = html_tree->available_buffer;
			if ( *html_tree->next_input_char == '"' || *html_tree->next_input_char == '\'') {
				char end_quote = *html_tree->next_input_char;
				
				html_tree->next_input_char++;
			
				//space
				while (*html_tree->next_input_char != '\0' && isspace((unsigned char)*html_tree->next_input_char)) {
					html_tree->next_input_char++;
				}

				while (*html_tree->next_input_char != '\0' && 
						*html_tree->next_input_char != end_quote){
					*html_tree->available_buffer++ = *html_tree->next_input_char++;
				}
				
				//space
				while (html_tree->available_buffer-1 >= html_attribute->value && 
						isspace((unsigned char)*(html_tree->available_buffer-1)))
					html_tree->available_buffer--;
				
				if (*html_tree->next_input_char == end_quote) {
					html_tree->next_input_char++;
				}
			} else {
				while (*html_tree->next_input_char != '\0' &&
						*html_tree->next_input_char != '>' && !isspace((unsigned char)*html_tree->next_input_char)) {
					*html_tree->available_buffer++ = *html_tree->next_input_char++;
				}
			}
			*html_tree->available_buffer++ = '\0';
		}
			
		//space
		while (*html_tree->next_input_char != '\0' && isspace((unsigned char)*html_tree->next_input_char)) {
			html_tree->next_input_char++;
		}
	}
	

#ifdef _FILTER_CONTENT_
	char name[101] = {0};
	char name_2[101] = {0};
	memset( &name_value, 0, sizeof(css_font_t) );                   
	
	css_font_t *name_value_2 = NULL;	

	_html_attribute_t *aaa = html_tree->next_input_node->html_tag.attribute;
	char *font_tmp = 0;
	int gao_tmp = 0;
	
	if( ( strcmp( html_tree->next_input_node->html_tag.tag_name, "input" ) != 0 )
		||( strcmp( html_tree->next_input_node->html_tag.tag_name, "select" ) != 0 )
		||( strcmp( html_tree->next_input_node->html_tag.tag_name, "button" ) != 0 )
		||( strcmp( html_tree->next_input_node->html_tag.tag_name, "textarea" ) != 0 ) 
		)	
	{	
		while( aaa != NULL ) {
			
  	  if( aaa->value  != NULL ) 
  	  {
				font_tmp = strstr( aaa->value, "font-size:" );
				if( font_tmp == NULL ) 
				{
					font_tmp = strstr( aaa->value, "FONT-SIZE:" );
				}			
				if( font_tmp != NULL ) 
				{
					int i=10, k=0;
					for( i=10,k=0; font_tmp[i] != '\0' && ( isdigit( font_tmp[i] )  || isblank( font_tmp[i] ) ); i++ ) 
					{ 
						if( isblank( font_tmp[i] ) ) continue;
						html_tree->font.font_size[k] = font_tmp[i];
						k++;
					}
					html_tree->font.font_size[k] = '\0';
#ifdef _FONT_PRINT_
 					printf("1974: %s html_tree->font.font_size = %s\n", html_tree->next_input_node->html_tag.tag_name, html_tree->font.font_size );
#endif
 					html_tree->font.font_size_depth = html_tree->stack_depth;
					gao_tmp = atoi( html_tree->font.font_size );    
 					if( gao_tmp > 1 )
 					{
 						st_font_tag.push( html_tree->font );
 					}	
				} 
			}

			if( ( strcasecmp(aaa->name, "id") == 0 ) && ( aaa->value != NULL ) ) 
			{
				snprintf(name,80,"#%s", aaa->value );
			} 
			else if( ( strcasecmp(aaa->name, "class") == 0 ) && ( aaa->value != NULL ) ) 
				{
					snprintf(name,80,".%s", aaa->value );
				} 

		aaa = aaa->next;
		}

//First:	p   Second:	#b p  Third:	#b

	if( name[0] != '#' && name[0] != '.' )
	{
		strcpy(name_value.name, html_tree->next_input_node->html_tag.tag_name );     //mod_name = html_tree->next_input_node->html_tag->tag_name
		name_value_2 = (css_font_t *)bsearch( name_value.name, css_font[0].name, iCssNumber, 66, Compstr );
		if( name_value_2 != NULL )
		{
			strcpy( html_tree->css_depth.font_size, name_value.value );
			html_tree->css_depth.font_size_depth = html_tree->stack_depth;
			st_font_css.push( html_tree->css_depth );
#ifdef _FONT_PRINT_
			printf("html_tree->css_depth.font_size_depth = %d\n", html_tree->css_depth.font_size_depth );
			printf("html_tree->css_depth.font_size_depth = %s\n", name_value.name );
			printf("html_tree->css_depth.font_size = %s\n", html_tree->css_depth.font_size ); 
#endif
		}
	}
	else
	{
		strcpy( name_2, name );
		strcat( name, " " );
		strcat( name, html_tree->next_input_node->html_tag.tag_name );
		strcpy(name_value.name, name );
		name_value_2 = (css_font_t *)bsearch( name_value.name, css_font[0].name, iCssNumber, 66, Compstr );
		if( name_value_2 != NULL )
		{
			strcpy( html_tree->css_depth.font_size, name_value.value );
			html_tree->css_depth.font_size_depth = html_tree->stack_depth;
			st_font_css.push( html_tree->css_depth );
#ifdef _FONT_PRINT_
			printf("html_tree->css_depth.font_size_depth = %d\n", html_tree->css_depth.font_size_depth );
			printf("html_tree->css_depth.font_size_depth = %s\n", name_value.name );
			printf("html_tree->css_depth.font_size = %s\n", html_tree->css_depth.font_size ); 
#endif
		}
		else
		{
			strcpy(name_value.name, name_2 );
		name_value_2 = (css_font_t *)bsearch( name_value.name, css_font[0].name, iCssNumber, 66, Compstr );
		if( name_value_2 != NULL )
		{
				strcpy( html_tree->css_depth.font_size, name_value.value );
				html_tree->css_depth.font_size_depth = html_tree->stack_depth;
				st_font_css.push( html_tree->css_depth );
#ifdef _FONT_PRINT_
				printf("html_tree->css_depth.font_size_depth = %d\n", html_tree->css_depth.font_size_depth );
				printf("html_tree->css_depth.font_size_depth = %s\n", name_value.name );
				printf("html_tree->css_depth.font_size = %s\n", html_tree->css_depth.font_size ); 
#endif
			}
		}
	}
	
} //if input!

#endif

	return 1;
}

/*
 * if node_end is end tag of node
 */ 
int html_tree::is_this_end_tag(html_node_t *node, html_node_t *node_end)
{
	assert(node_end->html_tag.tag_name != NULL && node_end->html_tag.tag_name[0] == '/');
	if (node->html_tag.tag_type == TAG_ROOT || node->html_tag.tag_type == TAG_PURETEXT) {
		return 0;
	}
	return (strcmp(node->html_tag.tag_name, node_end->html_tag.tag_name+1) == 0);
}

/*
 * if node_end is end tag of one ancestor of node
 */ 
int html_tree::is_ancestor_end_tag(html_node_t *node, html_node_t *node_end)
{
	html_node_t *parent;
	assert(node_end->html_tag.tag_name != NULL && node_end->html_tag.tag_name[0] == '/');

	if (node->html_tag.tag_type == TAG_ROOT)
		return 0;
	
	for (parent=node->parent; parent->parent!=NULL; parent=parent->parent) {
		if (strcmp(parent->html_tag.tag_name, node_end->html_tag.tag_name+1) == 0) {
			return 1;
		}
	}
	return 0;
}

/* delete html tree allocation memory, don't check initial state.
 * if create tree fail, must call this function
 */
void html_tree::html_tree_del_inner(html_tree_t *html_tree)
{
	//del child parent dict
	if (html_tree->child_parent_dict != NULL)
		hash_delete(html_tree->child_parent_dict);
	
	//del tag type dict
	if (html_tree->tag_type_dict != NULL)
		hash_delete(html_tree->tag_type_dict);

	//del string buffer
	if (html_tree->string_heap != NULL)
		free(html_tree->string_heap);
	
	//del attributes heap
	if (html_tree->attributes_heap != NULL)
		block_manager::block_heap_delete(html_tree->attributes_heap);
	
	//del nodes heap
	if (html_tree->nodes_heap != NULL)
		block_manager::block_heap_delete(html_tree->nodes_heap);
	
	//zero memory of html_tree
	memset(html_tree, 0, sizeof(html_tree_t));
	
	//free html_tree
	free(html_tree);
}

/*
 * delete html tree allocated by html_tree_create
 * the html_tree must be in initial state
 */ 
void html_tree::html_tree_del()
{
	assert(is_clean_tree());

	html_tree_del_inner (tree);
}



/* internal function for tree visit*/
int html_tree::html_node_visit(html_node_t *html_node, int (* start_visit)(html_tag_t *html_tag, void *result, int min_font), 
						int (* finish_visit)(html_tag_t *html_tag, void *result), void *result)
{
	int ret;
	
	if (start_visit != NULL) {
		ret = (*start_visit)(&html_node->html_tag, result, min_font);
		if (ret == VISIT_ERROR || ret == VISIT_FINISH) {
			return ret;
		} 
		if (ret == VISIT_SKIP_CHILD) {
			return VISIT_NORMAL;
		}
		assert(ret == VISIT_NORMAL);
	}
	
	for (html_node_t *child=html_node->child; child!=NULL; child=child->next) {
		ret = html_node_visit(child, start_visit, finish_visit, result);
		if (ret == VISIT_ERROR || ret == VISIT_FINISH) {
			return ret;
		}
		assert(ret == VISIT_NORMAL);
	}
	if (finish_visit != NULL) {
		ret = (*finish_visit)(&html_node->html_tag, result);
		if (ret == VISIT_ERROR || ret == VISIT_FINISH) {
			return ret;
		}
		assert(ret == VISIT_NORMAL);
	}
	return VISIT_NORMAL;
}

int html_tree::html_tree_visit(int (* start_visit)(html_tag_t *html_tag, void *result, int min_font ),
			       int (* finish_visit)(html_tag_t *html_tag, void *result), void *result)
{
	int ret;
	
	ret = this->html_node_visit(&(tree->root), start_visit, finish_visit, result);
	if (ret == VISIT_ERROR)
		return 0;
	return 1;
}


	/*
	 *  search the given tagname node.
	 */
html_node_t *html_tree::search_tag_node(html_node_t *start_node, const char *tagname)
{
	html_node_t *ret;

	if ( start_node->html_tag.tag_name && 
		 strcmp(start_node->html_tag.tag_name, tagname) == 0 )
		return start_node;

	for (html_node_t *child=start_node->child; child!=NULL; child=child->next) {
		if (  ( ret = search_tag_node( child, tagname) )  )
			return  ret;

	}

	return NULL;	
}


char *html_tree::get_attribute_value( html_node_t *node , char *name)
{       
	html_tag_t *html_tag;

	html_tag =  &( node->html_tag );

        html_attribute_t *attribute;
        for (attribute=html_tag->attribute; attribute!=NULL; attribute=attribute->next) {
                if (strcasecmp(attribute->name, name) == 0) {
                        return attribute->value;
                }
        }       
        return NULL;
}     




	/*
	 * get  base tag, and charset info.
	 */
void  html_tree::process_head_info(  )
{
	html_node_t * head;

	char *charset, *base_url;


	head = search_tag_node( &(tree->root), "head");
	if ( head == NULL )
		return ; /* do nothing */

//	printf("charset old: %s, %s\n", charset_, head->html_tag.tag_name);

	for (html_node_t *child= head->child; child!=NULL; child=child->next) {

		
		if ( child->html_tag.tag_name == NULL )
			continue;
//		printf("child: %s\n", child->html_tag.tag_name );

		if ( strcmp(child->html_tag.tag_name, "base") == 0 &&
			(base_url = get_attribute_value(child, "href") )  ) {
			strncpy( base_url_, base_url, MAX_URL_LEN );

		} else if ( strcasecmp(child->html_tag.tag_name, "meta") == 0 )
				//	&& get_attribute_value(child, "http-equiv") 
				//	&& strcasecmp( get_attribute_value(child, "http-equiv"), "content-type") == 0 ) 
		{
			charset = get_attribute_value(child, "content");

			if ( charset == NULL )
				continue;

//			printf("charset content: %s\n", charset);

			charset = strstr(charset, "charset=");

			if ( charset ) {
				charset += 8;
				strncpy( charset_, charset, 200); 

			}
		}
	}

//	printf("HEAD info: %s, %s\n", charset_, base_url_);
	return ;
}




