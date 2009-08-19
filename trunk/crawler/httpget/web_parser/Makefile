INC = -I../../../../parser/html/
LIB = ../../../../parser/html/htmlparser.a ../../../../parser/html/libcharsetdetector.a

CPPFLAGS := $(CPPFLAGS) -g -Wall $(INC)

OBJS = web_parse.o
LIBO = ./libparse.so

$(LIBO): $(OBJS)
	rm -f $@
	g++ $(CPPFLAGS) -shared -Wl,-soname,$@ -o $@ $? $(INC) $(LIB)
	rm -f *.o
	cp ./libparse.so ../../../../lib/

all :  clean $(LIBO)

clean:
	rm -f *.o *.so
	touch *.cpp
