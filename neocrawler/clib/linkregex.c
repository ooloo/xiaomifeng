#include <assert.h>
#include "linkregex.h"

#define MAX_REGEX_LEN 4*1024*1024

regex_t *index_preg = NULL;
regex_t *normal_preg = NULL;

void load_conf_file(const char *filename)
{
    int isIndex=0;
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
        }
        else if(0 == strcasecmp(line, "[normal_regex]"))
        {
            isIndex = 0;
        }

        snprintf(url, MAX_REGEX_LEN, "|\\^%s", line);

        if(1 == isIndex)
        {
            strcat(index_pattern, url);
        }
        else
        {
            strcat(normal_pattern, url);
        }

        regcomp(index_preg, index_pattern, REG_EXTENDED|REG_ICASE|REG_NOSUB);
        regcomp(normal_preg, normal_pattern, REG_EXTENDED|REG_ICASE|REG_NOSUB);
    }

    free(index_pattern);
    free(normal_pattern);
}

int link_regex(char *url)
{
    int x,y;

    x = regexec(index_preg, url, 0, NULL, 0);
    y = regexec(normal_preg, url, 0, NULL, 0);

    if(x==0 && y==0)
        return 0;
    else if(x==0)
        return 1;
    else if(y==0)
        return 2;
    else
        return -1;
}

void reload_conf_file()
{
    regfree(index_preg);
    regfree(normal_preg);

    return;
}
