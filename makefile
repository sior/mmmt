C=g++
CFLAGS=-c -Wall
LDFLAGS=-lX11 -lXtst -lrt -lncurses
DEBUG=-g

mmmt: mmmt.o touch.o events.o
	$(C) $(LDFLAGS) $(DEBUG)  -o mmmt mmmt.o touch.o events.o

mmmt.o: mmmt.cpp touch.h
	$(C) $(CFLAGS) $(DEBUG) mmmt.cpp

touch.o: events.h touch.h touch.cpp
	$(C) $(CFLAGS) $(DEBUG) touch.cpp

events.o: events.h events.cpp
	$(C) $(CFLAGS) $(DEBUG) events.cpp
