objects = src/lexer/lexer.o src/lexer/lexer_automaton.o src/parser/grammar_rules.o src/parser/parser.o src/command_processor/command_processor.o src/built_in_commands/built_in_commands.o src/aux/shared_alloc.o src/aux/array.o src/aux/dict.o src/aux/hash_function.o src/aux/linked_list.o src/aux/types.o
main_objs = src/main.o src/main_clean.o
ctest_obj = ext/ctest_lib/lib/ctest.a


miniSh: build $(objects)  $(main_objs)
	cc -o build/miniShClean src/main_clean.o $(objects)
	cc -o build/miniSh src/main.o $(objects)


build:
	mkdir build

src/main.o:                     src/lexer/lexer.h src/parser/parser.h src/command_processor/command_processor.h
src/main_clean.o:               src/lexer/lexer.h src/parser/parser.h src/command_processor/command_processor.h
src/lexer/lexer.o:              src/lexer/lexer.h src/lexer/lexer_automaton.h
src/lexer/lexer_automaton.o:    src/lexer/lexer.h src/lexer/lexer_automaton.h
src/parser/parser.o:            src/parser/parser.h src/parser/grammar_rules.h
src/parser/grammar_rules.o:     src/parser/parser.h src/parser/grammar_rules.h
src/command_processor/command_processor.o: src/command_processor/command_processor.h src/parser/parser.h
src/built_in_commands/built_in_commands.o: src/built_in_commands/built_in_commands.h 
src/aux/shared_alloc.o:         src/aux/shared_alloc.h 
src/aux/array.o:                src/aux/array.h src/aux/types.h
src/aux/dict.o:                 src/aux/dict.h src/aux/types.h src/aux/linked_list.h src/aux/hash_function.h
src/aux/hash_function.o:        src/aux/hash_function.h src/aux/types.h
src/aux/linked_list.o:          src/aux/linked_list.h src/aux/types.h
src/aux/types.o:                src/aux/types.h src/aux/dict.h src/aux/array.h



.PHONY: clean test debug
clean: 
	rm -f $(objects)
	rm -rf ./build/

debug: build 
	cc -c -g src/main.c -o src/main.o
	cc -c -g src/main_clean.c -o src/main_clean.o
	cc -c -g src/lexer/lexer.c -o src/lexer/lexer.o
	cc -c -g src/lexer/lexer_automaton.c -o src/lexer/lexer_automaton.o
	cc -c -g src/parser/parser.c -o src/parser/parser.o
	cc -c -g src/parser/grammar_rules.c -o src/parser/grammar_rules.o
	cc -c -g src/command_processor/command_processor.c -o src/command_processor/command_processor.o
	cc -c -g src/built_in_commands/built_in_commands.c -o src/built_in_commands/built_in_commands.o
	cc -c -g src/aux/shared_alloc.c -o src/aux/shared_alloc.o
	cc -c -g src/aux/array.c -o src/aux/array.o
	cc -c -g src/aux/dict.c -o src/aux/dict.o
	cc -c -g src/aux/hash_function.c -o src/aux/hash_function.o
	cc -c -g src/aux/linked_list.c -o src/aux/linked_list.o
	cc -c -g src/aux/types.c -o src/aux/types.o
	cc -g -o build/miniShDebug src/main.o $(objects)
	cc -g -o build/miniShDebugClean src/main_clean.o $(objects)

test: build $(objects)
	mkdir -p build/test
	@echo Compile test_lexer.c
	cc -g -o build/test/test_lexer testing_and_debugging/testing/test_lexer.c src/lexer/lexer.o src/lexer/lexer_automaton.o $(ctest_obj)
	cc -g -o build/test/test_parser testing_and_debugging/testing/test_parser.c src/parser/parser.o src/parser/grammar_rules.o src/aux/shared_alloc.o $(ctest_obj)
	

 
