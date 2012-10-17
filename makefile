C=gcc
CFLAGS=-c
LDFLAGS=-lX11 -lXtst -lrt
DEBUG=-g

all: mmmt

mmmt: mmmt.o touch.o  util.o events.o
	$(C) $(LDFLAGS) $(DEBUG) mmmt.o touch.o util.o events.o -o mmmt

mmmt.o: mmmt.c touch.h util.h events.h
	$(C) $(CFLAGS) $(DEBUG) mmmt.c

touch.o: touch.c touch.h util.h
	$(C) $(CFLAGS) $(DEBUG) touch.c

util.o: util.c util.h
	$(C) $(CFLAGS) $(DEBUG) util.c

events.o: events.c events.h touch.h
	$(C) $(CFLAGS) $(DEBUG) events.c

clean:
	rm -rf *.o mmmt
