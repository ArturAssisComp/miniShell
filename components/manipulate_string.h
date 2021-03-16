#ifndef MANIPULATE_STRING_H
#define MANIPULATE_STRING_H

//Structs and enums:
typedef enum
{
	ARG, //Argument
	OP,  //Operator
	NOT_RECOGNIZED_CHAR
} type;


typedef struct
{
	char *string;
	type type;
} token;

typedef struct
{
	char *command;
	char **argv;
	int argc;
	char *input_redirect;
	char **output_redirect;
} command;

//Function Declaration:
token **get_tokens(const char *str);
command **turn_to_commands(token **get_tokens);
void delete_token_array(token ***token_array_adress);
void delete_command_array(command ***command_array_address);
#endif
