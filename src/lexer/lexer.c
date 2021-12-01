#include <stdbool.h>
#include "lexer.h"
#include "lexer_automaton.h"

#define INITIAL_ARRAY_CAPACITY 16

//Local function declarations:
static struct L_token *get_next_token(char str[], size_t *current_index);

//Function definitions:
struct L_token_array *L_read_tokens(char str[], char **error_msg_ref)
/**
 *
 * Description: This function reads the string 'str' and returns an array with all 
 * the tokens read from it. The tokens are defined by the rules from the 
 * documentation of the miniShell program.
 *
 * Error handling: This function returns NULL in case that an error happens. 
 * If an error happens, the error message will be stored at *error_msg_ref pointer.
 *
 * Memory issues: After using the returned token array, the user must call the 
 * function L_delete_token_array passing that token array as argument. As the result, the 
 * memory allocated will be deallocated. If an error happens, the user must call
 * free for the pointer *error_msg_ref.
 *
 * Input: (char []) str -> The input must be a '\0' terminated string. Otherwise, 
 * the behavior is undefined.
 *        (char **) error_msg_ref -> The address of a pointer to char. The pointer
 *        *error_msg_ref must have value equals NULL, otherwise the behavior is 
 *        undefined.
 *
 */
{
	size_t current_index = 0;
	size_t array_capacity = 0;
	struct L_token *current_token = NULL;
	struct L_token_array *token_array = NULL;

	//Initialize the token array:
	token_array = malloc(sizeof *token_array);
	if(token_array == NULL) goto error;

	token_array->num_of_tokens = 0;
	//Allocate initial capacity size memory for the array:
	token_array->array = calloc(INITIAL_ARRAY_CAPACITY, sizeof *(token_array->array));
	if(token_array->array == NULL) goto error;
	array_capacity = INITIAL_ARRAY_CAPACITY;


	do
	{
		current_token = get_next_token(str, &current_index, error_msg_ref);
		if(current_token == NULL) goto error;

		//Check if the array capacity is enough to add a new token:
		if(token_array->num_of_tokens >= array_capacity)
		{
			//Duplicate the array capacity:
			array_capacity *= 2;
			tmp_ptr = reallocarray(token_array->array, array_capacity, sizeof *(token_array->array));
			if(tmp_ptr == NULL) goto error;
			token_array->array = tmp_ptr;
		}
		//Add the new token to the array:
		token_array->array[token_array->num_of_tokens++] = current_token; 
		current_token = NULL;
	}while(current_token->token_type != EOF_TOKEN);


normal_return:
	return token_array;
error:
	if(token_array != NULL) L_delete_token_array(&token_array);
	return token_array;
}

//Local function definitions:
static struct L_token *get_next_token(char str[], size_t *start_index, char **error_msg_ref)
/**
 * Error handling: If this function can not get the next token from the string 
 * 'str', it returns NULL and it does not change 'start_index'. It stores the 
 * error message into the the pointer *error_msg_ref.
 *
 * Memory issues: This function allocates memory for the next token and for 
 * its value, if necessary (if the token has semantic value). The user must free 
 * them after using. If an error happens, it allocates memory for the error message.
 * The memory is allocated for the pointer *error_msg_ref.
 *
 * Input: (char []) str -> The string 'str' must be '\0' terminated, otherwise,
 *                         the behavior is undefined.
 *        (size_t *) start_index -> This input must be the address of the variable
 *                                    that stores the value of the starting index
 *                                    from which the token will be extracted. 
 *                                    This value must be less than strlen(str), otherwise,
 *                                    NULL is returned.
 *        (char **) error_msg_ref -> The address of a pointer to char. The pointer
 *        *error_msg_ref must have value equals NULL, otherwise the behavior is 
 *        undefined.
 */
{
	L_token *next_token = NULL;

	//Check inputs:
	if(str == NULL || start_index == NULL || start_index >= strlen(str)) goto error;

	//Allocate memory:
	next_token = malloc(sizeof *next_token);
	if(next_token == NULL) goto error;

	//Call the lexer automaton:
	if(!LA_execute_lexer_automaton(str, start_index, next_token, error_msg_ref)) goto error;

	


return_result:
	return next_token;

error:
	//Free allocated memory:
	if(next_token != NULL)
	{
		if(next_token->token_value != NULL) free(next_token->token_value);
		free(next_token);

	}
	return NULL;
}

