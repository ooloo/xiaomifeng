/**@file tree_visit_for_link.h
*@brief  visit a dom tree and extract links
************************************************************************/



#ifndef __TREE_VISITOR_FOR_LINK_
#define __TREE_VISITOR_FOR_LINK_

#include "html_tree.h"
#include "URL.h"

/**class tree_visit_for_links
 *@brief visit for links
 **/
class tree_visitor_for_link
{

 private:
    html_tree *tree;

    
 private:
   
    static char *get_attribute_value(html_tag_t *html_tag, char *name);


    static int resovle_url( const char *url, char * result, void  * data);
 public:
    
    /**constructor 
     *@brief initial the tree pointer
     *@pTree: already parsed dom tree pointer
     **/
    tree_visitor_for_link(html_tree *pTree);

 public:
    static int pre_visit(html_tag_t *html_tag, void *result, int min_font);
    static int post_visit(html_tag_t *html_tag, void *result);

 public:
    /**@brief extract links for the tree
     *@param link: set links into this link_t array. memory must be allocate outside this function
     *@param num: max number of links
     **/ 
    int extract_link(link_t* link, int num);

};

#endif
