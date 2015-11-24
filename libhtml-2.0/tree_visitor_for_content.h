/**@file tree_visit_for_content.h
*@brief  visit a dom tree and extract content
************************************************************************/



#ifndef __TREE_VISITOR_FOR_CONTENT_
#define __TREE_VISITOR_FOR_CONTENT_

//#include "tree_visitor.h"
#include "html_tree.h"


/**class tree_visit for content
 *@brief visit for content
 **/
class tree_visitor_for_content {

  private:
    html_tree * tree;

  public:
    /**constructor 
     *@brief initial the tree pointer
     *@param pTree: already parsed dom tree pointer
     **/
    tree_visitor_for_content(html_tree * pTree) {
        tree = pTree;
  } public:
    static int pre_visit(html_tag_t * html_tag, void *result,
                         int min_font);
    static int post_visit(html_tag_t * html_tag, void *result);

  public:
    /**@brief extract content for the tree
     *@param content: set content into this space. memory must be allocate outside this function
     *@param num: size of content
     **/
    int extract_content(unsigned int *filter_len, char *content, int size);

};

#endif
