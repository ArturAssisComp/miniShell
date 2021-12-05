#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "lexer_automaton.h"

//DEBUG:
#include <stdio.h>

//Data types:
enum automaton_state
{
	START = 0,
	READING_COMMENT_1,
	READING_COMMENT_2,
	ACCEPT_COMMENT,
	ACCEPT_IN_REDIRECT,
	ACCEPT_OUT_REDIRECT,
	ACCEPT_PIPE,
	ACCEPT_BACKGROUND_EXEC,
	ACCEPT_NEW_LINE,
	ACCEPT_EOF,
	READING_ID,
	ACCEPT_ID,
	READING_EXTENDED_ID_1,
	READING_EXTENDED_ID_2,
	ACCEPT_EXTENDED_ID,
	UNEXPECTED_CHAR
};

enum input_type
{
	ASTERISK = 0,  // '*'
	LESS,          // <
	GREATER,       // >
	NEW_LINE_CHAR,      // '\n'
	WHITE_SPACE,   // ' '
	END_OF_FILE,   // '\0'
	VERTICAL_BAR,  // |
	ID_CHAR,       // [0-9, aA-zZ, -, /, _, .]
	DOUBLE_QUOTES, // "
	AMPERSAND,     // &
	TAB,           // '\t'
	BACKSLASH,     // '\'
	ELSE           // any other symbol.
};

//Macros:
#define NUM_OF_STATES      16
#define NUM_OF_INPUT_TYPES 13

//Local function declarations:
void get_automaton_rules(int automaton_matrix[NUM_OF_STATES][NUM_OF_INPUT_TYPES], bool accept_states[NUM_OF_STATES]);
enum input_type get_input_type(char c);
void generate_error_message(char **error_msg_ref, size_t initial_index, size_t current_index, char str[], enum automaton_state current_state, enum automaton_state next_state);





//Function definitions:

bool LA_execute_lexer_automaton(char str[], size_t *start_index, struct L_token *next_token, char **error_msg_ref)
/**
 * Description: this function executes the lexer automaton described at lexer.txt design
 * document. The input for the automaton is the stream of chars from 'str' starting
 * at index '*start_index'. It returns true if the next token was successfully recognized
 * and false otherwise. The result of the recognized token is stored into the variable
 * '*next_token'.
 *
 * Error handling: if the next token was not recognized, false is returned and an error
 * message is stored into *error_msg_ref. In that situation, start_index and next_token
 * will not be changed.
 *
 */
{
	int automaton_matrix[NUM_OF_STATES][NUM_OF_INPUT_TYPES]; 
	bool accept_states[NUM_OF_STATES];
	bool result = true;
	size_t initial_index;
	size_t final_index;
	size_t current_index;
	enum automaton_state current_state;
	enum automaton_state next_state;
	enum input_type current_input_type;	
	char current_input;
	char *tmp;

	//Check inputs:
	if(str == NULL || start_index == NULL || next_token == NULL) 
	{
		*error_msg_ref = "LA_execute_lexer_automaton: Input error";
		result = false;
		goto return_result;
	}

	//Get automaton rules:
	get_automaton_rules(automaton_matrix, accept_states);

	//Run automaton:
	current_index = *start_index;
	current_state = START;
	while(!accept_states[current_state])
	{
		current_input = str[current_index];
		current_input_type = get_input_type(current_input);
		next_state = automaton_matrix[current_state][current_input_type];

		//Special actions for specific transitions:
		//START -> READING_ID
		if(current_state == START && next_state == READING_ID) initial_index = current_index;
		
		//READING_ID -> ACCEPT_ID
		if(current_state == READING_ID && next_state == ACCEPT_ID) final_index = current_index;

		//START -> READING_EXTENDED_ID_1
		if(current_state == START && next_state == READING_EXTENDED_ID_1) initial_index = current_index + 1;

		//READING_EXTENDED_ID_2 -> ACCEPT_EXTENDED_ID
		if(current_state == READING_EXTENDED_ID_2 && next_state == ACCEPT_EXTENDED_ID) final_index = current_index;

		//Create error message: <any state not accepted> -> UNEXPECTED_CHAR
		if(next_state == UNEXPECTED_CHAR)
		{
			generate_error_message(error_msg_ref, initial_index, current_index, str, current_state, next_state);
			result = false;
			goto return_result;
		}	

		//upgrade the current_index and the current_state:
		if(next_state != ACCEPT_ID && next_state != ACCEPT_COMMENT) ++current_index;
		current_state = next_state;
	}

	

	//Update next_token:
	switch(current_state)
	{
		case ACCEPT_COMMENT:
			next_token->token_type = COMMENT;
			next_token->token_value = NULL;
			break;
		case ACCEPT_IN_REDIRECT:
			next_token->token_type = IN_REDIRECT;
			next_token->token_value = NULL;
			break;
		case ACCEPT_OUT_REDIRECT:
			next_token->token_type = OUT_REDIRECT;
			next_token->token_value = NULL;
			break;
		case ACCEPT_PIPE:
			next_token->token_type = PIPE;
			next_token->token_value = NULL;
			break;
		case ACCEPT_BACKGROUND_EXEC:
			next_token->token_type = BACKGROUND_EXEC;
			next_token->token_value = NULL;
			break;
		case ACCEPT_NEW_LINE:
			next_token->token_type = NEW_LINE;
			next_token->token_value = NULL;
			break;
		case ACCEPT_EOF:
			next_token->token_type = EOF_TOKEN;
			next_token->token_value = NULL;
			break;
		case ACCEPT_ID:
		case ACCEPT_EXTENDED_ID:
			//Try to allocate memory for token value:
			tmp = calloc(final_index - initial_index + 1, sizeof *(next_token->token_value));
			if(tmp == NULL) 
			{
				*error_msg_ref = "LA_execute_lexer_automaton: error while allocating memory for the token value";
				result = false;
				goto return_result;
			}

			next_token->token_type = ID;
			//Get semantic value:
			next_token->token_value = tmp;
			memcpy(next_token->token_value, str + initial_index, final_index - initial_index);
			next_token->token_value[final_index] = '\0';
			break;
	}

	//Update *start_index:
	*start_index = current_index;
return_result:
	return result;
}


//Local function definitions:
void get_automaton_rules(int automaton_matrix[NUM_OF_STATES][NUM_OF_INPUT_TYPES], bool accept_states[NUM_OF_STATES])
/**
 * Description: This function populates the automaton_matrix with all the automaton
 * rules described into the lexer documentation and the accept_states with all the 
 * states that are accepted by the automaton. This function populates each 
 * element of the matrix automaton_matrix in the following way: 
 * 	automaton_matrix[current_state][current_input_type] = next_state.
 * And populates each element of the array accept_states in the following way:
 * 	accept_states[state] = true/false
 *
 * Input: (int [][]) automaton_matrix -> The input is a matrix with dimension
 *        NUM_OF_STATES X NUM_OF_INPUT_TYPES. Otherwise, the behavior is 
 *        undefined.
 *        (bool []) accept_states -> This input is an array with size NUM_OF_STATES.
 *        Otherwise, the behavior is undefined.
 */
{
	//Rules:
	/*

	-----------------------------------------------------------------------------------------
	|  <current_input_type>  |     ASTERISK      |        LESS        |       GREATER       | 
	-----------------------------------------------------------------------------------------
	|     <current_state>    |-------------------|--------------------|---------------------|
	| START                  | READING_COMMENT_1 | ACCEPT_IN_REDIRECT | ACCEPT_OUT_REDIRECT |
	| READING_COMMENT_1      | READING_COMMENT_2 |   UNEXPECTED_CHAR  |   UNEXPECTED_CHAR   |
	| READING_COMMENT_2      |         =         |          =         |          =          |
	| ACCEPT_COMMENT         |         -         |          -         |          -          |
	| ACCEPT_IN_REDIRECT     |         -         |          -         |          -          |
	| ACCEPT_OUT_REDIRECT    |         -         |          -         |          -          |
	| ACCEPT_PIPE            |         -         |          -         |          -          |
	| ACCEPT_BACKGROUND_EXEC |         -         |          -         |          -          |
	| ACCEPT_NEW_LINE        |         -         |          -         |          -          |
	| ACCEPT_EOF             |         -         |          -         |          -          |
	| READING_ID             |  UNEXPECTED_CHAR  |      ACCEPT_ID     |      ACCEPT_ID      |
	| ACCEPT_ID              |         -         |          -         |          -          |
	| READING_EXTENDED_ID_1  |  UNEXPECTED_CHAR  |   UNEXPECTED_CHAR  |   UNEXPECTED_CHAR   |
	| READING_EXTENDED_ID_2  |  UNEXPECTED_CHAR  |   UNEXPECTED_CHAR  |   UNEXPECTED_CHAR   |
	| ACCEPT_EXTENDED_ID     |         -         |          -         |          -          |
	| UNEXPECTED_CHAR        |         -         |          -         |          -          |
	-----------------------------------------------------------------------------------------

	--------------------------------------------------------------------------------
	|  <current_input_type>  |  NEW_LINE_CHAR  |   WHITE_SPACE   |   END_OF_FILE   |
	--------------------------------------------------------------------------------
	|     <current_state>    |-----------------|-----------------|-----------------|
	| START                  |        =        |        =        |    ACCEPT_EOF   |
	| READING_COMMENT_1      | UNEXPECTED_CHAR | UNEXPECTED_CHAR | UNEXPECTED_CHAR |
	| READING_COMMENT_2      |  ACCEPT_COMMENT |        =        |  ACCEPT_COMMENT |
	| ACCEPT_COMMENT         |        -        |        -        |        -        |
	| ACCEPT_IN_REDIRECT     |        -        |        -        |        -        |
	| ACCEPT_OUT_REDIRECT    |        -        |        -        |        -        |
	| ACCEPT_PIPE            |        -        |        -        |        -        |
	| ACCEPT_BACKGROUND_EXEC |        -        |        -        |        -        |
	| ACCEPT_NEW_LINE        |        -        |        -        |        -        |
	| ACCEPT_EOF             |        -        |        -        |        -        |
	| READING_ID             |    ACCEPT_ID    |    ACCEPT_ID    |    ACCEPT_ID    |
	| ACCEPT_ID              |        -        |        -        |        -        |
	| READING_EXTENDED_ID_1  | UNEXPECTED_CHAR |        =        | UNEXPECTED_CHAR |
	| READING_EXTENDED_ID_2  | UNEXPECTED_CHAR |        =        | UNEXPECTED_CHAR |
	| ACCEPT_EXTENDED_ID     |        -        |        -        |        -        |
	| UNEXPECTED_CHAR        |        -        |        -        |        -        |
	--------------------------------------------------------------------------------

	--------------------------------------------------------------------
	|  <current_input_type>  |   VERTICAL_BAR  |        ID_CHAR        |
	--------------------------------------------------------------------
	|     <current_state>    |-----------------|-----------------------|
	| START                  |   ACCEPT_PIPE   |       READING_ID      |
	| READING_COMMENT_1      | UNEXPECTED_CHAR |    UNEXPECTED_CHAR    |
	| READING_COMMENT_2      |        =        |           =           |
	| ACCEPT_COMMENT         |        -        |           -           |
	| ACCEPT_IN_REDIRECT     |        -        |           -           |
	| ACCEPT_OUT_REDIRECT    |        -        |           -           |
	| ACCEPT_PIPE            |        -        |           -           |
	| ACCEPT_BACKGROUND_EXEC |        -        |           -           |
	| ACCEPT_NEW_LINE        |        -        |           -           |
	| ACCEPT_EOF             |        -        |           -           |
	| READING_ID             |    ACCEPT_ID    |           =           |
	| ACCEPT_ID              |        -        |           -           |
	| READING_EXTENDED_ID_1  | UNEXPECTED_CHAR | READING_EXTENDED_ID_2 |
	| READING_EXTENDED_ID_2  | UNEXPECTED_CHAR |           =           |
	| ACCEPT_EXTENDED_ID     |        -        |           -           |
	| UNEXPECTED_CHAR        |        -        |           -           |
	--------------------------------------------------------------------


	---------------------------------------------------------------------------
	|  <current_input_type>  |     DOUBLE_QUOTES     |        AMPERSAND       |
	---------------------------------------------------------------------------
	|     <current_state>    |-----------------------|------------------------|
	| START                  | READING_EXTENDED_ID_1 | ACCEPT_BACKGROUND_EXEC |
	| READING_COMMENT_1      |    UNEXPECTED_CHAR    |     UNEXPECTED_CHAR    |
	| READING_COMMENT_2      |           =           |            =           |
	| ACCEPT_COMMENT         |           -           |            -           |
	| ACCEPT_IN_REDIRECT     |           -           |            -           |
	| ACCEPT_OUT_REDIRECT    |           -           |            -           |
	| ACCEPT_PIPE            |           -           |            -           |
	| ACCEPT_BACKGROUND_EXEC |           -           |            -           |
	| ACCEPT_NEW_LINE        |           -           |            -           |
	| ACCEPT_EOF             |           -           |            -           |
	| READING_ID             |    UNEXPECTED_CHAR    |        ACCEPT_ID       |
	| ACCEPT_ID              |           -           |            -           |
	| READING_EXTENDED_ID_1  |    UNEXPECTED_CHAR    |     UNEXPECTED_CHAR    |
	| READING_EXTENDED_ID_2  |   ACCEPT_EXTENDED_ID  |     UNEXPECTED_CHAR    |
	| ACCEPT_EXTENDED_ID     |           -           |            -           |
	| UNEXPECTED_CHAR        |           -           |            -           |
	---------------------------------------------------------------------------


	--------------------------------------------------------------------------------
	|  <current_input_type>  |       TAB       |    BACKSLASH    |       ELSE      |
	--------------------------------------------------------------------------------
	|     <current_state>    |-----------------|-----------------|-----------------|
	| START                  |        =        | ACCEPT_NEW_LINE | UNEXPECTED_CHAR |
	| READING_COMMENT_1      | UNEXPECTED_CHAR | UNEXPECTED_CHAR | UNEXPECTED_CHAR |
	| READING_COMMENT_2      |        =        |        =        |        =        |
	| ACCEPT_COMMENT         |        -        |        -        |        -        |
	| ACCEPT_IN_REDIRECT     |        -        |        -        |        -        |
	| ACCEPT_OUT_REDIRECT    |        -        |        -        |        -        |
	| ACCEPT_PIPE            |        -        |        -        |        -        |
	| ACCEPT_BACKGROUND_EXEC |        -        |        -        |        -        |
	| ACCEPT_NEW_LINE        |        -        |        -        |        -        |
	| ACCEPT_EOF             |        -        |        -        |        -        |
	| READING_ID             |    ACCEPT_ID    |    ACCEPT_ID    | UNEXPECTED_CHAR |
	| ACCEPT_ID              |        -        |        -        |        -        |
	| READING_EXTENDED_ID_1  | UNEXPECTED_CHAR | UNEXPECTED_CHAR | UNEXPECTED_CHAR |
	| READING_EXTENDED_ID_2  | UNEXPECTED_CHAR | UNEXPECTED_CHAR | UNEXPECTED_CHAR |
	| ACCEPT_EXTENDED_ID     |        -        |        -        |        -        |
	| UNEXPECTED_CHAR        |        -        |        -        |        -        |
	--------------------------------------------------------------------------------


	Accept states: ACCEPT_*, UNEXPECTED_CHAR;
	Legend: The state = means that the next state is the same as the current state
	and the state - means don't care.
	*/

	//Current state START:
	automaton_matrix[START][ASTERISK]      = READING_COMMENT_1;
	automaton_matrix[START][LESS]          = ACCEPT_IN_REDIRECT;
	automaton_matrix[START][GREATER]       = ACCEPT_OUT_REDIRECT;
	automaton_matrix[START][NEW_LINE_CHAR] = START;
	automaton_matrix[START][WHITE_SPACE]   = START;
	automaton_matrix[START][END_OF_FILE]   = ACCEPT_EOF;
	automaton_matrix[START][VERTICAL_BAR]  = ACCEPT_PIPE;
	automaton_matrix[START][ID_CHAR]       = READING_ID;
	automaton_matrix[START][DOUBLE_QUOTES] = READING_EXTENDED_ID_1;
	automaton_matrix[START][AMPERSAND]     = ACCEPT_BACKGROUND_EXEC;
	automaton_matrix[START][TAB]           = START;
	automaton_matrix[START][BACKSLASH]     = ACCEPT_NEW_LINE;
	automaton_matrix[START][ELSE]          = UNEXPECTED_CHAR;

	//Current state READING_COMMENT_1:
	automaton_matrix[READING_COMMENT_1][ASTERISK]      = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_1][LESS]          = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][GREATER]       = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][NEW_LINE_CHAR] = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][WHITE_SPACE]   = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][END_OF_FILE]   = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][VERTICAL_BAR]  = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][ID_CHAR]       = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][DOUBLE_QUOTES] = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][AMPERSAND]     = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][TAB]           = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][BACKSLASH]     = UNEXPECTED_CHAR;
	automaton_matrix[READING_COMMENT_1][ELSE]          = UNEXPECTED_CHAR;

	//Current state READING_COMMENT_2:
	automaton_matrix[READING_COMMENT_2][ASTERISK]      = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][LESS]          = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][GREATER]       = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][NEW_LINE_CHAR] = ACCEPT_COMMENT;
	automaton_matrix[READING_COMMENT_2][WHITE_SPACE]   = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][END_OF_FILE]   = ACCEPT_COMMENT;
	automaton_matrix[READING_COMMENT_2][VERTICAL_BAR]  = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][ID_CHAR]       = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][DOUBLE_QUOTES] = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][AMPERSAND]     = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][TAB]           = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][BACKSLASH]     = READING_COMMENT_2;
	automaton_matrix[READING_COMMENT_2][ELSE]          = READING_COMMENT_2;


	//Current state ACCEPT_COMMENT:
	//don't care


	//Current state ACCEPT_IN_REDIRECT:
	//don't care

	//Current state ACCEPT_OUT_REDIRECT:
	//don't care

	//Current state ACCEPT_PIPE:
	//don't care

	//Current state ACCEPT_BACKGROUND_EXEC:
	//don't care

	//Current state ACCEPT_NEW_LINE:
	//don't care

	//Current state ACCEPT_EOF:
	//don't care

	//Current state READING_ID:
	automaton_matrix[READING_ID][ASTERISK]      = UNEXPECTED_CHAR;
	automaton_matrix[READING_ID][LESS]          = ACCEPT_ID;
	automaton_matrix[READING_ID][GREATER]       = ACCEPT_ID;
	automaton_matrix[READING_ID][NEW_LINE_CHAR] = ACCEPT_ID;
	automaton_matrix[READING_ID][WHITE_SPACE]   = ACCEPT_ID;
	automaton_matrix[READING_ID][END_OF_FILE]   = ACCEPT_ID;
	automaton_matrix[READING_ID][VERTICAL_BAR]  = ACCEPT_ID;
	automaton_matrix[READING_ID][ID_CHAR]       = READING_ID;
	automaton_matrix[READING_ID][DOUBLE_QUOTES] = UNEXPECTED_CHAR;
	automaton_matrix[READING_ID][AMPERSAND]     = ACCEPT_ID;
	automaton_matrix[READING_ID][TAB]           = ACCEPT_ID;
	automaton_matrix[READING_ID][BACKSLASH]     = ACCEPT_ID;
	automaton_matrix[READING_ID][ELSE]          = UNEXPECTED_CHAR;

	//Current state ACCEPT_ID:
	//don't care

	//Current state READING_EXTENDED_ID_1:
	automaton_matrix[READING_EXTENDED_ID_1][ASTERISK]      = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][LESS]          = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][GREATER]       = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][NEW_LINE_CHAR] = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][WHITE_SPACE]   = READING_EXTENDED_ID_1;
	automaton_matrix[READING_EXTENDED_ID_1][END_OF_FILE]   = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][VERTICAL_BAR]  = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][ID_CHAR]       = READING_EXTENDED_ID_2;
	automaton_matrix[READING_EXTENDED_ID_1][DOUBLE_QUOTES] = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][AMPERSAND]     = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][TAB]           = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][BACKSLASH]     = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_1][ELSE]          = UNEXPECTED_CHAR;

	//Current state READING_EXTENDED_ID_2:
	automaton_matrix[READING_EXTENDED_ID_2][ASTERISK]      = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][LESS]          = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][GREATER]       = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][NEW_LINE_CHAR] = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][WHITE_SPACE]   = READING_EXTENDED_ID_2;
	automaton_matrix[READING_EXTENDED_ID_2][END_OF_FILE]   = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][VERTICAL_BAR]  = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][ID_CHAR]       = READING_EXTENDED_ID_2;
	automaton_matrix[READING_EXTENDED_ID_2][DOUBLE_QUOTES] = ACCEPT_EXTENDED_ID;
	automaton_matrix[READING_EXTENDED_ID_2][AMPERSAND]     = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][TAB]           = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][BACKSLASH]     = UNEXPECTED_CHAR;
	automaton_matrix[READING_EXTENDED_ID_2][ELSE]          = UNEXPECTED_CHAR;

	//Current state ACCEPT_EXTENDED_ID:
	//don't care

	//Current state UNEXPECTED_CHAR:
	//don't care
	

	//Populate accept states array:
	accept_states[START]                  = false;
	accept_states[READING_COMMENT_1]      = false;
	accept_states[READING_COMMENT_2]      = false;
	accept_states[ACCEPT_COMMENT]         = true;
	accept_states[ACCEPT_IN_REDIRECT]     = true;
	accept_states[ACCEPT_OUT_REDIRECT]    = true;
	accept_states[ACCEPT_PIPE]            = true;
	accept_states[ACCEPT_BACKGROUND_EXEC] = true;
	accept_states[ACCEPT_NEW_LINE]        = true;
	accept_states[ACCEPT_EOF]             = true;
	accept_states[READING_ID]             = false;
	accept_states[ACCEPT_ID]              = true;
	accept_states[READING_EXTENDED_ID_1]  = false;
	accept_states[READING_EXTENDED_ID_2]  = false;
	accept_states[ACCEPT_EXTENDED_ID]     = true;
	accept_states[UNEXPECTED_CHAR]        = true;

}

enum input_type get_input_type(char c)
{
	if(c == '*') return ASTERISK;
	else if (c == '<') return LESS;
	else if (c == '>') return GREATER;
	else if (c == '\n') return NEW_LINE_CHAR;
	else if (c == ' ') return WHITE_SPACE;
	else if (c == '\0') return END_OF_FILE;
	else if (c == '|') return VERTICAL_BAR;
	else if (isalnum(c) || c == '-' || c == '/' || c == '_' || c == '.') return ID_CHAR;
	else if (c == '"') return DOUBLE_QUOTES;
	else if (c == '&') return AMPERSAND;
	else if (c == '\t') return TAB;
	else if (c == '\\') return BACKSLASH;
	else return ELSE;
}

void generate_error_message(char **error_msg_ref, size_t initial_index, size_t current_index, char str[], enum automaton_state current_state, enum automaton_state next_state)
{
	*error_msg_ref = "Generic error message";
}

