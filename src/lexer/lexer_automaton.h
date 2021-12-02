#ifndef LEXER_AUTOMATON_H
#define LEXER_AUTOMATON_H
#include <stdbool.h>
#include <stddef.h>
#include "lexer.h"

bool LA_execute_lexer_automaton(char str[], size_t *start_index, struct L_token *next_token, char **error_msg_ref);
#endif
