#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#define L_ERROR_MSG_SZ 256

//Data type:
enum L_token_type
{
	RES_WORD,        //Reserved word
	ID,              
	IN_REDIRECT,
	OUT_REDIRECT,
	PIPE,
	BACKGROUND_EXEC, 
	NEW_LINE,
	COMMENT,
	EOF_TOKEN
};

struct L_token
{
	enum L_token_type token_type;
	char *token_value;
};

struct L_token_array
{
	struct L_token **array;
	size_t num_of_tokens;
};

//Function declarations:
struct L_token_array *L_read_tokens(char str[], char error_msg[L_ERROR_MSG_SZ]);
void L_delete_token_array(struct L_token_array **token_array_address);

#endif
