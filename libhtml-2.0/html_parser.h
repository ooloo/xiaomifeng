#ifndef __HTML_PARSER_H__
#define __HTML_PARSER_H__

#include <unistd.h>
#include <sys/types.h>
#include "html_tree.h"
#include <recode.h>

#ifdef _TEST_TIME_
#include <sys/time.h>
#endif

#define   max_page_len  4*1024*1024
#define   max_title_len  2*1024
#define   max_content_len  4*1024*1024
#define   max_links_num  8*1024


class html_parser {

#ifdef _TEST_TIME_
  public:
    double time_Total_1;
    double time_Total_2;
    double time_Total_3;
    double time_Total_4;
    double time_Total_5;
    double time_Total_6;
#endif

  public:

    char *page_;
    char *title_;
    char *content_;
    link_t *links_;
    int links_num_;
    unsigned int filter_len_;

    /*
       0: index page.
       1: content page.
       -1: have not get it.
     */
    int page_type;

     html_parser() {
        page_ = NULL;
        title_ = NULL;
        content_ = NULL;
        links_ = NULL;
        links_num_ = 0;
        tree_ = NULL;
        filter_len_ = 0;
    };

    ~html_parser();

    /* init 
       return -1 if failed.
     */
    int parse_init();


    int parse(const char *url, const char *page, int len);

    int parse_links(const char *url, const char *page, int len);

    /*
       prepare for a new parsing.
     */
    void parse_reset();
  private:

    html_tree * tree_;

    void parse_destroy();

    RECODE_OUTER outer_;
    char *buffer_conv_;
    size_t buffer_conv_len_;

    void encode_conv();
};



#endif                          /* __HTML_PARSER_H__ */
