# Cathal O'Grady, 21442084 - Operating Systems Project 1
# ca216/myshell
#
# I understand that the University regards breaches of academic integrity and
# plagiarism as grave and serious. I have read and understood the DCU Academic
# Integrity and Plagiarism Policy. I accept the penalties that may be imposed
# should I engage in practice or practices that breach this policy. I have
# identified and included the source of all facts, ideas, opinions and
# viewpoints of others in the assignment references. Direct quotations,
# paraphrasing, discussion of ideas from books, journal articles, internet
# sources, module text, or any other source whatsoever are acknowledged and the
# sources cited are identified in the assignment references. I declare that
# this material, which I now submit for assessment, is entirely my own work and
# has not been taken from the work of others save and to the extent that such
# work has been cited and acknowledged within the text of my work. I have used
# the DCU library referencing guidelines (available at
# https://www4.dcu.ie/library/classes_and_tutorials/citingreferencing.shtml
# and/or the appropriate referencing system recommended in the assignment
# guidelines and/or programme documentation. By signing this form or by
# submitting material online I confirm that this assignment, or any part of it,
# has not been previously submitted by me or any other person for assessment on
# this or any other course of study By signing this form or by submitting
# material for assessment online I confirm that I have read and understood the
# DCU Academic Integrity and Plagiarism Policy (available at
# http://www.dcu.ie/registry/examinations/index.shtml).

.POSIX:
.SUFFIXES:

include config.mk

bin = myshell
src = src/main.c src/parser.c src/token.c src/lib.c src/command.c src/prompt.c src/builtin.c
obj = main.o parser.o token.o lib.o command.o prompt.o builtin.o
headers = src/parser.h src/token.h src/command.h src/lib.h src/prompt.h src/builtin.h src/manual.h

# using c99 for portability (also just used to it)
# ansi C also gives better warnings for platform dependent
# functions (if you have issues on your platform open an issue)
cflags = $(INCS) -Wall -pedantic -std=c99
ldflags = -lc
optimize = -O3

ifdef debug
optimize = -Og
defines = -Ddebug
debugflag = -g$(debug)
endif

all: bin manual $(bin)

%.o:
	$(CC) $(debugflag) -c $(src) $(ldflags) $(cflags) $(defines) $(optimize)

$(bin): $(obj)
	$(CC) $(debugflag) -o bin/$@ $^ $(ldflags) $(cflags) $(optimize)

clean:
	@echo
	@echo "-- clean build --"
	@echo
	rm -f bin/myshell
	rm -f src/manual.h
	rm -f $(bin)
	rm -f $(obj)

bin:
	mkdir -p bin

# manual copy hook, uses xxd to copy manual into char buf in manual.h
# @ref https://linux.die.net/man/1/xxd
manual:
	xxd -i ./manual/readme.md > ./src/manual.h

watch:
	echo $(src) $(headers) makefile | tr ' ' '\n' | entr make clean all

.PHONY: all myshell clean bin manual watch
