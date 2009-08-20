
#ifndef LINK_FS_H
#define LINK_FS_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define MIN_FILE_BLK 4194304 

FILE *lfs_open(const char *path, const char *mode, size_t fileLen);

int lfs_write(const void *ptr, size_t size, size_t nmemb, size_t fileLen, FILE *stream);

int lfs_close(FILE *stream);

#endif
//------------------------------END----------------------------
