# miniShell
This repository contains the implementation of a simple shell based on UNIX.

## Current version = 1.0
# Features
- [x] pipelines;
- [x] output redirection;
- [x] input redirection;
- [x] built-in commands;
- [x] basic command history; 
- [ ] alias;
- [ ] job control;
- [ ] multiple commands per line;
- [ ] custom prompt line;

# Tested in:
OP: Enterprise Linux 8 (compilers: clang 12.0.1, gcc 8.5.0; shell: bash 4.4.20);

# Documentation
## 1. Requirements:
gcc or clang\
make\
ctest (https://github.com/ArturAssisComp/ctest.git) \
bash

## 2. Installing:
### 2.1 - Open bash;
### 2.2 - Clone the project:       git clone https://github.com/ArturAssisComp/miniShell.git
### 2.3 - Install the project:     make install
### 2.4 - Build:                   make
### 2.* - (optional) For debuging: make debug
### 2.* - (optional) For testing:  make test

## 3. Opening the Shell:
### 3.1 - Execute: ./build/miniSh
### 3.* - (optional) Open the debugging version: ./build/miniShDEBUG
### 3.* - (optional) Open build/tests and execute any of them;

## 4. Using the shell:
### - Executing basic commands:
```markdown
  To execute basic commands is necessary to type the command name (full name
  or relative name).
Ex. 1:
$ pwd
/home/user

  To add arguments to the command, type them after the command id.
Ex. 2:
$ ls -a
. .. folder1 file1 file2
Ex. 3:
$ cat -T example_file1
  (...)
```
 ### - Using history feature:
 ```markdown
    To use the history feature, the user must type '!!'.
 Ex.:
 $ ls -a
 . .. folder1 file1 file2
 $ !!
 Last command: ls -a
 . .. folder1 file1 file2
```
 ### - Redirecting output and input:
 ```markdown
    To redirect the output of a command, the user must type '>' 
    followed by the name of the output file after the command id. 
    And to redirect the input, the user must type '<' followed by 
    the name of the input file after the command id.
 Ex.:
 $ echo Hello World > hello_world.txt
 $ cat hello_world.txt
 Hello World
 $ cat < hello_world.txt
 Hello World
```
 ### - Communication via pipe:
 ```markdown
    The pipe command ('|') can be used to redirect the output of one command to the 
    next command. The input redirection can be used only with the first command of the pipe. 
    The output redirection, on the other side, can be used in any command of the pipe.
 Ex. 1:
  $ cat example_file.txt
 a
 c -> I want this line
 d
 e
 b
 f
 g
  $ cat example_file.txt | sort > sorted_example_file.txt | grep c
 c -> I want this line
  $ cat sorted_example_file.txt
 a
 b
 c -> I want this line
 d
 e
 f
 g
 ```
 


