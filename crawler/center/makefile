TARGET = neocenter neoradar neoseed debug check bbscenter hoocenter

INC = -I../clib/
LIB = -L../clib/ -lhoobase -lpthread -lssl

CC      =       gcc -Wall
CFLAGS := $(CFLAGS) -g -DDEBUG $(INC)

all :   clean $(TARGET)

neocenter	:   centerMain.c centerSelector.c centerCache.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -ldb

bbscenter	:   centerMain.c centerSelector.c centerCache.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -ldb -DBBS

hoocenter	:   centerMain.c centerSelector.c centerCache.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -ldb -DHOO

neoradar	:   neoradar.c centerbdb.c 
	gcc -D__KERNEL__ -g $? -pthread -o $@ $(INC) $(LIB) -ldb -DRADAR

neoseed	:   neoseed.c 
	gcc -D__KERNEL__ -g $? -pthread -o $@ $(INC) $(LIB) -ldb

debug	:   debug.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -ldb -DRADAR

check	:   debug.c centerbdb.c 
	gcc -g $? -pthread -o $@ $(INC) $(LIB) -ldb

clean:
	rm -f $(TARGET) *.o *.a
	touch *.c
