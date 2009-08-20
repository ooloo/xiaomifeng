
INCLUDE = -I../ \

LIB = -L./ \


CPPFLAGS = -g -Wall $(INCLUDE) $(LIB)

CC = g++

all:clean parser_t 

objects = parser_t.o


$(objects): %.o: %.cpp
	$(CC) -c  $(CPPFLAGS) $< -o $@ 

htmlparser.a:$(objects)
	ar -r htmlparser.a  $(objects) 

parser_t:$(objects)
	$(CC) -O2 -o parser_t $(objects) ../htmlparser.a ../libcharsetdetector.a ../librecode.a -lssl

clean:  
	rm *.o -f	
