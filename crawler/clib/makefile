
INC = -I./
LIB = -L./ -lhoobase

OBJS = Hoofs.o Hootcp.o rbdict.o hashtable.o
LIBO = ./libhoobase.so
LIBS = ./libhoobase.a

CC      =   gcc
CFLAGS := $(CFLAGS) -g -Wall $(INC)

.c.o: 
	$(CC) -c $(CFLAGS) -o $@ $<

all: $(LIBS) clean 

$(LIBO): $(OBJS)
	rm -f $@
	$(CC) -shared -Wl,-soname,$@ -o $@ $? -lssl

$(LIBS) : $(OBJS)
	rm -f $@
	ar rcv $@ $?
	rm $?
clean:
	rm -rf *.o
