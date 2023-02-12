.POSIX:
.SUFFIXES:

include config.mk

bin = myshell
src = src/main.c
obj = $(src:.c=.o)
headers =

cflags = $(INCS) -Wall -pedantic -std=c99
ldflags =
optimize = -O3

ifdef debug
optimize = -Og
defines = -Ddebug
debugflag = -g$(debug)
endif

all: $(bin)

src/%.o:
	$(CC) -o $@ $(debugflag) -c $(src) $(cflags) $(defines) $(optimize)

$(bin): $(obj)
	$(CC) $(debugflag) -o $@ $^ $(ldflags) $(cflags) $(optimize)

clean:
	rm -f $(bin)
	rm -f $(obj)

watch:
	echo $(src) $(headers) | tr ' ' '\n' | entr make clean all CC=gcc

.PHONY: all myshell clean watch
