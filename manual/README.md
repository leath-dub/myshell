# NAME
myshell - my simple shell

# SYNOPSIS
myshell [batchfile]

# DESCRIPTION

Myshell is a restricted implementation of a shell. A shell is
a command language interpreter [1] - it reads users text and
parses it into a command that can be executed.

With myshell, each command has one program to execute, this
program can be redirected (see IO REDIRECTION) or run in
the background (see BACKGROUND).

# IO REDIRECTION
IO/(Input Output) redirection is the means to which a process's
input and output can be altered, i.e. the behaviour and source
of it's input and output.

In a UNIX-like operating system files are the backbone interface
for programs between each other, and programs and the kernel.
[source here] It follows unsurprisingly that files have a crucial
part in the interface of passing and receiving data between programs.

When you execute a program a 2 special file descriptors are made
available to your program, stdin and stdout. Stdin by default
takes input from the user

# REFERENCES
[1] (POSIX shell)[https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html]
