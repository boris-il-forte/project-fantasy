CC = gcc
LIBSGTK= $(shell pkg-config --cflags --libs gtk+-2.0)
LIBS= $(LIBSGTK)
#CFLAGS= -Wall -Wextra -O2 -march=native -pipe -fomit-frame-pointer
CFLAGS= -Wall -Wextra -ggdb3 -pipe

TARGET=gbin

PROGRAM_OBJS=fantasy-main.o fantasy-lib.o fantasy-loadsave.o

all: $(TARGET)

$(TARGET): $(PROGRAM_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(PROGRAM_OBJS) -o $(TARGET)

.c.o:
	$(CC) $(CFLAGS) $(LIBSGTK) -c $< -o $@

test: cleant testlib startest

testlib: provalib.o fantasy-lib.o
	${CC} ${CFLAGS} ${LIBS} provalib.o fantasy-lib.o -o bin

startest: 
	./bin

provalib.o:
	${CC} ${CFLAGS} ${LIBS} -c provalib.c

cleanobj:
	rm -f $(PROGRAM_OBJS)

clean: cleanobj
	rm -f $(TARGET)

mrproper: clean
	rm -f *~

cleant: cleanobj
	rm -f bin
