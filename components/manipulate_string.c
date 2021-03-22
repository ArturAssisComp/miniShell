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
} state; //Get tokens


typedef enum
{
	FIRST_COMMAND,
	READ_FIRST_COMMAND_ARGS,
	READING_ARGUMENTS_FIRST_COMMAND,
	READ_SYMBOL_REDIRECT_OUTPUT_FIRST_COMMAND,
	READ_SYMBOL_REDIRECT_INPUT_FIRST_COMMAND,
	READ_REDIRECT_OUTPUT_ARG_FIRST_COMMAND,
	READ_REDIRECT_INPUT_ARG_FIRST_COMMAND,
	OTHER_COMMAND, //Commands after first command.
	READ_OTHER_COMMAND_ARGS,
	READING_ARGUMENTS_OTHER_COMMAND,
	READ_SYMBOL_REDIRECT_OUTPUT_OTHER_COMMAND,
	READ_REDIRECT_OUTPUT_ARG_OTHER_COMMAND
} command_state; //Get commands


//Aux function declarations:
int _get_next_token(token *recognized_token, const char * str, int index);
int _is_white_char(char str);
int _is_op(char str);
int _is_arg_char(char str);



//Function definitions:


command **get_commands(token **tokens)
/**
 * Description: This function reads commands from 'tokens'. Each command is 
 * stored in one element of a command array. The command array is returned
 * in the end of the operation. The last element is NULL. Each command is
 * composed by the command string ('command'), arguments ('argv'), number
 * of arguments ('argc'), input redirect string ('input_redirect'), 
 * output redirect array of strings ('output_redirect'), and the number
 * of output redirect strings ('out_red_c'). Each command must have at least
 * the string 'command'. Only the first command may have input redirect args 
 * and only the last command may have output redirect args. 
 * 	If 'tokens' is NULL, this function returns NULL.
 *
 * Input: (token **) tokens --> The tokens that will be read by this function.
 * 
 * Output: (command **) --> Array with commands.
 *
 * Time Complexity: -
 *
 * Space Complexity: O(n)
 */
{
	int command_index = 0, token_index = 0, arg_index, output_redirect_index;
	int num_of_commands = 0;
	command_state current_state = FIRST_COMMAND;
	command **command_array;
	size_t array_length = 2, str_size, args_array_length = 2, out_red_array_length = 2; //Initial guess (must be greater than 1).

	//Check if 'tokens' is NULL:
	if(tokens == NULL)
		return NULL;

	//Initialize the first command:
	command_array = (command **)xcalloc(array_length, sizeof (command *));
	command_array[command_index] = (command *)xmalloc(sizeof (command));
	num_of_commands++;
	command_array[command_index + 1] = NULL;

	//argv:
	arg_index = 0;
	command_array[command_index]->argv = (char **) xcalloc(args_array_length, sizeof (char *));
	command_array[command_index]->argv[arg_index] = NULL;
	command_array[command_index]->argc = 0;

	//Input redirect:
	command_array[command_index]->input_redirect = NULL;

	//Output redirect:
	output_redirect_index = 0;
	command_array[command_index]->output_redirect = (char **) xcalloc(out_red_array_length, sizeof (char *));
	command_array[command_index]->out_red_c = 0;
	command_array[command_index]->output_redirect[output_redirect_index] = NULL;



	while(tokens[token_index] != NULL)
	{
		switch(current_state)
		{
			case FIRST_COMMAND:
/////////////////////
//printf("Current state: FIRST_COMMAND.\n Reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					printf("Error: The first argument must be a command, but it is the operator %s.\n", tokens[token_index]->string);
					delete_command_array(&command_array);
					return NULL;
				}
				else //tokens[token_index]->type == ARG
				{
					//Read the first command:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->command = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->command, tokens[token_index]->string);

					//Update the state and loop variables:
					current_state = READ_FIRST_COMMAND_ARGS;
					token_index++;
				}
				break;

			case READ_FIRST_COMMAND_ARGS:
/////////////////////
//printf("Current state: FIRST_COMMAND_ARGS.\n Reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					switch(tokens[token_index]->string[0])
					{
						case '>':
							//Update the state and loop variables:
							current_state = READ_SYMBOL_REDIRECT_OUTPUT_FIRST_COMMAND ;
							token_index++;
							break;
						
						case '<':
							//Update the state and loop variables:
							current_state = READ_SYMBOL_REDIRECT_INPUT_FIRST_COMMAND;
							token_index++;
							break;

						case '|':
							//Update the state and loop variables:
							current_state = OTHER_COMMAND;
							token_index++;

							//Go to the next command.
							command_index++; 
							break;

						default:
							printf("Error: operator %c is not recognized.", tokens[token_index]->string[0]);
							delete_command_array(&command_array);
							return NULL;
					}
				}
				else //tokens[token_index]->type == ARG
				{
					//Read the first argument of the first command:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->argv[arg_index] = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->argv[arg_index], tokens[token_index]->string);
					command_array[command_index]->argc++;
					arg_index++;

					//Set the last element to NULL:
					command_array[command_index]->argv[arg_index] = NULL;

					//Update the state and loop variables:
					current_state = READING_ARGUMENTS_FIRST_COMMAND;
					token_index++;
				}
				break;

			case READING_ARGUMENTS_FIRST_COMMAND:
/////////////////////
//printf("Current state: READING_ARGUMENTS_FIRST_COMMAND.\n Reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					switch(tokens[token_index]->string[0])
					{
						case '>':
							//Update the state and loop variables:
							current_state = READ_SYMBOL_REDIRECT_OUTPUT_FIRST_COMMAND ;
							token_index++;
							break;
						
						case '<':
							//Update the state and loop variables:
							current_state = READ_SYMBOL_REDIRECT_INPUT_FIRST_COMMAND;
							token_index++;
							break;

						case '|':
							//Update the state and loop variables:
							current_state = OTHER_COMMAND;
							token_index++;

							//Go to the next command.
							command_index++; 
							break;

						default:
							printf("Error: operator %c is not recognized.", tokens[token_index]->string[0]);
							delete_command_array(&command_array);
							return NULL;
					}
				}
				else //tokens[token_index]->type == ARG
				{
					//Check if there is enough space to another arg:
					if(command_array[command_index]->argc + 1 > args_array_length)
					{
						//Allocate extra space:
						args_array_length *= 2;
						command_array[command_index]->argv = (char **)xrealloc(command_array[command_index]->argv, sizeof (char *) * args_array_length);
					}

					//Read the next argument of the first command:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->argv[arg_index] = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->argv[arg_index], tokens[token_index]->string);
					command_array[command_index]->argc++;
					arg_index++;

					//Set the last element to NULL:
					command_array[command_index]->argv[arg_index] = NULL;

					//Update the state and loop variables:
					token_index++;
				}
				break;

			case READ_SYMBOL_REDIRECT_OUTPUT_FIRST_COMMAND:
/////////////////////
//printf("Current state: READ_SYMBOL_REDIRECT_OUTPUT_FIRST_COMMAND.\n Reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					printf("Error: Illegal argument %c after '>'.", tokens[token_index]->string[0]);
					delete_command_array(&command_array);
					return NULL;
				}
				else //tokens[token_index]->type == ARG
				{
					//Read the next argument of the first command output redirect:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->output_redirect[output_redirect_index] = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->output_redirect[output_redirect_index], tokens[token_index]->string);
					command_array[command_index]->out_red_c++;
					output_redirect_index++;

					//Set the last element to NULL:
					command_array[command_index]->output_redirect[output_redirect_index] = NULL;
					
					//Update the state and loop variables:
					current_state = READ_REDIRECT_OUTPUT_ARG_FIRST_COMMAND;
					token_index++;
				}
				break;

			case READ_SYMBOL_REDIRECT_INPUT_FIRST_COMMAND:
/////////////////////
//printf("Current state: READ_SYMBOL_REDIRECT_INPUT_FIRST_COMMAND.\n Reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					printf("Error: Illegal argument %c after '<'.\n", tokens[token_index]->string[0]);
					delete_command_array(&command_array);
					return NULL;
				}
				else //tokens[token_index]->type == ARG
				{
					//Read the next argument of the first command input redirect:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->input_redirect = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->input_redirect, tokens[token_index]->string);
					
					//Update the state and loop variables:
					current_state = READ_REDIRECT_INPUT_ARG_FIRST_COMMAND;
					token_index++;
				}
				break;

			case READ_REDIRECT_OUTPUT_ARG_FIRST_COMMAND:
/////////////////////
//printf("Current state: READ_REDIRECT_OUTPUT_ARG_FIRST_COMMAND.\n Reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					switch(tokens[token_index]->string[0])
					{
						case '|': 
							printf("Error: Output redirect already defined, it is not possible to create a pipeline.\n");
							delete_command_array(&command_array);
							return NULL;
							break;

						case '>':
							printf("Error: Multiple output redirect symbols ('>').\n");
							delete_command_array(&command_array);
							return NULL;
							break;

						case '<':
							//Check if input redirect is not NULL:
							if(command_array[command_index]->input_redirect)
							{
								printf("Error: Multiple input redirect symbols ('<').\n");
								delete_command_array(&command_array);
								return NULL;
							}

							//Update the state and loop variables:
							current_state = READ_SYMBOL_REDIRECT_INPUT_FIRST_COMMAND;
							token_index++;
							break;
						default:
							printf("Error: operator %c is not recognized.", tokens[token_index]->string[0]);
							delete_command_array(&command_array);
							return NULL;
					}
				}
				else //tokens[token_index]->type == ARG
				{
					//Check if there is enough space to another arg:
					if(command_array[command_index]->out_red_c + 1 > out_red_array_length)
					{
						//Allocate extra space:
						out_red_array_length *= 2;
						command_array[command_index]->output_redirect = (char **)xrealloc(command_array[command_index]->output_redirect, sizeof (char *) * out_red_array_length);
					}
					
					//Read the next argument of the first command output redirect:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->output_redirect[output_redirect_index] = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->output_redirect[output_redirect_index], tokens[token_index]->string);
					command_array[command_index]->out_red_c++;
					output_redirect_index++;

					//Set the last element to NULL:
					command_array[command_index]->output_redirect[output_redirect_index] = NULL;
					
					//Update and loop variables:
					token_index++;
				}
				break;

			case READ_REDIRECT_INPUT_ARG_FIRST_COMMAND:
/////////////////////
//printf("Current state: READ_REDIRECT_INPUT_ARG_FIRST_COMMAND.\n Reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					switch(tokens[token_index]->string[0])
					{
						case '>':
							//Check if there is redirect output:
							if(command_array[command_index]->out_red_c > 0)
							{
								printf("Error: Multiple output redirect symbols ('>').\n");
								delete_command_array(&command_array);
								return NULL;
							}

							//Update the state and loop variables:
							current_state = READ_SYMBOL_REDIRECT_OUTPUT_FIRST_COMMAND ;
							token_index++;
							break;
						
						case '<':
							printf("Error: Multiple input redirect symbols ('<').\n");
							delete_command_array(&command_array);
							return NULL;
							break;

						case '|':
							//Check if there is redirect output:
							if(command_array[command_index]->out_red_c > 0)
							{
								printf("Error: Output redirect already defined, it is not possible to create a pipeline.\n");
								delete_command_array(&command_array);
								return NULL;
							}

							//Update the state and loop variables:
							current_state = OTHER_COMMAND;
							token_index++;

							//Go to the next command.
							command_index++; 
							break;

						default:
							printf("Error: operator %c is not recognized.", tokens[token_index]->string[0]);
							delete_command_array(&command_array);
							return NULL;
					}
				}
				else //tokens[token_index]->type == ARG
				{
					printf("Error: Only one argument to redirect input is accepted per command.\n");
					delete_command_array(&command_array);
					return NULL;
				}
				break;

			case OTHER_COMMAND:
/////////////////////
//printf("Current state: OTHER_COMMAND.\n Reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				//Check if there is enough space to another command:
				if(num_of_commands + 1 > array_length)
				{
					//allocate extra space:
					array_length *= 2;
					command_array = (command **)xrealloc(command_array, sizeof (command *) * array_length);
				}

				//allocate the next command:
				command_array[command_index] = (command *)xmalloc(sizeof (command));
				num_of_commands++;
				command_array[command_index + 1] = NULL;

				//argv:
				arg_index = 0;
				command_array[command_index]->argv = (char **) xcalloc(args_array_length, sizeof (char *));
				command_array[command_index]->argv[arg_index] = NULL;
				command_array[command_index]->argc = 0;

				//Input redirect:
				command_array[command_index]->input_redirect = NULL;

				//Output redirect:
				output_redirect_index = 0;
				command_array[command_index]->output_redirect = (char **) xcalloc(out_red_array_length, sizeof (char *));
				command_array[command_index]->out_red_c = 0;
				command_array[command_index]->output_redirect[output_redirect_index] = NULL;

				if(tokens[token_index]->type == OP)
				{
					printf("Error: command %c is not recognized.", tokens[token_index]->string[0]);
					delete_command_array(&command_array);
					return NULL;
				}
				else //tokens[token_index]->type == arg
				{
					//Read the first command:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->command = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->command, tokens[token_index]->string);

					//Update the state and loop variables:
					current_state = READ_OTHER_COMMAND_ARGS;
					token_index++;
				}
				break;

			case READ_OTHER_COMMAND_ARGS:
/////////////////////
//printf("current state: READ_OTHER_COMMAND_ARGS.\n reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					switch(tokens[token_index]->string[0])
					{
						case '>':
							//Update the state and loop variables:
							current_state = READ_SYMBOL_REDIRECT_OUTPUT_OTHER_COMMAND ;
							token_index++;
							break;
						
						case '<':
							printf("Error: Input redirect cannot be defined because this command is in a pipeline.\n");
							delete_command_array(&command_array);
							return NULL;
							break;

						case '|':
							//Update the state and loop variables:
							current_state = OTHER_COMMAND;
							token_index++;

							//Go to the next command.
							command_index++; 
							break;

						default:
							printf("Error: operator %c is not recognized.", tokens[token_index]->string[0]);
							delete_command_array(&command_array);
							return NULL;
					}
				}
				else //tokens[token_index]->type == arg
				{
					//Read the first argument of the other command:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->argv[arg_index] = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->argv[arg_index], tokens[token_index]->string);
					command_array[command_index]->argc++;
					arg_index++;

					//Set the last element to NULL:
					command_array[command_index]->argv[arg_index] = NULL;

					//Update the state and loop variables:
					current_state = READING_ARGUMENTS_OTHER_COMMAND;
					token_index++;
				}
				break;

			case READING_ARGUMENTS_OTHER_COMMAND:
/////////////////////
//printf("current state: READING_ARGUMENTS_OTHER_COMMAND.\n reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					switch(tokens[token_index]->string[0])
					{
						case '>':
							//Update the state and loop variables:
							current_state = READ_SYMBOL_REDIRECT_OUTPUT_OTHER_COMMAND ;
							token_index++;
							break;
						
						case '<':
							printf("Error: Input redirect cannot be defined because this command is in a pipeline.\n");
							delete_command_array(&command_array);
							return NULL;
							break;

						case '|':
							//Update the state and loop variables:
							current_state = OTHER_COMMAND;
							token_index++;

							//Go to the next command.
							command_index++; 
							break;

						default:
							printf("Error: operator %c is not recognized.", tokens[token_index]->string[0]);
							delete_command_array(&command_array);
							return NULL;
					}
				}
				else //tokens[token_index]->type == arg
				{
					//Check if there is enough space to another arg:
					if(command_array[command_index]->argc + 1 > args_array_length)
					{
						//Allocate extra space:
						args_array_length *= 2;
						command_array[command_index]->argv = (char **)xrealloc(command_array[command_index]->argv, sizeof (char *) * args_array_length);
					}

					//Read the first argument of the other command:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->argv[arg_index] = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->argv[arg_index], tokens[token_index]->string);
					command_array[command_index]->argc++;
					arg_index++;

					//Set the last element to NULL:
					command_array[command_index]->argv[arg_index] = NULL;

					//Update the state and loop variables:
					token_index++;
				}
				break;

			case READ_SYMBOL_REDIRECT_OUTPUT_OTHER_COMMAND:
/////////////////////
//printf("current state: READ_SYMBOL_REDIRECT_OUTPUT_OTHER_COMMAND.\n reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					printf("Error: Illegal argument %c after '>'.", tokens[token_index]->string[0]);
					delete_command_array(&command_array);
					return NULL;
				}
				else //tokens[token_index]->type == arg
				{
					//Read the next argument of the first command output redirect:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->output_redirect[output_redirect_index] = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->output_redirect[output_redirect_index], tokens[token_index]->string);
					command_array[command_index]->out_red_c++;
					output_redirect_index++;

					//Set the last element to NULL:
					command_array[command_index]->output_redirect[output_redirect_index] = NULL;
					
					//Update the state and loop variables:
					current_state = READ_REDIRECT_OUTPUT_ARG_OTHER_COMMAND;
					token_index++;
				}
				break;

			case READ_REDIRECT_OUTPUT_ARG_OTHER_COMMAND:
/////////////////////
//printf("current state: READ_REDIRECT_OUTPUT_ARG_OTHER_COMMAND.\n reading token: %s\n\n", tokens[token_index]->string);
////////////////////
				if(tokens[token_index]->type == OP)
				{
					printf("Error: Illegal argument %c after the last output redirect '>'.", tokens[token_index]->string[0]);
					delete_command_array(&command_array);
					return NULL;
				}
				else //tokens[token_index]->type == arg
				{
					//Check if there is enough space to another arg:
					if(command_array[command_index]->out_red_c + 1 > out_red_array_length)
					{
						//Allocate extra space:
						out_red_array_length *= 2;
						command_array[command_index]->output_redirect = (char **)xrealloc(command_array[command_index]->output_redirect, sizeof (char *) * out_red_array_length);
					}
					
					//Read the next argument of the other command output redirect:
					str_size = strlen(tokens[token_index]->string);
					command_array[command_index]->output_redirect[output_redirect_index] = (char *)xcalloc(str_size + 1, sizeof(char));
					strcpy(command_array[command_index]->output_redirect[output_redirect_index], tokens[token_index]->string);
					command_array[command_index]->out_red_c++;
					output_redirect_index++;

					//Set the last element to NULL:
					command_array[command_index]->output_redirect[output_redirect_index] = NULL;
					
					//Update and loop variables:
					token_index++;
				}
				break;
		}
	}

	//Reject the input:
	if(current_state == FIRST_COMMAND                               || 
	   current_state == READ_SYMBOL_REDIRECT_OUTPUT_FIRST_COMMAND   || 
	   current_state == READ_SYMBOL_REDIRECT_INPUT_FIRST_COMMAND    || 
	   current_state == OTHER_COMMAND                               ||
	   current_state == READ_SYMBOL_REDIRECT_OUTPUT_OTHER_COMMAND)
	{
		printf("Error: Incomplete command.");
		delete_command_array(&command_array);
		return NULL;
	}
	//return the array with commands:
	return command_array;
}



token **get_tokens(const char * str)
/**
 * description: this function applies _get_next_token on the string 'str' in such 
 * a way that every token is obtained and saved in an array. if all chars from the
 * string 'str' are recognized, then the result is returned. otherwise, an error
 * message is printed and NULL is returned. if the 'str' is empty, an array with 
 * value NULL in the position 0 is returned. the order of analysis is from left to
 * right. the end of the array is indicated with NULL.
 * 
 * input: (const char *) str --> string from which the tokens will be extracted.
 * 
 * output: (token **) --> an array of pointers. each pointer is a token. 
 */
{
	int index, number_of_tokens, i;
	size_t array_length = 2; //initial guess (must be greater than 1).

	token **token_array;

	//allocate memory for token_array:
	token_array = (token **)xcalloc(array_length, sizeof (token));

	//get each token of the input line:
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

		//check the received token:
		if (token_array[i]->type == NOT_RECOGNIZED_CHAR)
		{
			printf("--> input '%s' is not recognized.\n", token_array[i]->string);
			return NULL;
		}
		else if (token_array[i]->string != NULL) //it is not empty string.
			//increment the number of tokens:
			number_of_tokens = ++i;

	}while (index != -1 && index != 0);

	//NULL indicates the end of the array.
	token_array[i] = NULL; 

	return token_array;
}

int _get_next_token(token *recognized_token, const char * str, int index)
/*
 * description: this function reads the next token from string 'str', saves it 
 * in 'recognized_token' and returns the index on which it stopped. if an unrecognized
 * char or '\0' is read, it returns -1. the rules to recognize a token are: 
 * 1 - the token can be the name/path of a program or an argument for that program. 
 *     the rule is ([a-z]|[a-z]|[0-9]|[./_])+ ;
 * 2 - the token can be an operator: ([|<>])+ ;
 * 3 - whitespaces are ignored;
 * 4 - any other char is not recognized.
 *
 * input: (token *) recognized_token --> store informations about the token.
 *        (const char * ) str --> string from which the function will extract the
 *                               tokens.
 *        (int) index --> index to start the analysis.
 *
 * output: (int) --> the index from which the analysis will continue. 
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
				//go to the next state:
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

				//consume char:
				index++; 
				break;

			case READING_ARG:
				//go to the next state:
				if(str[index] == '\0' || _is_op(str[index]) || _is_white_char(str[index])) //next char --> '\0' or operator or whitespace
					current_state = GET_ARG;
				else if (_is_arg_char(str[index]))  //next char --> arg char
					index++; //consume char.
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
//printf("debug get_op--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
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
//printf("debug get_arg--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
/////////////////////
                                //check if the last char read was '\0':
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
//printf("debug not_recognized--> string: '%s' , size: '%d' , index: '%d' , initial_index: '%d'\n", recognized_token->string, size, index, initial_index);
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
	//arguments may contain [a-z] | [A - Z] | [0 - 9] | [./]
	if ((str >= 'A' && str <= 'Z') || (str >= 'a' && str <= 'z'))
		return 1;
	if(str == '.' || str == '/')
		return 1;
	if(str >= '0' && str <= '9')
		return 1;
	if(str == '_' || str == '-')
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


void delete_token_array(token ***token_array_address)
/**
 * description: this function deletes the token array pointed by
 * 'token_array_address'. it must be called after using the token array created
 * using function get_tokens. the user must not call free() by himself/herself.
 * after deleting the token array, it points the original pointer to NULL.
 *
 * input: (token ***) token_array_address --> pointer to a pointer of pointer to 
 *                                            token.
 *
 * output: (void)
 *
 * time complexity: -
 * 
 * space complexity: o(1)
 *
 */
{
	//initialize auxilliary variables:
	token **token_array;
	token_array = *token_array_address;
	int i = 0;

	//free each element of the array:
	while(token_array[i] != NULL)
	{
		free(token_array[i]->string);
		free(token_array[i]);
		i++;
	}

	//Free the token array itself:
	free(token_array);

	//Redirect *token_array_address to NULL:
	*token_array_address = NULL;
}

void delete_command_array(command ***command_array_address)
/**
 * Description: This function deletes the command array pointed by
 * 'command_array_address'. It must be called after using the command array created
 * using function get_commands. The user must not call free() by himself/herself.
 * After deleting the command array, it points the original pointer to NULL.
 *
 * Input: (command ***) command_array_address --> pointer to a pointer of pointer to 
 *                                            command.
 *
 * Output: (void)
 *
 * Time Complexity: -
 * 
 * Space Complexity: O(1)
 *
 */
{
	//Initialize auxilliary variables:
	command **command_array;
	command_array = *command_array_address;
	int i = 0, j;

	//Free each element of the array:
	while(command_array[i] != NULL)
	{
		if(command_array[i]->command)
			free(command_array[i]->command);
		if(command_array[i]->input_redirect)
			free(command_array[i]->input_redirect);
		j = 0;
		while(command_array[i]->argv[j] != NULL)
			free(command_array[i]->argv[j++]);
		j = 0;
		while(command_array[i]->output_redirect[j] != NULL)
			free(command_array[i]->output_redirect[j++]);

		i++;
	}

	//Free the command array itself:
	free(command_array);

	//Redirect *command_array_address to NULL:
	*command_array_address = NULL;
}
