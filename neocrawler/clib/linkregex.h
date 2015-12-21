
#ifndef LINK_REGEX_H
#define LINK_REGEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>


void load_conf_file(const char *filename);

int link_regex(char *url);

void reload_conf_file();

#endif
