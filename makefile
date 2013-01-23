C=g++
CFLAGS=-c -Wall
LDFLAGS=-lX11 -lXtst -lrt -lncurses
DEBUG=-g

mmmt: mmmt.o events.o touch.o
	$(C) $(LDFLAGS) $(DEBUG)  -o mmmt mmmt.o events.o touch.o

mmmt.o: mmmt.cpp touch.h
	$(C) $(CFLAGS) $(DEBUG) mmmt.cpp

events.o: events.h events.cpp
	$(C) $(CFLAGS) $(DEBUG) events.cpp

touch2.o: events.h touch.h touch.cpp
	$(C) $(CFLAGS) $(DEBUG) touch.cpp
