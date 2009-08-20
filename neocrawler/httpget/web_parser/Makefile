INC = -I../../../libhtml-2.0/
LIB = ../../../libhtml-2.0/htmlparser.a ../../../libhtml-2.0/libcharsetdetector.a

CPPFLAGS := $(CPPFLAGS) -g -Wall $(INC)

OBJS = web_parse.o
LIBO = ./libparse.so

$(LIBO): $(OBJS)
	rm -f $@
	g++ $(CPPFLAGS) -shared -Wl,-soname,$@ -o $@ $? $(INC) $(LIB)
	rm -f *.o
	cp ./libparse.so ../../../libhtml-2.0/

all :  clean $(LIBO)

clean:
	rm -f *.o *.so
	touch *.cpp
