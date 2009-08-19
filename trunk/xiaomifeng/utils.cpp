


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


#include "safe-ctype.h"
#include "URL.h"

void mytabletrim(char *url)
{
	char *p, *pp;

	p = pp = url;
	while(*p)
	{
		if(*p != '\t')
		{
			*pp = *p;
			pp++;
		}
		p++;
	}
	*pp = 0;

	return;
}


void myurltrim(char *url)
{
	int tflag=0;
	char *t, *p;
	
	if((t=strstr(url, "../")))
	{
		p = strchr(url + 7, '/');
		if(p == NULL)
			return;
		if(p > t)
			return;

		t += 2;
		*t = '\t';
		while(*t && p != t)
		{
			if(*t == '/')
				tflag++;
			if(tflag == 2)
				break;
			*t = '\t';
			t--;
		}
		mytabletrim(url);
		myurltrim(url);
	}
	else if((t=strstr(url, "./")))
	{
		t += 1;
		*t = '\t';
		while(*t)
		{
			if(*t != '/')
				*t = '\t';
			else
				break;
			t--;
		}
		mytabletrim(url);
		myurltrim(url);
	}
	else
		return;
}

int resovle_relative_URL( const char *base_url, const char *url, char *result)
{

    	URL  base_URL_;

	char base_host[1024];
	char base_URL[1024];
	int base_URL_end =0;


	if ( base_url == NULL || url == NULL )
		return -1;

	base_URL_.setBaseURL(base_url); 
        char *port;

        port = base_URL_.getPort();
	
        if ( port && ( atoi(port) != 80 ) )
                snprintf( base_host, 1024, "http://%s:%s", base_URL_.getHost(), port );
        else
                snprintf( base_host, 1024, "http://%s", base_URL_.getHost() );


        snprintf( base_URL, 1024, "%s%s", base_host, 
		base_URL_.getPath() );
	
        int len = strlen (base_URL );

	if ( base_URL[len-1] == '/')
                base_URL_end = 1;



	URL  url_parsed;
	url_parsed.setURL(url);

	if ( url_parsed.isAbsoluteURL() )
	{

		snprintf(result, 1024, "%s", url);
		
		return 0; /* noting to do.  */

	}

	char * path =  url_parsed.getPath();


	if ( path[0] == '/' ) {

		 sprintf(result, "%s%s", base_host, path);
	} 
	else 
	{

	         if ( base_URL_end == 1)
	                sprintf(result, "%s%s", base_URL, path);
	         else
			 sprintf(result, "%s/%s", base_URL, path);

	}

	myurltrim(result);
	
	return 0;

}


/* Convert an ASCII hex digit to the corresponding number between 0
   and 15.  X should be a hexadecimal digit that satisfies isxdigit;
   otherwise, the result is undefined.  */
#define XDIGIT_TO_NUM(x) ((x) < 'A' ? (x) - '0' : TOUPPER (x) - 'A' + 10)

/* Convert a sequence of ASCII hex digits X and Y to a number betewen
   0 and 255.  Uses XDIGIT_TO_NUM for conversion of individual
   digits.  */
#define X2DIGITS_TO_NUM(h1, h2) ((XDIGIT_TO_NUM (h1) << 4) + XDIGIT_TO_NUM (h2))

/* The reverse of the above: convert a number in the [0, 16) range to
   its ASCII representation in hex.  The A-F characters are in upper
   case.  */
#define XNUM_TO_DIGIT(x) ("0123456789ABCDEF"[x])

/* Like XNUM_TO_DIGIT, but generates lower-case characters. */
#define XNUM_TO_digit(x) ("0123456789abcdef"[x])




/* Support for encoding and decoding of URL strings.  We determine
   whether a character is unsafe through static table lookup.  This
   code assumes ASCII character set and 8-bit chars.  */

enum {
  /* rfc1738 reserved chars, preserved from encoding.  */
  urlchr_reserved = 1,

  /* rfc1738 unsafe chars, plus some more.  */
  urlchr_unsafe   = 2
};

#define urlchr_test(c, mask) (urlchr_table[(unsigned char)(c)] & (mask))
#define URL_RESERVED_CHAR(c) urlchr_test(c, urlchr_reserved)
#define URL_UNSAFE_CHAR(c) urlchr_test(c, urlchr_unsafe)

/* Shorthands for the table: */
#define R  urlchr_reserved
#define U  urlchr_unsafe
#define RU R|U

const static unsigned char urlchr_table[256] =
{
  U,  U,  U,  U,   U,  U,  U,  U,   /* NUL SOH STX ETX  EOT ENQ ACK BEL */
  U,  U,  U,  U,   U,  U,  U,  U,   /* BS  HT  LF  VT   FF  CR  SO  SI  */
  U,  U,  U,  U,   U,  U,  U,  U,   /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  U,  U,  U,  U,   U,  U,  U,  U,   /* CAN EM  SUB ESC  FS  GS  RS  US  */
  U,  0,  U, RU,   0,  U,  R,  0,   /* SP  !   "   #    $   %   &   '   */
  0,  0,  0,  R,   0,  0,  0,  R,   /* (   )   *   +    ,   -   .   /   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* 0   1   2   3    4   5   6   7   */
  0,  0, RU,  R,   U,  R,  U,  R,   /* 8   9   :   ;    <   =   >   ?   */
 RU,  0,  0,  0,   0,  0,  0,  0,   /* @   A   B   C    D   E   F   G   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* H   I   J   K    L   M   N   O   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* P   Q   R   S    T   U   V   W   */
  0,  0,  0, RU,   U, RU,  U,  0,   /* X   Y   Z   [    \   ]   ^   _   */
  U,  0,  0,  0,   0,  0,  0,  0,   /* `   a   b   c    d   e   f   g   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* h   i   j   k    l   m   n   o   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* p   q   r   s    t   u   v   w   */
  0,  0,  0,  U,   U,  U,  U,  U,   /* x   y   z   {    |   }   ~   DEL */

  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,

  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
};
#undef R
#undef U
#undef RU

/* URL-unescape the string S.

   This is done by transforming the sequences "%HH" to the character
   represented by the hexadecimal digits HH.  If % is not followed by
   two hexadecimal digits, it is inserted literally.

   The transformation is done in place.  If you need the original
   string intact, make a copy before calling this function.  */

void url_unescape (char *s)
{
  char *t = s;			/* t - tortoise */
  char *h = s;			/* h - hare     */

  for (; *h; h++, t++)
    {
      if (*h != '%')
	{
	copychar:
	  *t = *h;
	}
      else
	{
	  /* Do nothing if '%' is not followed by two hex digits. */
	  if (!h[1] || !h[2] || !(ISXDIGIT (h[1]) && ISXDIGIT (h[2])))
	    goto copychar;
	  *t = X2DIGITS_TO_NUM (h[1], h[2]);
	  h += 2;
	}
    }
  *t = '\0';
}


enum copy_method { CM_DECODE, CM_ENCODE, CM_PASSTHROUGH };

/* Decide whether to encode, decode, or pass through the char at P.
   This used to be a macro, but it got a little too convoluted.  */
static inline enum copy_method
decide_copy_method (const char *p)
{
  if (*p == '%')
    {
      if (ISXDIGIT (*(p + 1)) && ISXDIGIT (*(p + 2)))
	{
	  /* %xx sequence: decode it, unless it would decode to an
	     unsafe or a reserved char; in that case, leave it as
	     is. */
	  char preempt = X2DIGITS_TO_NUM (*(p + 1), *(p + 2));
	  if (URL_UNSAFE_CHAR (preempt) || URL_RESERVED_CHAR (preempt))
	    return CM_PASSTHROUGH;
	  else
	    return CM_DECODE;
	}
      else
	/* Garbled %.. sequence: encode `%'. */
	return CM_ENCODE;
    }
  else if (URL_UNSAFE_CHAR (*p) && !URL_RESERVED_CHAR (*p))
    return CM_ENCODE;
  else
    return CM_PASSTHROUGH;
}

void  reencode_escapes_URL (const char *s, char *result)
{
  const char *p1;
  char *newstr, *p2;
  int oldlen, newlen;

  int encode_count = 0;
  int decode_count = 0;

   newstr = result;
  /* First, pass through the string to see if there's anything to do,
     and to calculate the new length.  */
  for (p1 = s; *p1; p1++)
    {
      switch (decide_copy_method (p1))
	{
	case CM_ENCODE:
	  ++encode_count;
	  break;
	case CM_DECODE:
	  ++decode_count;
	  break;
	case CM_PASSTHROUGH:
	  break;
	}
    }

  if (!encode_count && !decode_count)
  {

    /* The string is good as it is. */
   // return (char *)s;		/* C const model sucks. */

	strcpy(newstr, s);
  }

  oldlen = p1 - s;
  /* Each encoding adds two characters (hex digits), while each
     decoding removes two characters.  */
  newlen = oldlen + 2 * (encode_count - decode_count);

//  newstr = (char*)malloc (newlen + 1);

  p1 = s;
  p2 = newstr;

  while (*p1)
    {
      switch (decide_copy_method (p1))
	{
	case CM_ENCODE:
	  {
	    unsigned char c = *p1++;
	    *p2++ = '%';
	    *p2++ = XNUM_TO_DIGIT (c >> 4);
	    *p2++ = XNUM_TO_DIGIT (c & 0xf);
	  }
	  break;
	case CM_DECODE:
	  *p2++ = X2DIGITS_TO_NUM (p1[1], p1[2]);
	  p1 += 3;		/* skip %xx */
	  break;
	case CM_PASSTHROUGH:
	  *p2++ = *p1++;
	}
    }
  *p2 = '\0';
  assert (p2 - newstr == newlen);

}




