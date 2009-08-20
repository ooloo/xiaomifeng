TARGET = testfetch neofetch neoproduce bbsproduce hooproduce

INC = -I. -I../clib -I../center/
LIB = -L../clib/ -lhoobase -lpthread -lssl

CC      =       gcc -Wall
CFLAGS := $(CFLAGS) -g -DDEBUG $(INC)

all :   clean $(TARGET)

testfetch	:  	testfetch.c http_fetcher.c http_error_codes.c ip_cache.c 1109.c
	gcc -g $? -pthread -o $@ $(INC) $(LIB) /usr/lib/libz.a 

neofetch	:  	neofetch.c http_fetcher.c http_error_codes.c ip_cache.c 1109.c
	gcc -g $? -pthread -o $@ $(INC) $(LIB) /usr/lib/libz.a

neoproduce	:  	neoproduce.c listpage.c tlds_domain.c lex.yy.c
	cd web_parser; make;
	gcc -g $? -DECHO -pthread -o $@ $(INC) $(LIB) -L../../libhtml-2.0/ -lparse -lrecode -lz

bbsproduce	:  	neoproduce.c listpage.c tlds_domain.c picktime.c lex.yy.c
	gcc -g $? -DBBS -DECHO -pthread -o $@ $(INC) $(LIB) -L../../libhtml-2.0/ -lparse -lrecode -lz

hooproduce	:  	neoproduce.c listpage.c tlds_domain.c picktime.c lex.yy.c
	gcc -g $? -DHOO -DECHO -pthread -o $@ $(INC) $(LIB) -L../../libhtml-2.0/ -lparse -lrecode -lz

clean:
	cd web_parser; make clean;
	rm -f $(TARGET) *.o *.a
	flex link.lex
	touch *.c
