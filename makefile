.POSIX:
.SUFFIXES:

include config.mk

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

all: myshell

*.o:
	$(CC) $(debugflag) -c $@ $(cflags) $(defines) $(optimize)

myshell: $(obj)
	$(CC) $(debugflag) -o $@ $^ $(ldflags) $(cflags) $(optimize)

.PHONY: all myshell
