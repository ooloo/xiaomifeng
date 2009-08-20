/**
*@file html_tree.h
*@brief  To build a html Dom Tree From the input html file
************************************************************************/

#ifndef __HTML_TREE_H__
#define __HTML_TREE_H__

#include "hash_dict.h"
#include "tag_def.h"
#include "block_manager.h"

#include "define.h"
#ifdef _FILTER_CONTENT_
#include <vector>
using namespace std ;
#endif

#define VISIT_ERROR -1
#define VISIT_NORMAL 1
#define VISIT_FINISH 2
#define VISIT_SKIP_CHILD 	 3


#define  MAX_URL_LEN     1024
#define  MAX_TEXT_LEN   200
#define  MAX_TAGTEXT_LEN   (256*1024)

#ifdef _FILTER_CONTENT_

#define I_MAX 999

#define CSS_SIZE 4

typedef struct _css_font_t {
	char name[101];
	char value[CSS_SIZE];
} css_font_t;

typedef struct _font_size_t {
	int font_size_depth;
	char font_size[5];
	//char *font_size;
} font_size_t;

typedef struct _strong_hx_t {
	int i_strong_depth;
	char *p_strong_value;
} strong_hx_t;
#endif

typedef struct _link_t {
        char url[MAX_URL_LEN];
        char text[MAX_TEXT_LEN];
} link_t;



/** 
* basic datastructure used for building the tree
*************************************************************************/

//attribute
typedef struct _html_attribute_t {
	char *name;
	char *value;
	struct _html_attribute_t *next;
} html_attribute_t;

//tag
typedef struct _html_tag_t {
	char *tag_name;
	struct _html_attribute_t *attribute;
	char *text;
	html_tag_type_t tag_type;
#ifdef _FILTER_CONTENT_
	char font_value[5];
#endif
	int tag_code;
} html_tag_t;


//node 
typedef struct _html_node_t {
	html_tag_t html_tag;
    	struct _html_node_t *parent;
	struct _html_node_t *child;
    	struct _html_node_t *next;
} html_node_t;


//tree
typedef struct _html_tree_t {
	block_heap_t *nodes_heap;			//the nodes buffer
	block_heap_t *attributes_heap;		//the attributes buffer
	char *string_heap;					//the text buffer
	int string_heap_size;				//the text buffer size	
	char *available_buffer;				//the available postion of text buffer
	
	char *next_input_char;				//the next input char, for scanner
	html_node_t *next_input_node;		//the next input token, for parser
	
	int next_tag_code;					//the order number of tag

	int stack_depth;					//stack depth when parsing, for parser
	
	hash_dict_t *tag_type_dict;			//dict to translate tag name to type, for scanner
	hash_dict_t *child_parent_dict;		//dict of all child parent relations of html tags, for parser
	
	html_node_t root;					//the tree

#ifdef _FILTER_CONTENT_

	_font_size_t font;
	_font_size_t css_depth;
	_strong_hx_t strong_hx;
	
#endif

} html_tree_t;
	
/**class html_tree
 *@brief html_tree contained the basic data structure of a html tree and its interface 
 **/
class html_tree
{
 private:
    static char tag_char[];
		
		int min_font;
		
 private:
    int max_page_len;

    html_tree_t *tree;

 private:

    int is_ancestor_end_tag(html_node_t *node, html_node_t *node_end);

    int is_this_end_tag(html_node_t *node, html_node_t *node_end);

    int html_tree_scan_attributes(html_tree_t *html_tree);

    html_tag_type_t get_tag_type(html_tree_t *html_tree, char *tag_name);

    int html_tree_scan(html_tree_t *html_tree);

#ifdef _FILTER_CONTENT_    

    int html_tree_fontsize_attributes(html_tree_t *html_tree);
		int html_tree_strong_attributes(html_tree_t *html_tree);
    void html_tree_scan_style_text_analyze_font(html_tree_t *html_tree);
    	
#endif

    void append_child(html_node_t *parent, html_node_t *child);

    int is_child_tag(html_tree_t *html_tree, html_node_t *parent, html_node_t *child);

    int scan_comment_inner (html_tree_t *html_tree);

    void html_tree_scan_comment(html_tree_t *html_tree);

    void html_tree_scan_doctype(html_tree_t *html_tree);

    void html_tree_scan_script_text(html_tree_t *html_tree);

    void html_tree_scan_style_text(html_tree_t *html_tree);

    int get_node_token_type(html_node_t *html_node);

    int is_clean_tree();

    int html_tree_parse_sub_tree(html_tree_t *html_tree, html_node_t *html_node);

    hash_dict_t *create_child_parent_dict(hash_dict_t *tag_type_dict);

    hash_dict_t  *create_tag_type_dict();

    void html_tree_del_inner(html_tree_t *html_tree);

    int html_tree_create();

    html_node_t *search_tag_node(html_node_t *start_node, const char *tagname);

    /* internal function for tree visit*/
    int html_node_visit(html_node_t *html_node, int (* start_visit)(html_tag_t *html_tag, void *result, int min_font ), 
			       int (* finish_visit)(html_tag_t *html_tag, void *result), void *result);


    char *get_attribute_value( html_node_t *node , char *name);
    void  process_head_info();


 public:
    
    char base_url_[MAX_URL_LEN+1];
    char charset_[200 +1];

    /* 
    	0: index page.
	1: content page.
    
    */
    int  page_type;


    /**constructor 
     *@brief allocate memory for parsing
     *@param max_page_len: is the max length of the input html
     **/
    html_tree(int max_page_len);


    ~html_tree();

    /**@brief build Dom tree
     *@param page: input html as char stream
     *@param len: length of page
     *@retval 0: fail
     *@retval 1: success
     **/
    int html_tree_parse(const char*, char *page, int len);
    
    /**
     *@brief clean the inner data structure but not free memory
     **/
    void html_tree_clean();

    void html_tree_del();
    
    /**
     *@breif get Dom Tree
     *@return if tree is not generted return NULL
     **/
    html_tree_t *get_tree(){return tree;}

    /*parse the tree*/


    /* visit the tree*/
    /**
    *@brief tree visit interface
    *@param pre_visit:  function for before visit a html tag
    *@param post_visit: function for after visit a html tag
    *@param result: information is filled into 
    *@retval -1: error
    *@retval 1:  normal
    **/
   int html_tree_visit(int (*pre_visit)(html_tag_t *html_tag, void *result, int min_font ),
			       int (*post_visit)(html_tag_t *html_tag, void *result), void *result);


};

#endif
