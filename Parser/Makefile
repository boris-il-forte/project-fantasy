# A Makefile for simple lex and yacc examples

BIN=./bin/
BUILD=./build/
SRC=./src/
CFLAGS=-I./include
OBJECTS=$(BUILD)y.tab.o $(BUILD)lex.yy.o $(BUILD)fantasy-fuzzy.o

# Comment out the proper lines below according to the scanner and
# parser generators available in your system

LEX = flex 
YACC = bison

# We assume that your C-compiler is called cc

CC = cc

# parser is the final object that we will generate, it is produced by
# the C compiler from the y.tab.o and from the lex.yy.o


all: parser

parser: dir $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN)parser $(OBJECTS)  -ll -lm 

dir:
	mkdir -p $(BUILD) $(BIN)
	
$(BUILD)%.o : $(SRC)%.c
	@echo building: $<
	@$(CC) $(CFLAGS) -c $< -o $@

# These dependency rules indicate that (1) lex.yy.o depends on
# lex.yy.c and y.tab.h and (2) lex.yy.o and y.tab.o depend on parser.h.
# Make uses the dependencies to figure out what rules must be run when
# a file has changed.

$(BUILD)lex.yy.o: $(BUILD)lex.yy.c $(BUILD)y.tab.h
#$(BUILD)lex.yy.o $(BUILD)y.tab.o: $(BUILD)parser.h

## This rule will use yacc to generate the files y.tab.c and y.tab.h
## from our file parser.y

$(BUILD)y.tab.c $(BUILD)y.tab.h: $(SRC)parser.y
	$(YACC)  -d -o $(BUILD)y.tab.c -v $(SRC)parser.y

## this is the make rule to use lex to generate the file lex.yy.c from
## our file scanner.l

$(BUILD)lex.yy.c: $(SRC)scanner.l
	$(LEX) -o $(BUILD)lex.yy.c $(SRC)scanner.l
	

## Make clean will delete all of the generated files so we can start
## from scratch

clean:
	rm -rf $(BUILD)
	rm -f $(BIN)*