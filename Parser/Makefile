# A Makefile for simple lex and yacc examples

BIN=./bin/
BUILD=./build/
SRC=./src/

# Comment out the proper lines below according to the scanner and
# parser generators available in your system

#LEX = lex
#YACC = yacc -d
LEX = flex -o $(BUILD)lex.yy.c
YACC = bison -d -o $(BUILD)y.tab.c

# We assume that your C-compiler is called cc

CC = cc

# parser is the final object that we will generate, it is produced by
# the C compiler from the y.tab.o and from the lex.yy.o


all: parser

parser: dir $(BUILD)y.tab.o $(BUILD)lex.yy.o
	$(CC) -o $(BIN)parser $(BUILD)y.tab.o $(BUILD)lex.yy.o -ll -lm 

dir:
	mkdir -p $(BUILD) $(BIN)

# These dependency rules indicate that (1) lex.yy.o depends on
# lex.yy.c and y.tab.h and (2) lex.yy.o and y.tab.o depend on parser.h.
# Make uses the dependencies to figure out what rules must be run when
# a file has changed.

$(BUILD)lex.yy.o: $(BUILD)lex.yy.c $(BUILD)y.tab.h
#$(BUILD)lex.yy.o $(BUILD)y.tab.o: $(BUILD)parser.h

## This rule will use yacc to generate the files y.tab.c and y.tab.h
## from our file parser.y

$(BUILD)y.tab.c $(BUILD)y.tab.h: $(SRC)parser.y
	$(YACC) -v $(SRC)parser.y

## this is the make rule to use lex to generate the file lex.yy.c from
## our file scanner.l

$(BUILD)lex.yy.c: $(SRC)scanner.l
	$(LEX) $(SRC)scanner.l
	

## Make clean will delete all of the generated files so we can start
## from scratch

clean:
	rm -rf $(BUILD)
	rm -f $(BIN)*
#	-rm -f *.o lex.yy.c *.tab.*  parser *.output 