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
### -


