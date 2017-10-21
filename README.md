# Minimal Implementation of a CLI shell in C

This is a C implementation of a basic command line interface shell program. A shell is a user interface that is used to accces an operating system's services. When you open a terminal on your UNIX-based machine, or the command prompt on your Windows machine, you are opening a text (CLI) shell. 

The purpose of this project was to understand the basic components of a shell. A fairly basic shell implementation (like this one) is essentially an infinite loop going through the following cycles -

1. Read command
2. Parse the command
3. Execute the command

If the command is a built in command (for example `ls`,`cd`,`history` are inbuilt in UNIX shells), the shell simply executes it. However, the majority of commands invoke functionalities which are not inbuilt, but are in other programs somewhere else on your machine. In this case, the shell forks of a child which executes the command. 

Parsing the command can be complicated if there are many arguments, or if the line consists of several commands joined together by pipes (`|`) or redirections (`<` , `>>`). 

This projects has the following features -

1. It implements the `history` command as inbuilt. The history queue can be resized as well, using the option `--history-size n`. `!!` executes the last command, and `!n` executes the nth command in the queue.
2. It implements redirection in (`<`) and out (`>`) of files. Nested redirections are not allowed.
3. It implements piping between processes.
