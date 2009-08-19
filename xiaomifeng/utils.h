
#ifndef  UTILS_H
#define  UTILS_H

/**
#ifdef __cplusplus
extern "C" {
#endif
**/


void myurltrim(char *result);


int resovle_relative_URL( const char *base_url, const char *url, char *result);



void  reencode_escapes_URL (const char *s, char *result);




/***
#ifdef __cplusplus
}
#endif
**/


#endif

