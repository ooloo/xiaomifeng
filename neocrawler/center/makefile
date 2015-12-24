TARGET = neocenter neoradar neoseed debug check

INC = -I../clib/
LIB = -L../clib/ -lhoobase -lpthread -lssl -lcrypto -ldb

CC      =       gcc -Wall
CFLAGS := $(CFLAGS) -g -DDEBUG $(INC)

all :   clean $(TARGET)

neocenter	:   centerMain.c centerSelector.c centerCache.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB)

neoradar	:   neoradar.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -DRADAR

neoseed	:   neoseed.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB)

debug	:   debug.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -DRADAR

check	:   debug.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB)

clean:
	rm -f $(TARGET) *.o *.a
	touch *.c
