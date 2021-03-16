/**
 * Author: Artur Assis Alves
 * Title: MiniShell - Manipulate Strings
 */

//Includes:
#include "manipulate_string.h"
#include "memory_allocation.h"
#include <string.h>



//Automata definitions:
typedef enum
{
	START,
	READING_ARG,
	GET_OP,
	GET_ARG,
	NOT_RECOGNIZED,
	EMPTY_STR
} state;


//Aux function declarations:
int _get_next_token(token *recognized_token, const char * str, int index);
int _is_white_char(char str);
int _is_op(char str);
int _is_arg_char(char str);



//Function definitions:
token **get_tokens(const char * str)
/**
 * Description: This function applies _get_next_token on the string 'str' in such 
 * a way that every token is obtained and saved in an array. If all chars from the
 * string 'str' are recognized, then the result is returned. Otherwise, an error
 * message is printed and NULL is returned. If the 'str' is empty, an array with 
 * value NULL in the position 0 is returned. The order of analysis is from left to
 * right.
 * 
 * Input: (const char *) str --> string from which the tokens will be extracted.
 * 
 * Output: (token **) --> an array of pointers. Each pointer is a token. 
 */
{
	int index, number_of_tokens, i;
	size_t array_length = 2; //Initial guess (must be greater than 1).

	token **token_array;

	//Allocate memory for token_array:
	token_array = (token **)xcalloc(array_length, sizeof (token));

	//Get each token of the input line:
	index = 0;
	i = 0;
	do{
		if(i >= array_length)
		{
			array_length *= 2;
			token_array = (token **)xrealloc(token_array, sizeof (token) * array_length);
		}
		token_array[i] = (token *) xmalloc(sizeof (token));
		index = _get_next_token(token_array[i], str, index);

		//Check the received token:
		if (token_array[i]->type == NOT_RECOGNIZED_CHAR)
		{
			printf("--> Input '%s' is not recognized.\n", token_array[i]->string);
			return NULL;
		}
		else if (token_array[i]->string != NULL) //It is not empty string.
			//Increment the number of tokens:
			number_of_tokens = ++i;

	}while (index != -1 && index != 0);

	//NULL indicates the end of the array.
	token_array[i] = NULL; 

	return token_array;
}

int _get_next_token(token *recognized_token, const char * str, int index)
/*
 * Description: This function reads the next token from string 'str', saves it 
 * in 'recognized_token' and returns the index on which it stopped. If an unrecognized
 * char or '\0' is read, it returns -1. The rules to recognize a token are: 
 * 1 - The token can be the name/path of a program or an argument for that program. 
 *     The rule is ([a-z]|[A-Z]|[0-9]|[./_])+ ;
 * 2 - The token can be an operator: ([|<>])+ ;
 * 3 - Whitespaces are ignored;
 * 4 - Any other char is not recognized.
 *
 * Input: (token *) recognized_token --> Store informations about the token.
 *        (const char * ) str --> String from which the function will extract the
 *                               tokens.
 *        (int) index --> index to start the analysis.
 *
 * Output: (int) --> The index from which the analysis will continue. 
 */
{
	state current_state = START;
	int initial_index = index;
	int size;
	while(1)
	{

		switch (current_state)
		{
			case START:
				//Go to the next state:
				if(str[index] == '\0') //next char --> '\0'
					current_state = EMPTY_STR;
				else if(_is_op(str[index])) //next char --> operator
					current_state = GET_OP;
				else if(_is_arg_char(str[index]))//next char --> arg char
					current_state = READING_ARG;
				else if (_is_white_char(str[index])) 
					initial_index = index + 1;
				else//next char --> not recognized
					current_state = NOT_RECOGNIZED; //error

				//Consume char:
				index++; 
				break;

			case READING_ARG:
				//Go to the next state:
				if(str[index] == '\0' || _is_op(str[index]) || _is_white_char(str[index])) //next char --> '\0' or operator or whitespace
					current_state = GET_ARG;
				else if (_is_arg_char(str[index]))  //next char --> arg char
					index++; //Consume char.
				else //next char --> not recognized
				{
					current_state = NOT_RECOGNIZED;
					index++;
				}
				break;

			case GET_OP:
				size = index - initial_index;
				recognized_token->string = (char *)xcalloc(size + 1, sizeof (char));
				recognized_token->string[size] = '\0';
				memcpy(recognized_token->string, str + initial_index, size);
				recognized_token->type = OP;
////////////////////
//printf("Debug GET_OP--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
/////////////////////
				return index;
				break;

			case GET_ARG:
				size = index - initial_index;
				recognized_token->string = (char *)xcalloc(size + 1, sizeof (char));
				recognized_token->string[size] = '\0';
				memcpy(recognized_token->string, str + initial_index, size);
				recognized_token->type = ARG;
////////////////////
//printf("Debug GET_ARG--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
/////////////////////
                                //Check if the last char read was '\0':
                                if(str[index] == '\0')
					return -1;
				return index;
				break;

			case NOT_RECOGNIZED:
				size = index - initial_index;
				recognized_token->string = (char *)xcalloc(size + 1, sizeof (char));
				recognized_token->string[size] = '\0';
				memcpy(recognized_token->string, str + initial_index, size);
				recognized_token->type = NOT_RECOGNIZED_CHAR;
////////////////////
//printf("Debug NOT_RECOGNIZED--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
/////////////////////
				return -1;
				break;

			case EMPTY_STR:
				recognized_token->string = NULL;
				recognized_token->type = ARG;
				return -1;
				break;

		}
	}
}


int _is_arg_char(char str)
{
	//Arguments may contain [a-z] | [A - Z] | [0 - 9] | [./]
	if ((str >= 'a' && str <= 'z') || (str >= 'A' && str <= 'Z'))
		return 1;
	if(str == '.' || str == '/')
		return 1;
	if(str >= '0' && str <= '9')
		return 1;
	if(str == '_')
		return 1;
	return 0;

}

int _is_op(char str)
{

	if(str == '>' || str == '<' || str == '|')
		return 1;
	return 0;
}

int _is_white_char(char str)
{
	if(str == ' ' || str == '\t' || str == '\r' || str == '\n')
		return 1;
	return 0;

}
