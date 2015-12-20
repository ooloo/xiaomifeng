
HDR = dict.h hashtable.h hb_tree.h pr_tree.h rb_tree.h sp_tree.h wb_tree.h
SRC = dict.c hashtable.c hb_tree.c pr_tree.c rb_tree.c sp_tree.c wb_tree.c
OBJ = $(SRC:c=o)

A_LIB = libdict.a
S_LIB = libdict.sl

CC = gcc
#CFLAGS = -Wall -W -Wbad-function-cast -Wshadow -fPIC -g3 -O -DNDEBUG
CFLAGS =-fPIC -O -DNDEBUG
AR = ar
ARFLAGS = cru

PREFIX = /opt/libdict
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib
INCDIR = $(PREFIX)/include
MANDIR = $(PREFIX)/man
INSTALL = install
USER = 0
GROUP = 0

all : $(A_LIB) $(S_LIB)

$(A_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(A_LIB) $(OBJ)

$(S_LIB) : $(OBJ)
	$(CC) -shared -o $(S_LIB) $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -c $(<)

.PHONY : clean
clean :
	-rm -f $(OBJ) $(A_LIB) $(S_LIB)

old_install : $(A_LIB) $(S_LIB)
	-mkdir $(INCDIR)
	$(INSTALL) -c -o $(USER) -g $(GROUP) -m 644 $(HDR) $(INCDIR)
	$(INSTALL) -c -s -o $(USER) -g $(GROUP) -m 644 $(A_LIB) $(LIBDIR)
	$(INSTALL) -c -s -o $(USER) -g $(GROUP) -m 644 $(S_LIB) $(LIBDIR)


install : $(A_LIB) $(S_LIB)
	test -d $(INCDIR) || mkdirhier $(INCDIR)
	test -d $(LIBDIR) || mkdirhier $(LIBDIR)
	bsdinst -c  -m 0644 $(HDR) $(INCDIR)
	bsdinst -c  -m 0644 $(A_LIB) $(LIBDIR)
	bsdinst -c  -m 0755 $(S_LIB) $(LIBDIR)


uninstall :
	-rm -rf $(INCDIR)
	-rm -f $(LIBDIR)/$(A_LIB)
	-rm -f $(LIBDIR)/$(S_LIB)

test : test.c $(S_LIB) $(A_LIB)
	$(CC) $(CFLAGS)  test.c -o test -L. -ldict
