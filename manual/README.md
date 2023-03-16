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
of it's input and output. [2]

In a UNIX-like operating system files are the backbone interface
for programs between each other, and programs and the kernel.
[source here] It follows unsurprisingly that files have a crucial
part in the interface of passing and receiving data between programs.

When you execute a program a 2 special file descriptors are made
available to your program, stdin and stdout. Stdin by default
takes input from the user

# REFERENCES
[1] (POSIX shell)[https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html]
[2] (IO redirection)[http://linuxcommand.org/lc3_lts0070.php]

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
