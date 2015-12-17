INC = -I../../../libhtml-2.0/
LIB = ../../../libhtml-2.0/htmlparser.a

CPPFLAGS := $(CPPFLAGS) -g -fPIC -Wall $(INC)

OBJS = web_parse.o
LIBO = ./libparse.so

$(LIBO): $(OBJS)
	rm -f $@
	g++ -shared -Wl -o $@ $? $(INC) $(LIB)
	rm -f *.o
	cp ./libparse.so ../../../libhtml-2.0/

all :  clean $(LIBO)

clean:
	rm -f *.o *.so
	touch *.cpp
