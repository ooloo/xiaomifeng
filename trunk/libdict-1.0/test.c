
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

/* These includes renders this file non-ANSI :/ */
#include <sys/types.h>
#include <sys/time.h>

#include "dict.h"

const char appname[] = "test";

char *xstrdup(const char *str);

void err_quit(const char *, ...);
void err_dump(const char *, ...);
void *xmalloc(size_t size);
void *xcalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xdup(const void *ptr, size_t size);
#define xfree(p)	{ if (p) { free(p); } }
#define xfree0(p)	{ if (p) { free(p); (p) = NULL; } }

unsigned
s_hash(str)
	const char *str;
{
	const unsigned char *p = str;
	unsigned hash = 0;

	/* Simple but decent hash. */
	while (*p)
		hash = hash * 33 + *p++;
	return(hash);
}

#define HSIZE	30011

/*#define NWORDS	235881*/

#define NWORDS	8

int
main(argc, argv)
	int argc;
	char **argv;
{
	unsigned i;
	char buf[512], *ptr, *p;
	char *words[NWORDS];
	FILE *fp;
	int rv;
	dict *dct;
	struct timeval tv, etv, tot = { 0, 0 };

	if (argc != 2)
		err_quit("usage: %s [input]", appname);

	dict_set_malloc(xmalloc);

	dct = pr_dict_new((dict_cmp_func)strcmp, free, NULL);
	if (!dct)
		err_quit("can't create container");

	fp = fopen(*++argv, "r");
	if (fp == NULL)
		err_quit("cant open file `%s': %s", *argv, strerror(errno));

	i = 0;
	for (i = 0; fgets(buf, sizeof(buf), fp); i++) {
		strtok(buf, "\n");
		words[i] = xstrdup(buf);
	}
	fclose(fp);

	gettimeofday(&tv, NULL);
	for (i = 0; i < NWORDS; i++) {
		ptr = words[i];
		if ((rv = dict_insert(dct, ptr, ptr, 0)) != 0)
			err_quit("insert failed with %d for `%s'", rv, ptr);
	}
	gettimeofday(&etv, NULL);
	if (etv.tv_usec < tv.tv_usec)
		etv.tv_sec--, etv.tv_usec += 1000000;
	etv.tv_sec -= tv.tv_sec; tot.tv_sec += etv.tv_sec;
	etv.tv_usec -= tv.tv_usec; tot.tv_usec += etv.tv_usec;
	printf("insert = %f s\n",
		   ((float)etv.tv_sec * 1000000.0 + etv.tv_usec) / 1000000.0);

	if ((i = dict_count(dct)) != NWORDS)
		err_quit("bad count (%u)!", i);

	gettimeofday(&tv, NULL);
	for (i = 0; i < NWORDS; i++) {
		ptr = words[i];
		if ((p = dict_search(dct, ptr)) == NULL)
			err_quit("lookup failed for `%s'", buf);
		if (p != ptr)
			err_quit("bad data for `%s', got `%s' instead", ptr, p);
	}
	gettimeofday(&etv, NULL);
	if (etv.tv_usec < tv.tv_usec)
		etv.tv_sec--, etv.tv_usec += 1000000;
	etv.tv_sec -= tv.tv_sec; tot.tv_sec += etv.tv_sec;
	etv.tv_usec -= tv.tv_usec; tot.tv_usec += etv.tv_usec;
	printf("search = %f s\n",
		   ((float)etv.tv_sec * 1000000.0 + etv.tv_usec) / 1000000.0);

	gettimeofday(&tv, NULL);
	for (i = 0; i < NWORDS; i++) {
		ptr = words[i];
		if ((rv = dict_remove(dct, ptr, 1)) != 0)
			err_quit("removing `%s' failed (%d)!\n", ptr, rv);
	}
	gettimeofday(&etv, NULL);
	if (etv.tv_usec < tv.tv_usec)
		etv.tv_sec--, etv.tv_usec += 1000000;
	etv.tv_sec -= tv.tv_sec; tot.tv_sec += etv.tv_sec;
	etv.tv_usec -= tv.tv_usec; tot.tv_usec += etv.tv_usec;
	printf("remove = %f s\n",
		   ((float)etv.tv_sec * 1000000.0 + etv.tv_usec) / 1000000.0);

	if ((i = dict_count(dct)) != 0)
		err_quit("error - count not zero (%u)!", i);

	dict_destroy(dct, 1);

	printf("total  = %f s\n",
		   ((float)tot.tv_sec * 1000000.0 + tot.tv_usec) / 1000000.0);

	exit(0);
}

char *
xstrdup(str)
	const char *str;
{
	return(xdup(str, strlen(str) + 1));
}

void
err_quit(fmt)
	const char *fmt;
	...
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "%s: ", appname);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);

	exit(1);
}

void
err_dump(fmt)
	const char *fmt;
	...
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "%s: ", appname);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);

	abort();
}

void *
xmalloc(size)
	size_t size;
{
	void *p;

	if ((p = malloc(size)) == NULL)
		err_quit("out of memory");
	return(p);
}

void *
xcalloc(size)
	size_t size;
{
	void *p;

	p = xmalloc(size);
	memset(p, 0, size);
	return(p);
}

void *
xrealloc(ptr, size)
	void *ptr;
	size_t size;
{
	void *p;

	if ((p = realloc(ptr, size)) == NULL && size == 0)
		err_quit("out of memory");
	return(p);
}

void *
xdup(ptr, size)
	const void *ptr;
	size_t size;
{
	void *p;

	p = xmalloc(size);
	memcpy(p, ptr, size);
	return(p);
}

