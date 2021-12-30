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
	int out_red_c; //Number of output redirect
} command;

//Function Declaration:
command **get_commands(token **tokens);
token **get_tokens(const char *str);
void delete_token_array(token ***token_array_adress);
void delete_command_array(command ***command_array_address);
#endif
