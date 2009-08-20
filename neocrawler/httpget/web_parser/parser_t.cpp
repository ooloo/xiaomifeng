

#include <iostream>
#include "html_parser.h"
            
int main(int argc, char **argv)
{       
            
	html_parser parser;

        int len;
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

	//parser.parse(NULL, page, len);
	parser.parse("http://www.digitimes.com.cn/pages/index.php", page, len);
	
	fprintf(stdout,"TITLE:   %s\n\n",parser.title_);
	fprintf(stdout,"CONTENT:   %s\n\n",parser.content_);

	if(parser.links_num_ > 0)
            fprintf(stdout, "LINKS: \n");
        for(int i=0; i < parser.links_num_; ++i)
        {
            fprintf(stdout, "%s:  %s\n\n", parser.links_[i].url, parser.links_[i].text);
        }

	parser.parse_reset();


	return 0;
}

