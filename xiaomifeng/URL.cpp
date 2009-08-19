
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include "URL.h"



const size_t maxURLLength=1024;



/*
 * Construct an empty URL,
 */
URL::URL()
        :url(NULL),scheme(NULL),host(NULL),port(NULL),path(NULL),depth(0)
{
}

/*
 * Construct URL class with absolute URL
 * And fill scheme, host, port and path with proper value
 */
URL::URL(const char *str)
        :url(NULL),scheme(NULL),host(NULL),port(NULL),path(NULL)
{
        setURL(str);
}


void URL:: getCannonURL(char *buffer)
{
	if ( host != NULL )
	{
		if ( port_val == 80 )
			snprintf(buffer,1024,"http://%s%s",  host, path);
		else
			snprintf(buffer,1024,"http://%s:%s%s",  host,port, path);

	}
	else
	{

		
		snprintf(buffer,1024,"%s",  path);


	}




}



URL::URL(const char *str, bool hasDepth)
        :url(NULL),scheme(NULL),host(NULL),port(NULL),path(NULL)
{
        if ( hasDepth ){
                const char *u=str;
                char depth[20];
                char *d=depth;
                while ( *u!=' ' )
                        *d++=*u++;
                *d = '\0';
                u++;
                while (*u==' ' )
                        u++;
                int n=atoi(depth);
                setURL(u);
                setDepth(n);
        }
        else{
                setURL(str);
        }
}

/*
 *  URL copy constructor
 */
URL::URL(const URL &u)
{
        int len;

        if ( u.url != NULL ){
                len = strlen(u.url);
                url = new char[len+1];
                memset(url,0,len+1);
                memcpy(url,u.url,len+1);
        }
        else
                url = NULL;

        if ( u.scheme != NULL ){
                len = strlen(u.scheme);
                scheme = new char[len+1];
                memset(scheme,0,len+1);
                memcpy(scheme,u.scheme,len+1);
        }
        else
                scheme = NULL;
        
        if ( u.host != NULL ){
                len = strlen(u.host);
                host = new char[len+1];
                memset(host,0,len+1);
                memcpy(host,u.host,len+1);
        }
        else
                host = NULL;

        if ( u.port!=NULL ){
                len = strlen(u.port);
                port = new char[len+1];
                memset(port,0,len+1);
                memcpy(port,u.port,len+1);
        }
        else
                port = NULL;

        if ( u.path != NULL ){
                len = strlen(u.path);
                path = new char[len+1];
                memset(path,0,len+1);
                memcpy(path,u.path,len);
        }
        else
                path = NULL;
}

/*
 * destructor, clean the memory
 */
URL::~URL()
{
        clean();
}
  
bool URL::isValid()
{
        if(scheme!=NULL && host !=NULL && strlen(host) != 0 && url!=NULL )
                return true;
        else
                return false;		
        
}


bool URL::isAbsoluteURL()
{
        if( host !=NULL && strlen(host) != 0 && url!=NULL )
                return true;
        else
                return false;		
        

}


void URL::clean()
{
        if ( url!=NULL ){
                delete [] url;
                url = NULL;
        }
        if ( scheme!=NULL ){
                delete [] scheme;
                scheme = NULL;
        }
        if ( host!=NULL ){
                delete [] host;
                host = NULL;
        }
        if ( port!=NULL ){
                delete [] port;
                port = NULL;
        }
        if ( path!=NULL ){
                delete [] path;
                path = NULL;
        }
}        

/*
 * set URL, scan the URL and build scheme/host/port/path/hashcode, etc.
 */
bool URL::setURL(const char* str)
{
        clean();
        if ( str == NULL )                
                return false;        
        if ( strlen(str)>maxURLLength )
                return false;
        
        int len;


        len = strlen(str);
        url = new char[len+1];
        memset(url,0,len+1);
        memcpy(url,str,len+1);
        char *p;
        p=strchr(url,'#');
        if(p!=NULL)
                *p='\0';
        simplifyURL ();

        scan();

        return isValid();
}

/*
 * generate a hashCode for the URL
 */
unsigned long URL::getHashCode()
{
        unsigned int h;
        if ( port!= NULL && port[0]!=0 )
                h=atoi(port);
        else
                h=80;
        unsigned int i=0;
        if ( host!= NULL ){
                while (host[i] != 0) {
                        h = 31*h + host[i];
                        i++;
                }
        }
        i=0;
        if ( path!=NULL ){
                while (path[i] != 0) {
                        h = 31*h + path[i];
                        i++;
                }
        }
        return h;
        //return (h % hashSize);
}

/*
 * deal the URL build the base URL 
 * URL      (http://www.163.com:80/2005-03-02/123454.html) 
 * base URL (http://www.163.com:80/2005-03-02/)
 */
void URL::setBaseURL(const char *str)
{
        clean();
        int length;
        if ( str == NULL )                
                return ;        

        int len;

        len = strlen(str);
        url = new char[len+1];
        memset(url,0,len+1);
        memcpy(url,str,len+1);
        simplifyURL ();
        
        

        if(*url)
        {
                length = strlen (url) - 1;
                // search for the last DIR_SEP char 

                while (length >= 0  && url[length] != '/' )
                        length--;
                if (length >= 0)
		{
			if ( length >= 3 &&  url[length-1] == '/' &&
				url[length-2] == ':' )
				;
			else
                // found the last DIR_SEP char, end the string there 
                        url[length + 1] = '\0';		   
		}
                else
                // search for the first PATH_STR char
                {
                        for (length = 0; url[length] != ':' && 
                        url[length] != '\0'; length ++);
                        if (url[length] == ':')
                        // found, so end the string there 
                                url[length + 1] = '\0';
                        else
                        // not found, discard the base 
                                url[0] = '\0';
                }
        } 
        
        scan();
        
        
}

/*
 * build scheme://host:port
 */
void URL::getURLPrefix (char **p_str,int len)
{
        char *str;
        str=*p_str;
        memset(str,0,len);
        if(scheme!=NULL)
                strcat(str,scheme);
        if(host!=NULL)
        {
                strcat(str,"://");
                strcat(str,host);		
        }
        if(port!=NULL)
        {
                strcat(str,":");
                strcat(str,port);		
        }	

	
}

void URL::scan ()
{
        int len;
        char *t_url;
        t_url=NULL;
        len=strlen(url);
        t_url=new char[len+10];
        memset(t_url,0,sizeof(char)*len);
        strcpy(t_url,url);
        char *p_scheme;
        char *p_host;
        char *p_port;
        char *p_path;
        int flag;

        char *   p;
        char *   after_scheme = t_url;
        p_scheme=NULL;
        p_host=NULL;
        p_port=NULL;
        p_path=NULL;
        flag=0;
        
       	int top_level = 0;
	if ( t_url[0] == '/' )
		top_level = 1;

        for (p=t_url; *p; p++)
        {
                if (*p == '/' || *p == '#' || *p == '?')
                        break;
                if (*p == ':')
                {
                        *p = 0;
                        p_scheme = after_scheme; // Scheme has been specified 
                        // The combination of gcc, the "-O" flag and the HP platform is
                        //unhealthy. The following three lines is a quick & dirty fix, but is
                        //not recommended. Rather, turn off "-O". 
                        after_scheme = p+1;
//#ifdef _GNU_LINUX
                        if (!strcasecmp("http", p_scheme))
/***
#else
                        if (!stricmp("http", p_scheme))
#endif
***/
                                break;
                        else
                                p_scheme = NULL;
                                
                }
        }
        p = after_scheme;
        if (*p == '/')
        {
                if (p[1] == '/')
                {
			char* p_ask; 
                        p_host = p+2;		// host has been specified 	
                        *p = 0;			// Terminate access 		
                        // look for end of host name if any 
                        p = strchr (p_host, '/');                    
			p_ask = strchr(p_host, '?') ;
               

			if ( p_ask && (  ( (unsigned long)p_ask ) < (  (unsigned long)p ) || p == NULL)  ) 
			{
				int path_len;

				p = p_ask;
				path_len =  strlen(p);
				for( int i= path_len; i >= 0 ; i-- )
				{
					p[i+1] = p[i] ;

				}
				*p = '/';	
			}

        
                        if (p)
                        {
                                *p = '\0';			// Terminate host 
                                p_path = p+1;		// Root has been found 
                                
                                p = strchr (p_host, ':');
                                if(p)
                                {
                                        *p = '\0';			// Terminate host 
                                        p_port = p+1;		// Root has been found 
                                }
                        } 
                        else
                        {
                                p = strchr (p_host, '#');
                                if(p)
                                {
                                        *p = '\0';
                                        p_path = p+1;
                                        flag=1;
                                }
                        }
                        
                       
                }
                else
                        // Root found but no host 
                        p_path = p+1;
        }
        else
        {
                p_path = (*after_scheme) ? after_scheme : 0; // zero for "" 
        }
        if(p_scheme!=NULL)
        {
                len=strlen(p_scheme)+1;
                scheme=new char[len];
                memcpy(scheme,"http",len);		
        }
        if(p_host!=NULL)
        {
                len=strlen(p_host)+1;
                host=new char[len];
                memcpy(host,p_host,len);	
        }
        if(port!=NULL)
        {
                len=strlen(p_port)+1;
                port=new char[len];
                memcpy(port,p_port,len);
        }
        else
        {		
                port=new char[3];
                memcpy(port,"80",3);
        }

	port_val = atoi(port);

        if(p_path!=NULL)
        {
	    if (  host !=NULL || top_level == 1 )
	    {
                len=strlen(p_path);
                path=new char[len+2];
                path[0]='/';
                memcpy(path+1,p_path,len);
		path[len+1] = '\0';
	    } 
	    else
	    {

                len=strlen(p_path);
                path=new char[len+2];
                memcpy(path,p_path,len);
		path[len] = '\0';


	    }

                
        }
        else
        {
                len=2;
                path=new char[len];
                path[0]='/';
                path[1]='\0';
                strcpy(t_url,url);
                if(url!=NULL){
                        delete [] url; url=NULL; 
                }
                len=strlen(t_url)+2;
                url=new char[len];
                sprintf(url,"%s/",t_url);
        }
        
        
        if(t_url!=NULL){
                delete [] t_url; t_url=NULL;
        }
}
/*----------------------------------------------------------------------
     URLCanon
  	Canonicalizes the URL in the following manner starting from the host
  	pointer:
  
  	1) The host name is converted to lowercase  	
  
  	Return: OK	The position of the current path part of the URL
  			which might be the old one or a new one.
  
  ----------------------------------------------------------------------*/
char * URL::URLCanon ( char *p_host)
{
        char   *p_path;
        char   *strptr;
        char   *p_port;
        char   *p_scheme = p_host-3;
  
     
  
        while (p_scheme > url && *(p_scheme - 1) != '/') // Find access method 
                p_scheme--;
        if ((p_path = strchr (p_host, '/')) == NULL)	        // Find path
                p_path = p_host + strlen (p_host);
    
        if ((p_port = strchr (p_host, ':')) != NULL && p_port > p_path)   // Port number 
                p_port = NULL;

        strptr = p_host;				    // Convert to lower-case 
        while (strptr < p_path)
        {
                *strptr = tolower (*strptr);
                strptr++;
        }
    
        // Does the URL contain a full domain name? This also works for a
        //   numerical host name. The domain name is already made lower-case
        //   and without a trailing dot. 
    
        char  *dot = p_port ? p_port : p_path;
        if (dot > url && *--dot == '.')
        {
                char  *orig = dot;
                char  *dest = dot + 1;
                while ((*orig++ = *dest++));
                if (p_port) p_port--;
                p_path--;
        }    
        return p_path;
}
/*----------------------------------------------------------------------
  simplifyURL: simplify a URI
  A URI is allowed to contain the sequence xxx/../ which may be
  replaced by "" , and the sequence "/./" which may be replaced by DIR_STR.
  Simplification helps us recognize duplicate URIs. 
  
  Thus, 	/etc/junk/../fred 	becomes	/etc/fred
                /etc/junk/./fred	becomes	/etc/junk/fred
  
  but we should NOT change
                http://fred.xxx.edu/../..
  
  	or	../../albert.html
  
  In order to avoid empty URLs the following URLs become:
  
  		/fred/..		becomes /fred/..
  		/fred/././..		becomes /fred/..
  		/fred/.././junk/.././	becomes /fred/..
  
  If more than one set of `://' is found (several proxies in cascade) then
  only the part after the last `://' is simplified.
  
  Returns: A string which might be the old one or a new one.
  ----------------------------------------------------------------------*/
void URL::simplifyURL ()
{
        char   *p_path;
        char   *p_scheme;
        char   *newptr; 
        char   *p;
        char   *orig, *dest, *end;
        bool ddot_simplify; // used to desactivate the double dot simplifcation:
			    // something/../ simplification in relative URLs when they start with a ../ 
        if (!url)
                return;
        // should we simplify double dot? 
        p_path = url;  
        if (*p_path == '.' && *(p_path + 1) == '.')
                ddot_simplify = false;
        else
                ddot_simplify = true;
        // Find any scheme name 
        if ((p_path = strstr (url, "://")) != NULL)
        {
                // Find host name 
                p_scheme = url;
                while (p_scheme < p_path && (*p_scheme = tolower (*p_scheme)))
                        p_scheme++;
                p_path += 3;
                while ((newptr = strstr (p_path, "://")) != NULL)
                        // For proxies 
                        p_path = newptr + 3;
                // We have a host name 
                p_path = URLCanon (p_path);
        }
        else if ((p_path = strstr (url, ":/")) != NULL)
                p_path += 2;
        else
                p_path = url;
        if (*p_path == '/' && *(p_path+1) == '/')
                // Some URLs start //<foo> 
                p_path += 1;
        
        if ((p = p_path))
        {
                if (!((end = strchr (p_path, ';')) || (end = strchr (p_path, '?')) ||
                        (end = strchr (p_path, '#'))))
                        end = p_path + strlen (p_path);
                // Parse string second time to simplify 
                p = p_path;
                while (p < end)
                {
                        // if we're pointing to a char, it's safe to reactivate the 
	                // ../ convertion 
	                if (!ddot_simplify && *p != '.' && *p != '/')
                                ddot_simplify = true;
                        if (*p=='/')
                        {
                                if (p > url && *(p+1) == '.' && (*(p+2) == '/' || !*(p+2)))
                                {
                                        orig = p + 1;
                                        dest = (*(p+2) != '/') ? p+2 : p+3;
                                        while ((*orig++ = *dest++)); // Remove a '/' and a dot
                                        end = orig - 1;
                                }
                                else if (ddot_simplify && *(p+1) == '.' && *(p+2) == '.' 
                                        && (*(p+3) == '/' || !*(p+3)))
                                {
                                        newptr = p;
                                        while (newptr>p_path && *--newptr!='/'); // prev '/' 
                                        if (*newptr == '/')
                                                orig = newptr + 1;
                                        else
                                                orig = newptr;
                                        dest = (*(p+3) != '/') ? p+3 : p+4;
                                        while ((*orig++ = *dest++)); // Remove /xxx/.. 
                                        end = orig-1;
                                        // Start again with prev slash 
                                        p = newptr;
                                }
                                else if (*(p+1) == '/')
                                {
                                        while (*(p+1) == '/')
                                        {
                                                orig = p;
                                                dest = p + 1;
                                                while ((*orig++ = *dest++));  // Remove multiple /'s 
                                                end = orig-1;
                                        }
                                }
                                else
                                        p++;
                        }
                        else
                                p++;
                }
        }
        
        // Check for host/../.. kind of things
        
        if (*p_path == '/' && *(p_path+1) == '.' && *(p_path+2) == '.' 
	        && (!*(p_path+3) || *(p_path+3) == '/'))
	        *(p_path+1) = '\0';
        return;
}


bool URL::isPath()
{
        if ( path!=NULL && path[0]!='\0' && path[strlen(path)-1]=='/' )
                return true;
        else return
                     false;
}

bool URL::isIndexFile()
{
        if ( path==NULL || path[0]=='\0' )
                return false;
        int len = strlen(path);
        char *p = &path[len-1];
        for ( int i=len-1; i>=0; i-- ){
                if ( *p=='/' )
                        break;
                else
                        p--;
        }
        p++;
        if ( !strncmp(p,"index.",6) || !strncmp(p,"default.",8) ) {
                int n=&path[len-1]-p;
                if (n > 15)
                        return false;
                return true;
        }
        else
                return false;
}

/**
 *  generate a 32 bit fingerprint by Rabin alghorithm
 */

/***
uint32_t URL::getFingerPrint(const char* s) {
        int nlen=strlen(s);
        unsigned char result32[4];
        uint32_t res=0;
        if(0==sigfunc32_get((const unsigned char*)s,nlen,result32)) {
                res=*((uint32_t*)result32);
        }
        return res;
}

**/
