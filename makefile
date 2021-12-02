objects = src/main.o src/lexer/lexer.o src/lexer/lexer_automaton.o


miniSh: build $(objects) 
	cc -o build/miniSh $(objects)


build:
	mkdir build

src/main.o:                     src/lexer/lexer.h
src/lexer/lexer.o:              src/lexer/lexer.h src/lexer/lexer_automaton.h
src/lexer/lexer_automaton.o:    src/lexer/lexer.h src/lexer/lexer_automaton.h



.PHONY: clean
clean: 
	rm build/miniSh $(objects)
	rmdir build

 
