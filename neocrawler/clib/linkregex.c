#include <assert.h>
#include "linkregex.h"

#define MAX_REGEX_LEN 4*1024*1024

regex_t index_preg;
regex_t normal_preg;

void load_conf_file(const char *filename)
{
    int ret, isIndex;
    char line[128],url[128];
    FILE *fp;

    fp=fopen(filename, "r");
    assert(fp);

    char *index_pattern;
    index_pattern = malloc(MAX_REGEX_LEN);
    assert(index_pattern);
    bzero(index_pattern, MAX_REGEX_LEN);

    char *normal_pattern;
    normal_pattern = malloc(MAX_REGEX_LEN);
    assert(normal_pattern);
    bzero(normal_pattern, MAX_REGEX_LEN);

    strcpy(index_pattern, "xiaomifeng");
    strcpy(normal_pattern, "xiaomifeng");

    isIndex = 0;
    while(fgets(line,128,fp) != NULL)
    {
        if(line[0] == '#' || line[0] == '\n' || line[0] == '\0')
        {
            continue;
        }

        line[strlen(line)-1] = '\0';
        if(0 == strcasecmp(line, "[index_regex]"))
        {
            isIndex = 1;
            continue;
        }
        else if(0 == strcasecmp(line, "[normal_regex]"))
        {
            isIndex = 0;
            continue;
        }

        snprintf(url, 128, "|^%s", line);

        if(1 == isIndex)
        {
            strcat(index_pattern, url);
        }
        else
        {
            strcat(normal_pattern, url);
        }
    }

    ret = regcomp(&index_preg, index_pattern, REG_EXTENDED|REG_ICASE|REG_NOSUB);
    printf("index_preg load return code = %d.", ret);
    ret = regcomp(&normal_preg, normal_pattern, REG_EXTENDED|REG_ICASE|REG_NOSUB);
    printf("normal_preg load return code = %d.", ret);

    free(index_pattern);
    free(normal_pattern);
}

int link_regex(char *url, int flag)
{
    int x,y;

    if(INDEX_MODE == flag)
    {
        x = regexec(&index_preg, url, 0, NULL, 0);
        return x;
    }

    if(NORMAL_MODE == flag)
    {
        y = regexec(&normal_preg, url, 0, NULL, 0);
        return y;
    }

    return -1;
}

void reload_conf_file()
{
    regfree(&index_preg);
    regfree(&normal_preg);

    return;
}
