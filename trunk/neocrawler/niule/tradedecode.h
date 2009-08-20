#ifndef _URL_DECODE_H
#define _URL_DECODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*判断是否为合法编码URL字符串。合法返回1，否则为0*/
int isValidEncodeUrl(const char *url);

/*直接解码字符串，需要事先做一下验证后调用*/
void decodeUrl(char *url);


#ifdef __cplusplus
}
#endif

#endif // _URL_DECODE_H
