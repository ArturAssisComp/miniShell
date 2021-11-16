#ifndef LEXER_H
#define LEXER_H

#include

//Data type:
typedef enum 
{
	RES_WORD,        //Reserved word
	ID,              
	IN_REDIRECT,
	OUT_REDIRECT,
	PIPE,
	BACKGROUNG_EXEC, 
	NEW_LINE,
	EOF_TOKEN
} L_token_type;

typedef struct
{
	L_token_type (continuar) 
} L_token;

#endif
