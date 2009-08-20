LIB = ../clib/
CC = gcc -g -Wall

EXES = trade_txt2idx trade_index userinfo_server userinfo_client trade_cgi url_usr_cgi cache_cgi

all	: clean $(EXES)

%.o : %.c
	gcc -g -DSNAP -c $^ -I./ -I../base -I../include/ -I../nlp/ -I../crawler/clib

trade_txt2idx : trade_txt2idx.o tradebdb.o tradedecode.o 
	$(CC) -o $@ $^ -L$(LIB) -lparse -lnewnlp -lbase -lhoobase -lrecode -ldb -lssl -DSNAP

trade_index : trade_index.o tradebdb.o tradedecode.o 
	$(CC) -o $@ $^ -L$(LIB) -lm -lpthread -ldb -lssl -lnewnlp -lbase -lhoobase

userinfo_server: userinfo.o
	$(CC) -o $@ $^ -L$(LIB) -lhoobase -lpthread -lssl

userinfo_client: userinfo_client.o
	$(CC) -o $@ $^ -L$(LIB) -lhoobase

trade_cgi : trade_cgi.o tlds_domain.o tradedecode.o
	$(CC) -o $@ $^ -L$(LIB) -lhoobase

cache_cgi : cache_cgi.o tradebdb.o
	$(CC) -o $@ $^ -lz -ldb -lssl

url_usr_cgi : hoo_cgi.o
	$(CC) -o $@ $^

clean:
	rm -f *.o $(EXES)

.c.o:
	$(CC) -c $< -I../base

