# NAME
myshell - my simple shell

# SYNOPSIS
myshell [batchfile]

# DESCRIPTION

Myshell is a restricted implementation of a shell. A shell is
a command language interpreter [1] - it reads users text and
parses it into a command that can be executed. It can be used
to run external programs and availables builtins (see BUILTINS below).

# INVOCATION

# COMMANDS
Commands can be executed as follows:

```sh
$ <command> [argument 1] [argument 2] [argument 3] ...
```

Arguments can be separated by each other with 1 or more tabs or whitespaces.
The arguments will be passed to the command as is unless special tokens are
found in the commandline, see ASYNCRONOUS COMMANDS (BACKGROUND EXECUTION)
and IO REDIRECTION below.

`<command>` can be a external program or a shell builtin,
see BUILTINS below.

# BUILTINS
A builtin is executed like an external program, except the code that is
run is builtin-in to the shell, i.e. part of the shell executable. below are
the builtins available in myshell.

## CD
usage: cd [directory]

if [directory] argument given cd will change the current working directory
to be [directory] and set the `PWD` environment variable to [directory]

if no argument is given the current working directory is outputted to
stdout.

## CLR
usage: clr

clears the screen. This builtin uses ANSI escapes codes [4]
`ESC [2J`(erase display) and `ESC [H`(move cursor to home position)

## DIR
usage: dir [directory]

if [directory] argument given the contents of [directory] are printed
to the screen via `ls -la`, see ls(1).

if no arguments are given the current directory will be used in place of
[directory].

## ENVIRON
usage: environ

outputs environment variable (see ENVIRONMENT VARIABLES below) list.

## ECHO
usage: echo [argument 1] [argument 2] [argument 3] ...

echo takes 0 or more arguments and outputs them each on a new line.

## HELP
usage: help

help uses the more program (see more(1)) to display myshell's manual to
the user (what you are reading right now).

## PAUSE
usage: pause

pauses execution of shell until user presses enter.

## QUIT
usage: quit

exits the shell.

# ENVIRONMENT VARIABLES
When a user logs in to a UNIX-like operating system they are given their
own environment, which is a set of key value pairs to allow the user
and programs they execute to understand the system configuration. [5].

In myshell there are 2 special environment variables, `shell` and `parent`.
The `shell` variable is assigned to the absolute path to the executeable
(myshell). Likewise for any programs that the shell executes will have the
`parent` variable assigned to the absolute path to the executeable
(myshell).

# IO REDIRECTION [2]
IO/(Input Output) redirection is the means to which a process's
input and output can be altered, i.e. the behaviour and source
of it's input and output.

Proccesses are able to read and write to files by using the operating
systems concept of "File descriptors". File descriptors are proccess
dependent unique identifiers, this means multiple proccesses can have
the same (same id) attached file descriptors which reference completely
different files on the file system.

Every proccess has 3 special file descriptors attached at execution, stdin,
stdout and stderr [3]. These reference different files in different proccesses
however all proccesses can access their set via the files /dev/stdin,
/dev/stdout and /dev/stderr or by directly making system calls via
file descriptors 0, 1 and 2 respectfully.

By default a proccesses file descriptor 0 (stdin) refers to the
users input however we can actually change what the file descriptor
references on the file system by using IO redirection. see below:

```sh
$ [program] < stdin.txt
```

Above will alter stdin to reference the file `stdin.txt` and when
`program` reads file descriptor 0 it will be reading the contents of that
file.

Likewise we can also change what stdout references. see below:

```sh
$ [program] > stdout.txt
```

Above will alter stdout to reference the file `stdout.txt`, in this case
the `>` token will open the file for writing, create it if it does not exist
and truncate it if it already exists. There is an alternative method
of changing stdout using the `>>` token, this will open the file for
writing, create it if it does not exist and append to it if it already
exists. see below:

```sh
[program] >> stdout.txt
```

You can also change stdin and stdout at the same time. see below:

```sh
[program] > out.txt < in.txt
[program] < in.txt > out.txt
```

The above examples are both equivalent.

# ASYNCRONOUS COMMANDS (BACKGROUND EXECUTION)
Sometimes you may wan't to run a command without having to wait
for it to finish. In this case you can utilize the `&` token to
tell myshell to run the current command in the background. see below:

```sh
$ sleep 10 &
$
```

Above will run the sleep command (see sleep(1)) which will wait for 10
seconds, however since there is a `&` the prompt will be returned to
the user imediately (if a `&` was omitted the user would have to
wait the 10 seconds)

The `&` token can be placed anywhere you choose. below is the equivalent to
above.

```sh
$ sleep & 10
```

# REFERENCES
[1] (POSIX shell)[https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html]
[2] (I/O Redirection)[https://tldp.org/LDP/abs/html/io-redirection.html]
[3] (Manpage on stdin, stdout and stderr)[https://man7.org/linux/man-pages/man3/stdin.3.html]
[4] (ANSI X3.64 standard)[unofficial (not on ansi.org) https://en.wikipedia.org/wiki/ANSI_escape_code]
[5] (Book, UNIX Shell Programming, revised edition 1990 - Stephen G. Kochan and Patrick H. Wood)

Cathal O'Grady, 21442084 - Operating Systems Project 1
ca216/myshell

I understand that the University regards breaches of academic integrity and
plagiarism as grave and serious. I have read and understood the DCU Academic
Integrity and Plagiarism Policy. I accept the penalties that may be imposed
should I engage in practice or practices that breach this policy. I have
identified and included the source of all facts, ideas, opinions and
viewpoints of others in the assignment references. Direct quotations,
paraphrasing, discussion of ideas from books, journal articles, internet
sources, module text, or any other source whatsoever are acknowledged and the
sources cited are identified in the assignment references. I declare that
this material, which I now submit for assessment, is entirely my own work and
has not been taken from the work of others save and to the extent that such
work has been cited and acknowledged within the text of my work. I have used
the DCU library referencing guidelines (available at
https://www4.dcu.ie/library/classes_and_tutorials/citingreferencing.shtml
and/or the appropriate referencing system recommended in the assignment
guidelines and/or programme documentation. By signing this form or by
submitting material online I confirm that this assignment, or any part of it,
has not been previously submitted by me or any other person for assessment on
this or any other course of study By signing this form or by submitting
material for assessment online I confirm that I have read and understood the
DCU Academic Integrity and Plagiarism Policy (available at
http://www.dcu.ie/registry/examinations/index.shtml).
