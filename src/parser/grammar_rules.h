#ifndef GRAMMAR_RULES_H
#define GRAMMAR_RULES_H

#include <stdbool.h>
#include <stddef.h>
#include "../lexer/lexer.h"
#include "parser.h"

/*
Grammar rules:
<token_array>      ::= <body> <comment_line> <end>
<body>             ::= <content> <modifiers> | (empty)
<content>          ::= <command_pipeline>
<command_pipeline> ::= <command> <next_command>
<next_command>     ::= PIPE <piped_command> <next_command> | (empty)
<command>          ::= <command_id> <resources> 
<command_id>       ::= ID
<resources>        ::= <argument> <resources> | <input_redirect> <resources1> 
                     | <output_redirect> <resources> | (empty)
<resources1>       ::= <argument> <resources1> | <output_redirect> <resources1> | (empty)
<argument>         ::= ID
<input_redirect>   ::= IN_REDIRECT ID
<output_redirect>  ::= OUT_REDIRECT ID
<piped_command>    ::= <command_id> <resources1>
<modifiers>        ::= BACKGROUND_EXEC | (empty)
<comment_line>     ::= COMMENT | (empty)
<end>              ::= EOF_TOKEN
*/

//Function declarations:
bool GR_start (struct L_token_array *array_of_tokens, struct P_command_pipeline_linked_list *command_pipeline_linked_list, char error_msg[P_ERROR_MSG_SZ]);      


#endif
