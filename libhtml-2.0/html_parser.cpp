#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <iostream>
#include <dlfcn.h>

#include "html_tree.h"
#include "tree_visitor_for_title.h"
#include "tree_visitor_for_content.h"
#include "tree_visitor_for_link.h"

#include "entities.h"
#include "charset-detector.h"

#include "html_parser.h"

int charset_detector(char *src, char *charsetbuff)
{
    char *htmlBuffer;
    const char *detectedCharset;
    void *detector;

    htmlBuffer = src;

    detector = init_charset_detector();
    detectedCharset =
        detect_buffer_charset(detector, htmlBuffer, strlen(htmlBuffer));
    if (detectedCharset)
    {
        //std::cout << detectedCharset << "\n";
        snprintf(charsetbuff, 64, "%s", detectedCharset);
        release_charset_detector(detector);
        return 0;
    } else
    {
        //std::cout << "I don't Know!\n";
        release_charset_detector(detector);
        return -1;
    }
}

int html_parser::parse_init()
{

    outer_ = recode_new_outer(true);

    page_ = (char *) malloc(max_page_len + 1);
    buffer_conv_ = (char *) malloc(max_page_len + 1);

    title_ = (char *) malloc(max_title_len + 1);
    content_ = (char *) malloc(max_content_len + 1);
    links_ = (link_t *) malloc(sizeof(link_t) * (max_links_num + 1));


    tree_ = new html_tree(max_page_len + 1);

    if (page_ == NULL || title_ == NULL || buffer_conv_ == NULL ||
        content_ == NULL || links_ == NULL || tree_ == NULL)
        goto fail;

    page_[0] = '\0';
    content_[0] = '\0';
    title_[0] = '\0';

    buffer_conv_len_ = max_page_len + 1;

    return 0;

  fail:
    parse_destroy();
    return -1;
}


void html_parser::parse_destroy()
{
    if (page_)
    {
        free(page_);
        page_ = NULL;
    }

    if (buffer_conv_)
    {
        free(buffer_conv_);
        buffer_conv_ = NULL;
        buffer_conv_len_ = 0;
    }

    if (title_)
    {
        free(title_);
        page_ = NULL;
    }

    if (content_)
    {
        free(content_);
        content_ = NULL;
    }

    if (links_)
    {
        free(links_);
        links_ = NULL;
    }

    if (tree_)
    {
        delete tree_;
        tree_ = NULL;
    }

    if (outer_)
        recode_delete_outer(outer_);
}

html_parser::~html_parser()
{

    parse_destroy();

}


void html_parser::parse_reset()
{
    tree_->html_tree_clean();
    links_num_ = 0;
    filter_len_ = 0;
}

        /*
         *  convert encode to GBK after parsing.
         */
void html_parser::encode_conv()
{
    char charset_check[200];
    //char charset_conv_str[200];
    //size_t output_allocated = 0;
    //size_t output_length = 0; // buffer_conv_len_;
    //bool success;
    char *outbuff = NULL;

    //printf("charset: %s\n", tree_->charset_);
    if (charset_detector(page_, charset_check) == 0)
    {
        //printf("----- %s ---\n", charset_check);
        //printf("----- %s ---\n", tree_->charset_);
        strcpy(tree_->charset_, charset_check);
    }
    if (tree_->charset_[0] == '\0')
    {
        title_[0] = 0;
        content_[0] = 0;
        return;
    }
    if (strcasestr(tree_->charset_, "utf") == 0 &&
        strcasestr(tree_->charset_, "big") == 0)
    {
        return;                 // no charset_ to be convert.
    }
    if (content_[0] == '\0' && title_[0] == '\0' && links_num_ == 0)
    {
        return;
    }
    // 2007-11-12 by huwei
    iconv_t cd;
    size_t bufsize, inlen, oleft;
    char *pin, *pout;

    cd = iconv_open((const char *) "gbk//ignore",
                    (const char *) tree_->charset_);
    if (cd == (iconv_t) - 1)
    {
        //printf("iconv_open(%s, %s) failed\n", "gbk", tree_->charset_);
    } else
    {
        bufsize = 512 * 1024;
        outbuff = (char *) malloc(bufsize + 1);
        assert(outbuff);

        if (title_[0])
        {
            bzero(outbuff, bufsize);
            pin = title_;
            pout = outbuff;
            inlen = strlen(title_);
            oleft = bufsize;

            //printf("title start\n");
            if (iconv(cd, &pin, &inlen, &pout, &oleft) != (size_t) (-1))
            {
                //printf("title success\n");
                *(outbuff + (bufsize - oleft)) = 0;
                strcpy(title_, outbuff);
            } else
            {
                *(outbuff + (bufsize - oleft)) = 0;
                strcpy(title_, outbuff);
            }
            //printf("iconv title: %s\n", outbuff);
        }

        if (links_num_)
        {
            for (int i = 0; i < links_num_; ++i)
            {
                bzero(outbuff, bufsize);
                pin = links_[i].text;
                pout = outbuff;
                inlen = strlen(links_[i].text);
                oleft = bufsize - 1;

                if (iconv(cd, &pin, &inlen, &pout, &oleft) !=
                    (size_t) (-1))
                {
                    *(outbuff + (bufsize - oleft)) = 0;
                    strcpy(links_[i].text, outbuff);
                } else
                {
                    *(outbuff + (bufsize - oleft)) = 0;
                    strcpy(links_[i].text, outbuff);
                }
            }
        }

        bzero(outbuff, bufsize);
        if (content_[0])
        {
            pin = content_;
            pout = outbuff;
            inlen = strlen(content_);
            oleft = bufsize - 1;

            //printf("contnet start\n");
            size_t ret;
            if ((ret =
                 iconv(cd, &pin, &inlen, &pout, &oleft)) != (size_t) (-1))
            {
                //printf("content success\n");
                *(outbuff + (bufsize - oleft)) = 0;
                strcpy(content_, outbuff);
            } else
            {
                *(outbuff + (bufsize - oleft)) = 0;
                strcpy(content_, outbuff);
            }
        }

        free(outbuff);
        iconv_close(cd);
    }
}



int html_parser::parse(const char *url, const char *page, int len)
{
    char *t;

    if (len > max_page_len)
        return -1;

    t = (char *) page;
    while (*t != '<' && (t - page) < len)
        t++;

    if (*t != '<')
        strncpy(page_, page, len);
    else
    {
        len -= t - page;
        strncpy(page_, t, len);
    }

    page_[len] = '\0';
    content_[0] = '\0';
    title_[0] = '\0';
    links_num_ = 0;
    page_type = -1;

#ifdef _TEST_TIME_
    struct timeval tpstart, tpend;
    float timeuse;
    gettimeofday(&tpstart, NULL);
#endif

    tree_->html_tree_parse(url, page_, len);

#ifdef _TEST_TIME_
    gettimeofday(&tpend, NULL);
    timeuse =
        1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec -
        tpstart.tv_usec;
    printf("%f	html_tree_parse\n ", timeuse);
    time_Total_1 += timeuse;
    gettimeofday(&tpstart, NULL);
    timeuse = 0;
#endif

    tree_visitor_for_title *title_visitor =
        new tree_visitor_for_title(tree_);
    title_visitor->extract_title(title_, max_title_len);

#ifdef _TEST_TIME_
    gettimeofday(&tpend, NULL);
    timeuse =
        1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec -
        tpstart.tv_usec;
    printf("%f	extract_title\n ", timeuse);
    time_Total_2 += timeuse;
    gettimeofday(&tpstart, NULL);
    timeuse = 0;
#endif
    tree_visitor_for_content *content_visitor =
        new tree_visitor_for_content(tree_);
    content_visitor->extract_content(&filter_len_, content_,
                                     max_content_len);

#ifdef _TEST_TIME_
    gettimeofday(&tpend, NULL);
    timeuse =
        1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec -
        tpstart.tv_usec;
    printf("%f	extract_content\n ", timeuse);
    time_Total_3 += timeuse;
    gettimeofday(&tpstart, NULL);
    timeuse = 0;
#endif

    tree_visitor_for_link *link_visitor = new tree_visitor_for_link(tree_);
    int link_ct = link_visitor->extract_link(links_, max_links_num);

#ifdef _TEST_TIME_
    gettimeofday(&tpend, NULL);
    timeuse =
        1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec -
        tpstart.tv_usec;
    printf("%f	extract_link\n ", timeuse);
    time_Total_4 += timeuse;
    timeuse = 0;
#endif

    if (link_ct > 0)
        links_num_ = link_ct;

    page_type = tree_->page_type;

#ifdef _TEST_TIME_
    gettimeofday(&tpstart, NULL);
#endif

    encode_conv();


#ifdef _TEST_TIME_
    gettimeofday(&tpend, NULL);
    timeuse =
        1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec -
        tpstart.tv_usec;
    printf("%f	encode_conv\n ", timeuse);
    time_Total_5 += timeuse;
    gettimeofday(&tpstart, NULL);
    timeuse = 0;
#endif

    UnescapeEntitiesForString(content_, strlen(content_), content_,
                              max_content_len);

#ifdef _TEST_TIME_
    gettimeofday(&tpend, NULL);
    timeuse =
        1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec -
        tpstart.tv_usec;
    printf("%f	UnescapeEntitiesForString\n ", timeuse);
    time_Total_6 += timeuse;
#endif

    return 0;
}



int html_parser::parse_links(const char *url, const char *page, int len)
{
    if (len > max_page_len)
        return -1;
    strncpy(page_, page, len);
    tree_->html_tree_parse(url, page_, len);

    tree_visitor_for_link *link_visitor = new tree_visitor_for_link(tree_);
    int link_ct = link_visitor->extract_link(links_, max_links_num);

    if (link_ct > 0)
        links_num_ = link_ct;

    encode_conv();

    return 0;
}
