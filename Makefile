CC=gcc
vpath %.c ./src
LIBSGTK=$(shell pkg-config --cflags --libs gtk+-2.0)
#LIBSGTK=$(shell pkg-config --cflags --libs gtk+-3.0)
LIBS=$(LIBSGTK)
CFLAGS=-I./include
# optimizations
CFLAGS+=-O2 -march=native -pipe -fomit-frame-pointer
# warn & debug
CFLAGS+=-Wall -Wextra -ggdb3 -DDEBUG -D_FORTIFY_SOURCE=2
# GTK3 compatibility
CFLAGS+=-DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE

TARGET=gbin

SRC=fantasy-lib.c fantasy-loadsave.c fantasy-gtk.c fantasIA.c fantasIA-gtk.c
MAIN_SRC =$(SRC) fantasy-main.c 
EDITOR_SRC=$(SRC) fantasy-editor.c

PROGRAM_OBJS = $(patsubst %.c,build/%.o,$(MAIN_SRC)) 
EDITOR_OBJS = $(patsubst %.c,build/%.o,$(EDITOR_SRC)) 

all: build $(TARGET) editor

build:
	mkdir -p $@

build/%.o : %.c
	@echo building: $<
	@$(CC) $(CFLAGS) $(LIBSGTK) -c $< -o $@

$(TARGET): $(PROGRAM_OBJS)
	@echo building game executable
	@$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(PROGRAM_OBJS) -o $(TARGET)

editor: $(EDITOR_OBJS)
	@echo building editor executable
	@$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(EDITOR_OBJS) -o editor

cleanobj:
	rm -f $(PROGRAM_OBJS)
	rm -f $(EDITOR_OBJS)
	rmdir ./build/

clean: cleanobj
	rm -f $(TARGET)

mrproper: clean
	rm -f *~

