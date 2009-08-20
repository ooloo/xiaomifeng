#include <iostream>
#include "html_parser.h"
        
        
#include "URL.h"
#include "utils.h"

int main(int argc, char **argv)
{       
            
	html_parser parser;

  int len,ret;
  char page[5120000];
  
  if (argc != 2 ) {
      fprintf(stdout,"error in args\n");
      return 1;
  }
 
  FILE* f = fopen(argv[1],"r");
  if(f == NULL)
  {
          fprintf(stdout, "ERROR IN OPEN FILE: %s\n", argv[1]);
          return 0;
  }
  
  len = fread(page, 1, sizeof(page)-1, f);
 
  page[len] = '\0';


	if ( parser.parse_init() < 0 )
        {
                fprintf(stdout, "parser init failed!\n");
                return 0;
        }

	/*parser.parse(NULL, page, len);*/
	parser.parse("http://www.sina.com.cn", page, len);
	
	fprintf(stdout,"TITLE:   %s\n\n",parser.title_);
	fprintf(stdout,"CONTENT:	%s\n\n",parser.content_);

	//if(parser.links_num_ > 0) fprintf(stdout, "LINKS: \n");
        for(int i=0; i < parser.links_num_; ++i)
        {
            //fprintf(stdout, "%s:  %s\n\n", parser.links_[i].url, parser.links_[i].text);
        }


  fprintf(stdout, "page type:%d \n", parser.page_type);

       // for(int i=0; i < parser.links_num_; ++i)
	parser.parse_reset();

	{
		char result[2048];

		ret =  resovle_relative_URL("http://www.sina.com.cn", "http://www.sina.com.cn",
			result );
            	//fprintf(stdout, "result:%s\n\n", result);


		reencode_escapes_URL("http://www.sina.com.cn", result);
            	//fprintf(stdout, "encode result:%s\n\n", result);


	}


	URL  m_url("http://www.sina.com.cn");
	//printf("%s\n", m_url.getHost());
	{
		char res[1024];

		m_url.getCannonURL(res);

		//printf("%s:%s; \t%s\n", m_url.getHost(),  m_url.getPath(), res);
	}

	return 0;
}

