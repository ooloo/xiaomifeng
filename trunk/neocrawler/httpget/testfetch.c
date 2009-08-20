/* testfetch.c
 *
 * A small program to test and demonstrate functionality of the HTTP Fetcher
 *	library.  For a more in-depth example take a look at fetch, a program
 *	that fully utilizes HTTP Fetcher to implement a feature-rich HTTP download
 *	tool (available at http://fetch.sourceforge.net).
 *
 * Lyle Hanson (lhanson@users.sourceforge.net,
 * 	http://designwithacause.com/~lhanson)
 * This code is placed in the public domain.
 */

#include <stdio.h>
#include <http_fetcher.h>		/* Must include this to use HTTP Fetcher */

int main(int argc, char *argv[])
{	
	int r,ret;
	short statuscode;
	char *fileBuf;						/* Pointer to downloaded data */

	ret = http_fetch(argv[1], &fileBuf, &statuscode);	/* Downloads page */
	if(ret == -1)						/* All HTTP Fetcher functions return */
		http_perror("http_fetch");		/*	-1 on error. */
	else
	{
		printf("Page successfully downloaded(%d). (%s)\n", ret, argv[1]);
		printf("%s\n", fileBuf);
		printf("-----------------%d-----------------\n", ret);
		free(fileBuf);
	}

	return 0;
}
