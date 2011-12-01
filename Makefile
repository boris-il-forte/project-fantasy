CC = gcc
SRCPATH= ./src/
LIBSGTK= $(shell pkg-config --cflags --libs gtk+-2.0)
LIBS= $(LIBSGTK)
#CFLAGS= -I./include -Wall -Wextra -O2 -march=native -pipe -fomit-frame-pointer
CFLAGS= -I./include -Wall -Wextra -ggdb3 -pipe

TARGET=gbin

PROGRAM_OBJS=$(SRCPATH)fantasy-main.o $(SRCPATH)fantasy-lib.o $(SRCPATH)fantasy-loadsave.o $(SRCPATH)fantasy-gtk.o
EDITOR_OBJS=$(SRCPATH)fantasy-editor.o $(SRCPATH)fantasy-gtk.o $(SRCPATH)fantasy-lib.o $(SRCPATH)fantasy-loadsave.o

all: $(TARGET) editor

$(TARGET): $(PROGRAM_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(PROGRAM_OBJS) -o $(TARGET)

editor: $(EDITOR_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(EDITOR_OBJS) -o editor

.c.o:
	$(CC) $(CFLAGS) $(LIBSGTK) -c $< -o $@

cleanobj:
	rm -f $(PROGRAM_OBJS)

cleaneditor:
	rm -f $(EDITOR_OBJS) editor

clean: cleanobj cleaneditor
	rm -f $(TARGET)

mrproper: clean
	rm -f *~
