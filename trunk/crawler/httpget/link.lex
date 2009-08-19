%{
int wordCount = 0;
int typeCount = 0;
int flag = 0;
%}

str1 ("reply"|"post"|"del"|"delete"|"message"|"edit"|"report"|"write"|"send"|"action"|"vote"|"cachetime"|"card")
str2 ("search"|"register"|"login"|"user"|"member"|"author"|"profile"|"feedback"|"comment"|"newsHits"|"my\."|"netbar")
str3 ("space"|"club"|"bbs"|"forum"|"blog"|"comic"|"flash"|"cartoon"|"wiki"|"product"|"gonghui"|"book"[./]|"txt"[./])
str4 ("pic"|"photo"|"tuya"|"tuke"|"pc"[./]|[./]"it"[./]|"article"|"zone")
str5 ("\?")

%%

{str1} { wordCount++; }
{str2} { wordCount++; }
{str3} { typeCount++; }
{str4} { typeCount++; }
{str5} { flag++; }

%%

int link_reg(char *str)
{
	wordCount = 0;
	typeCount = 0;
	flag = 0;

	yy_scan_string(str);
	yylex();
	//printf("No of words: %d\n", wordCount);
	yy_delete_buffer(YY_CURRENT_BUFFER);

	// change often
	//if(typeCount > 0 || wordCount > 0)
	if(typeCount > 0)
		return 0;
	
	if(flag > 0 && wordCount > 0)
		return 0;
	
	return -1;
}
int yywrap()
{
	return 1;
}
