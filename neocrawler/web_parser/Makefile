INC = -I../../libhtml-2.0/
LIB = ../../libhtml-2.0/htmlparser.a

CPPFLAGS := $(CPPFLAGS) -g -fPIC -Wall $(INC)

OBJS = web_parse.o
LIBO = ./libhtmlparse.so

$(LIBO): $(OBJS)
	rm -f $@
	g++ -shared -o $@ $? $(INC) $(LIB)
	rm -f *.o
	sudo cp ./libhtmlparse.so /usr/lib/

all :  clean $(LIBO)

clean:
	rm -f *.o *.so
	touch *.cpp
