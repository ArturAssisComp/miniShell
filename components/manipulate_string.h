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

//Function Declaration:
token **get_tokens(const char *str);
int _get_next_token(token *recognized_token, const char * str, int index);
#endif
