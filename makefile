objects = src/main.o src/lexer/lexer.o src/lexer/lexer_automaton.o src/parser/grammar_rules.o src/parser/parser.o src/aux/shared_alloc.o
ctest_obj = ext/ctest_lib/lib/ctest.a


miniSh: build $(objects) 
	cc -o build/miniSh $(objects)


build:
	mkdir build

src/main.o:                     src/lexer/lexer.h
src/lexer/lexer.o:              src/lexer/lexer.h src/lexer/lexer_automaton.h
src/lexer/lexer_automaton.o:    src/lexer/lexer.h src/lexer/lexer_automaton.h
src/parser/parser.o:            src/parser/parser.h src/parser/grammar_rules.h
src/parser/grammar_rules.o:     src/parser/parser.h src/parser/grammar_rules.h
src/aux/shared_alloc.o:         src/aux/shared_alloc.h 



.PHONY: clean test debug
clean: 
	rm -f $(objects)
	rm -rf ./build/

debug: build 
	cc -c -g src/main.c -o src/main.o
	cc -c -g src/lexer/lexer.c -o src/lexer/lexer.o
	cc -c -g src/lexer/lexer_automaton.c -o src/lexer/lexer_automaton.o
	cc -c -g src/parser/parser.c -o src/parser/parser.o
	cc -c -g src/parser/grammar_rules.c -o src/parser/grammar_rules.o
	cc -c -g src/aux/shared_alloc.c -o src/aux/shared_alloc.h
	cc -g -o build/miniShDebug $(objects)

test: build $(objects)
	mkdir -p build/test
	@echo Compile test_lexer.c
	cc -g -o build/test/test_lexer testing_and_debugging/testing/test_lexer.c src/lexer/lexer.o src/lexer/lexer_automaton.o $(ctest_obj)
	cc -g -o build/test/test_parser testing_and_debugging/testing/test_parser.c src/parser/parser.o src/parser/grammar_rules.o src/aux/shared_alloc.o $(ctest_obj)
	

 
