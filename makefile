.POSIX:
.SUFFIXES:

include config.mk

bin = myshell
src = src/main.c src/parser.c src/token.c src/lib.c src/command.c src/prompt.c
obj = main.o parser.o token.o lib.o command.o prompt.o
headers = src/parser.h src/token.h src/command.h src/lib.h src/prompt.h

cflags = $(INCS) -Wall -pedantic -std=c99
ldflags =
optimize = -O3

ifdef debug
optimize = -Og
defines = -Ddebug
debugflag = -g$(debug)
endif

all: $(bin)

%.o:
	$(CC) $(debugflag) -c $(src) $(cflags) $(defines) $(optimize)

$(bin): $(obj)
	$(CC) $(debugflag) -o $@ $^ $(ldflags) $(cflags) $(optimize)

clean:
	@echo
	@echo "-- clean build --"
	@echo
	rm -f $(bin)
	rm -f $(obj)

watch:
	echo $(src) $(headers) makefile | tr ' ' '\n' | entr make clean all CC=gcc

.PHONY: all myshell clean watch
