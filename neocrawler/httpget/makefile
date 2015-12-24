TARGET = testfetch neofetch neoproduce

INC = -I. -I../clib -I../center/
LIB = -L../clib/ -lhoobase -lpthread -lssl -lcrypto

CC      =       gcc -Wall
CFLAGS := $(CFLAGS) -g -DDEBUG $(INC)

all :   clean $(TARGET)

testfetch	:  	testfetch.c http_fetcher.c http_error_codes.c ip_cache.c 1109.c
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -lz

neofetch	:  	neofetch.c http_fetcher.c http_error_codes.c ip_cache.c 1109.c
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -lz

neoproduce	:  	neoproduce.c listpage.c tlds_domain.c
	cd web_parser; make;
	gcc -g $? -DECHO -pthread -o $@ $(INC) $(LIB) -lhtmlparse -lz
clean:
	cd web_parser; make clean;
	rm -f $(TARGET) *.o *.a
	touch *.c
