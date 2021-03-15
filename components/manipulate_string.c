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
int _is_white_char(char str);
int _is_op(char str);
int _is_arg_char(char str);



//Function definitions:
get_tokens(const char * str)
{
	/*
	char line[100];
	int index, i;
	token *my_token;
	my_token = (token *) xmalloc(sizeof *my_token);
	token new_token;
	while(fgets(line, 99, stdin))
	{
		index = 0;
		i = 0;
		while (index != -1)
		{
			index = _get_next_token(&new_token, line, index);

			//Empty string:
			if (index == 0)
				break;

			//Print the next word:
			printf("word[%d] = %s\n", ++i, new_token.string);
		}
	}
		
	return 0;
	*/

	;
}

int _get_next_token(token *recognized_token, const char * str, int index)
/*
 * Description: This function reads the next token from string 'str', saves it 
 * in 'recognized_token' and returns the index on which it stopped. If an unrecognizeble
 * char is read, it returns -1.
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
					current_state = NOT_RECOGNIZED;
				break;

			case GET_OP:
				size = index - initial_index;
				recognized_token->string = (char *)xcalloc(size + 1, sizeof (char));
				recognized_token->string[size] = '\0';
				memcpy(recognized_token->string, str + initial_index, size);
				recognized_token->type = OP;
////////////////////
printf("Debug GET_OP--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
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
printf("Debug GET_ARG--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
/////////////////////
                                //Check if the last char read was '\0':
                                if(str[index] == '\n')
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
printf("Debug NOT_RECOGNIZED--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
/////////////////////
				return -1;
				break;

			case EMPTY_STR:
				recognized_token->string = NULL;
				return 0;
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
