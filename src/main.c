/**
 * Author: Artur Assis Alves
 * Title: MiniShell - Main Function
 */

//Include:
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer/lexer.h"


//Define:
#define MAX_SIZE  1000 
#define MAX_WORDS 100

char *get_token_type_str(enum L_token_type t);

int main(){
	struct L_token_array *my_array;
	int i;
	char line[MAX_SIZE + 1];
	char *error_msg = NULL;

	//Initialize the shell:
	printf("\nminiSh\n");
	printf(">>> ");
	while(fgets(line, MAX_SIZE, stdin))
	{
		my_array = L_read_tokens(line, &error_msg);
		if(my_array == NULL)
		{
			printf("%s\n", error_msg);
			printf("\n>>> ");
		}
		else
		{
			i = 0;
			do 
			{
				printf("--> Token %d: %s", i, get_token_type_str(my_array->array[i]->token_type));
				if(my_array->array[i]->token_value) printf(" (V: %s)", my_array->array[i]->token_value);
				printf("\n");
				i++;
			}while(my_array->array[i]->token_type != EOF_TOKEN);
			printf("\n>>> ");
		}
	}

	//Free the allocated memory:
	i = 0;
	while (my_array->array[i]->token_type != EOF_TOKEN)
	{
		if (my_array->array[i]->token_value)
			free(my_array->array[i]->token_value);
	}
	free(my_array);
	return 0;
}



char *get_token_type_str(enum L_token_type t)
{
	if(t == RES_WORD) return "RES_WORD";
	else if (t == ID) return "ID";
	else if (t == IN_REDIRECT) return "<";
	else if (t == OUT_REDIRECT) return ">";
	else if (t == PIPE) return "|";
	else if (t == BACKGROUND_EXEC) return "&";
	else if (t == NEW_LINE) return "\\";
	else if (t == COMMENT) return "** <comment>";
	else if (t == EOF_TOKEN) return "EOF";
	else return "<not recognized>";
}
