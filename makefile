objects = src/lexer/lexer.o src/lexer/lexer_automaton.o src/parser/grammar_rules.o src/parser/parser.o src/command_processor/command_processor.o src/built_in_commands/built_in_commands.o src/user_interface/user_interface.o src/aux/shared_alloc.o src/aux/array.o src/aux/dict.o src/aux/hash_function.o src/aux/linked_list.o src/aux/types.o
main_objs = testing_and_debugging/debugging/main_debug.o src/main.o
ctest_obj = ext/ctest_lib/lib/ctest.a


miniSh: build $(objects)  $(main_objs)
	cc -o build/miniSh src/main.o $(objects) 
	cc -o build/miniShDEBUG testing_and_debugging/debugging/main_debug.o $(objects)


build:
	mkdir build

testing_and_debugging/debugging/main_debug.o: src/lexer/lexer.h src/parser/parser.h src/command_processor/command_processor.h
src/main.o:                     src/user_interface/user_interface.h
src/lexer/lexer.o:              src/lexer/lexer.h src/lexer/lexer_automaton.h
src/lexer/lexer_automaton.o:    src/lexer/lexer.h src/lexer/lexer_automaton.h
src/parser/parser.o:            src/parser/parser.h src/parser/grammar_rules.h
src/parser/grammar_rules.o:     src/parser/parser.h src/parser/grammar_rules.h
src/command_processor/command_processor.o: src/command_processor/command_processor.h src/parser/parser.h src/project_configurations.h
src/built_in_commands/built_in_commands.o: src/built_in_commands/built_in_commands.h 
src/user_interface/user_interface.o:       src/user_interface/user_interface.h src/lexer/lexer.h src/parser/parser.h src/command_processor/command_processor.h src/built_in_commands/built_in_commands.h src/project_configurations.h
src/aux/shared_alloc.o:         src/aux/shared_alloc.h 
src/aux/array.o:                src/aux/array.h src/aux/types.h
src/aux/dict.o:                 src/aux/dict.h src/aux/types.h src/aux/linked_list.h src/aux/hash_function.h
src/aux/hash_function.o:        src/aux/hash_function.h src/aux/types.h
src/aux/linked_list.o:          src/aux/linked_list.h src/aux/types.h
src/aux/types.o:                src/aux/types.h src/aux/dict.h src/aux/array.h



.PHONY: clean test debug install
clean: 
	rm -f $(objects)
	rm -rf ./build/

debug: build 
	cc -c -g testing_and_debugging/debugging/main_debug.c -o testing_and_debugging/debugging/main_debug.o
	cc -c -g src/main.c -o src/main.o
	cc -c -g src/lexer/lexer.c -o src/lexer/lexer.o
	cc -c -g src/lexer/lexer_automaton.c -o src/lexer/lexer_automaton.o
	cc -c -g src/parser/parser.c -o src/parser/parser.o
	cc -c -g src/parser/grammar_rules.c -o src/parser/grammar_rules.o
	cc -c -g src/command_processor/command_processor.c -o src/command_processor/command_processor.o
	cc -c -g src/built_in_commands/built_in_commands.c -o src/built_in_commands/built_in_commands.o
	cc -c -g src/user_interface/user_interface.c -o src/user_interface/user_interface.o 
	cc -c -g src/aux/shared_alloc.c -o src/aux/shared_alloc.o
	cc -c -g src/aux/array.c -o src/aux/array.o
	cc -c -g src/aux/dict.c -o src/aux/dict.o
	cc -c -g src/aux/hash_function.c -o src/aux/hash_function.o
	cc -c -g src/aux/linked_list.c -o src/aux/linked_list.o
	cc -c -g src/aux/types.c -o src/aux/types.o
	cc -g -o build/miniSh src/main.o $(objects)
	cc -g -o build/miniShDEBUG testing_and_debugging/debugging/main_debug.o $(objects)

test: build $(objects)
	mkdir -p build/test
	@echo Compile test_lexer.c
	cc -g -o build/test/test_lexer testing_and_debugging/testing/test_lexer.c src/lexer/lexer.o src/lexer/lexer_automaton.o $(ctest_obj)
	cc -g -o build/test/test_parser testing_and_debugging/testing/test_parser.c src/parser/parser.o src/parser/grammar_rules.o src/aux/shared_alloc.o $(ctest_obj)
	

install:
	echo "#ifndef PROJECT_CONFIGURATIONS_H"    > src/project_configurations.h
	echo "#define PROJECT_CONFIGURATIONS_H"   >> src/project_configurations.h
	echo "#define PC_PROJECT_PATH" \"$(PWD)\" >> src/project_configurations.h
	echo "#endif"                             >> src/project_configurations.h
 
