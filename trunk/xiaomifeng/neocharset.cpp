#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "charset-detector.h"

int check_charset(char *str)
{
	const char *detectedCharset;
	char charset[64]={0};
	void *detector;

	detector = init_charset_detector();
	detectedCharset = detect_buffer_charset(detector, str, strlen(str));
	if(detectedCharset)
	{
		snprintf(charset, 64, "%s", detectedCharset);
		release_charset_detector(detector);
		printf("%s\n", charset);
	}
	else
	{
		release_charset_detector(detector);
		return -1;
		printf("null\n");
	}
}

int main(int argc, char **argv)
{
	FILE *fp;
	char *src;

	if(argc != 2)
	{
		printf("%s srcfile\n", argv[0]);
		exit(1);
	}

	fp = fopen(argv[1], "r");
	assert(fp);

	src = (char*)calloc(1024, 1024);
	fread(src, 1024*1024-2, 1, fp);

	fclose(fp);

	check_charset(src);

	return 0;
}

