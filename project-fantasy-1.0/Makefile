CC = gcc
LIBSGTK= $(shell pkg-config --cflags --libs gtk+-2.0)
#CFLAGS= -Wall -Wextra -O2 -march=native -pipe -fomit-frame-pointer
LIBS= 
CFLAGS= -Wall -Wextra -ggdb3 -pipe

all: fantasy-core

test: cleant testlib startest

testlib: provalib.o fantasy-lib.o
	${CC} ${CFLAGS} ${LIBS}   provalib.o fantasy-lib.o -o bin

startest: 
	./bin

fantasy-core: fantasy-main.o fantasy-lib.o
	${CC} ${CFLAGS} ${LIBSGTK} fantasy-main.o fantasy-lib.o -o gbin

fantasy-main.o: fantasy-main.c
	${CC} ${CFLAGS} ${LIBSGTK} -c fantasy-main.c

fantasy-lib.o: fantasy-lib.c
	${CC} ${CFLAGS} ${LIBS} -c fantasy-lib.c
provalib.o:
	${CC} ${CFLAGS} ${LIBS} -c provalib.c

clean:
	rm -rf *.o gbin

cleant:
	rm -rf *.o bin
