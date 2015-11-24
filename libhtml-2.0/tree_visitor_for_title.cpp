/**
*@file tree_visit_for_title.cpp
*@brief  extract title from Dom Tree
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

#include "tree_visitor_for_title.h"

typedef struct _title_data_t {
    char begin;
    char need_space;
    char *title;
    int available;
    int end;
} title_data_t;

/*
 * extract PURETEXT only from first TITLE
 */
int tree_visitor_for_title::pre_visit(html_tag_t * html_tag, void *result,
                                      int min_font)
{
    title_data_t *title_data;

    title_data = (title_data_t *) result;

    if (html_tag->tag_type == TAG_TITLE)
    {
        title_data->begin = 1;
        return VISIT_NORMAL;
    }
    if (title_data->begin == 1 && html_tag->tag_type == TAG_PURETEXT)
    {
        if (title_data->available != title_data->end)
        {
            int rem = title_data->end - title_data->available - 1;
            int len = strlen(html_tag->text);
            strncpy(title_data->title + title_data->available,
                    html_tag->text, rem);
            if (len > rem)
            {
                title_data->available = title_data->end;
                title_data->title[title_data->end] = '\0';
            } else
                title_data->available += strlen(html_tag->text);
        }

    }
    return VISIT_NORMAL;
}



/*
 * finish visit of tree after visiting title
 */
int tree_visitor_for_title::post_visit(html_tag_t * html_tag, void *result)
{
    if (html_tag->tag_type == TAG_TITLE)
    {
        return VISIT_FINISH;
    }
    return VISIT_NORMAL;
}

/*
 * extract title from html_tree
 */
int tree_visitor_for_title::extract_title(char *title, int size)
{
//    html_tree_t *html_tree = tree->get_tree();
    title_data_t title_data;

    //assert(title != NULL);
    //assert(size > 0);

    *title = '\0';
    title_data.begin = 0;
    title_data.need_space = 0;
    title_data.available = 0;
    title_data.title = title;
    title_data.end = size - 1;

    return tree->html_tree_visit(&pre_visit, &post_visit, &title_data);
}
