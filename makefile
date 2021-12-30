objects = src/main.o src/lexer/lexer.o src/lexer/lexer_automaton.o
ctest_obj = ext/ctest_lib/lib/ctest.a


miniSh: build $(objects) 
	cc -o build/miniSh $(objects)


build:
	mkdir build

src/main.o:                     src/lexer/lexer.h
src/lexer/lexer.o:              src/lexer/lexer.h src/lexer/lexer_automaton.h
src/lexer/lexer_automaton.o:    src/lexer/lexer.h src/lexer/lexer_automaton.h



.PHONY: clean test
clean: 
	rm -f $(objects)
	rm -rf ./build/

test: build $(objects)
	mkdir -p build/test
	@echo Compile test_lexer.c
	cc -o build/test/test_lexer testing_and_debugging/testing/test_lexer.c src/lexer/lexer.o src/lexer/lexer_automaton.o $(ctest_obj)
	

 
