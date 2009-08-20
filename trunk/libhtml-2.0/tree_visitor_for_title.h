/**
*@file tree_visit_for_title.h
*@brief  visit a dom tree and extract title
************************************************************************/



#ifndef __TREE_VISITOR_FOR_TITLE_
#define __TREE_VISITOR_FOR_TITLE_

#include "html_tree.h"

/**class tree_visit_for_title
 *@brief visit for title
 **/
class tree_visitor_for_title
{

 private:
    html_tree *tree;

 public:
    /**constructor 
     *@brief initial the tree pointer
     *@param pTree: already parsed dom tree pointer
     **/
    tree_visitor_for_title(html_tree *pTree){ tree = pTree; }

 public:
    static int pre_visit(html_tag_t *html_tag, void *result, int min_font );
    static int post_visit(html_tag_t *html_tag, void *result);

 public:
    /**@brief extract title for the tree
     *@param title: set title into this space. memory must be allocate outside this function
     *@param size: size of title
     **/
    int extract_title(char *title, int size);

};

#endif
