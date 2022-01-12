#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "grammar_rules.h"
#include "../lexer/lexer.h"
#include "parser.h"

//Local function declarations:
static bool token_array (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_linked_list *command_pipeline_linked_list, char error_msg[P_ERROR_MSG_SZ]);      
static bool body (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_linked_list *command_pipeline_linked_list, char error_msg[P_ERROR_MSG_SZ]);             
static bool content (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_node *command_pipeline_node, char error_msg[P_ERROR_MSG_SZ]);          
static bool command_pipeline (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_node *command_pipeline_node, char error_msg[P_ERROR_MSG_SZ]); 
static bool next_command (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_node *command_pipeline_node, char error_msg[P_ERROR_MSG_SZ]);     
static bool command (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ]);          
static bool command_id (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ]);       
static bool resources (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ]);        
static bool resources1 (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ]);       
static bool argument (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ]);         
static bool input_redirect (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ]);   
static bool output_redirect (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ]);  
static bool piped_command (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ]);    
static bool modifiers (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_node *command_pipeline_node, char error_msg[P_ERROR_MSG_SZ]);        
static bool comment_line (size_t *current_index_address, struct L_token_array *array_of_tokens, char error_msg[P_ERROR_MSG_SZ]);     
static bool end (size_t *current_index_address, struct L_token_array *array_of_tokens, char error_msg[P_ERROR_MSG_SZ]);              


static char *get_token_type_str(enum L_token_type t);


//function definitions:
bool GR_start (struct L_token_array *array_of_tokens, struct P_command_pipeline_linked_list *command_pipeline_linked_list, char error_msg[P_ERROR_MSG_SZ])
{
    size_t current_index = 0;
    return token_array (&current_index, array_of_tokens, command_pipeline_linked_list, error_msg);      
}

//Local function definitions:
static bool token_array (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_linked_list *command_pipeline_linked_list, char error_msg[P_ERROR_MSG_SZ])
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected ID, COMMENT or EOF_TOKEN as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
        case COMMENT:
        case EOF_TOKEN:
            //<token_array> ::= <body> <comment_line> <end>
            result = body (current_index_address, array_of_tokens, command_pipeline_linked_list, error_msg); 
            if(result) result = comment_line (current_index_address, array_of_tokens, error_msg);     
            if(result) result = end (current_index_address, array_of_tokens, error_msg);              
            break;
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID, COMMENT or EOF_TOKEN as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;
        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected ID, COMMENT or EOF_TOKEN as next token.");
            break;
    }

return_result:
    return result;
}    


static bool body (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_linked_list *command_pipeline_linked_list, char error_msg[P_ERROR_MSG_SZ])             
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected ID, COMMENT or EOF_TOKEN as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //Create the first command pipeline node:
            command_pipeline_linked_list->head = malloc(sizeof *(command_pipeline_linked_list->head));
            if(!command_pipeline_linked_list->head)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Memory allocation error at file %s in function body() (L. %d).", __FILE__, __LINE__);
                goto return_result;
            }
            command_pipeline_linked_list->tail = command_pipeline_linked_list->head;

            //Update pipeline linked list length and initialize the first pipeline:
            command_pipeline_linked_list->length++;
            command_pipeline_linked_list->tail->first_command = NULL;
            command_pipeline_linked_list->tail->last_command = NULL;
            command_pipeline_linked_list->tail->length = 0;
            command_pipeline_linked_list->tail->remaining_to_execute = 0;
            command_pipeline_linked_list->tail->was_executed = false;
            command_pipeline_linked_list->tail->returned_status = 0;
            command_pipeline_linked_list->tail->execute_in_background = false;
            command_pipeline_linked_list->tail->next_pipeline = NULL;

            //<body> ::= <content> <modifiers>
            result = content(current_index_address, array_of_tokens, command_pipeline_linked_list->tail, error_msg);
            if(result) result = modifiers(current_index_address, array_of_tokens, command_pipeline_linked_list->tail, error_msg);
            break;

        case COMMENT:
        case EOF_TOKEN:
            //<body> ::= (empty)
            result = true;
            break;

        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID, COMMENT or EOF_TOKEN as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected ID, COMMENT or EOF_TOKEN as next token.");
            break;
    }

return_result:
    return result;
}    

static bool content (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_node *command_pipeline_node, char error_msg[P_ERROR_MSG_SZ])          
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected ID as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //<content> ::= <command_pipeline>
            result = command_pipeline(current_index_address, array_of_tokens, command_pipeline_node, error_msg);
            break;

        case COMMENT:
        case EOF_TOKEN:
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected ID as next token.");
            break;
    }

return_result:
    return result;
}    



static bool command_pipeline (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_node *command_pipeline_node, char error_msg[P_ERROR_MSG_SZ]) 
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected ID as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //Create the first command:
            command_pipeline_node->first_command = malloc(sizeof *(command_pipeline_node->first_command));
            if(!command_pipeline_node->first_command)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Memory allocation error at file %s in function command_pipeline() (L. %d).", __FILE__, __LINE__);
                goto return_result;
            }
            command_pipeline_node->last_command = command_pipeline_node->first_command;

            //Update pipeline length and initialize the command:
            command_pipeline_node->remaining_to_execute = ++(command_pipeline_node->length);
            command_pipeline_node->last_command->id = NULL;
            command_pipeline_node->last_command->args_list[0] = NULL;
            command_pipeline_node->last_command->input_redirection_id = NULL;
            command_pipeline_node->last_command->output_redirection_ids_list[0] = NULL;
            command_pipeline_node->last_command->next_pipelined_command = NULL;
            command_pipeline_node->last_command->returned_status = 0;
            command_pipeline_node->last_command->was_executed = false;
            command_pipeline_node->last_command->num_of_args = 0;
            command_pipeline_node->last_command->num_of_output_redirection_ids = 0;


            //<command_pipeline> ::= <command> <next_command>
            result = command(current_index_address, array_of_tokens, command_pipeline_node->last_command, error_msg);
            if(result) result = next_command(current_index_address, array_of_tokens, command_pipeline_node, error_msg);
            break;

        case COMMENT:
        case EOF_TOKEN:
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected ID as next token.");
            break;
    }

return_result:
    return result;
}    



static bool next_command (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_node *command_pipeline_node, char error_msg[P_ERROR_MSG_SZ])     
{
    struct P_command *tmp_command;
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected PIPE, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case PIPE:
            //Create the next command:
            tmp_command = malloc(sizeof *(tmp_command));
            if(!tmp_command)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Memory allocation error at file %s in function next_command() (L. %d).", __FILE__, __LINE__);
                goto return_result;
            }
            command_pipeline_node->last_command->next_pipelined_command = tmp_command;
            command_pipeline_node->last_command = tmp_command;

            //Update pipeline length and initialize the command:
            command_pipeline_node->remaining_to_execute = ++(command_pipeline_node->length);
            command_pipeline_node->last_command->id = NULL;
            command_pipeline_node->last_command->args_list[0] = NULL;
            command_pipeline_node->last_command->input_redirection_id = NULL;
            command_pipeline_node->last_command->output_redirection_ids_list[0] = NULL;
            command_pipeline_node->last_command->next_pipelined_command = NULL;
            command_pipeline_node->last_command->returned_status = 0;
            command_pipeline_node->last_command->was_executed = false;
            command_pipeline_node->last_command->num_of_args = 0;
            command_pipeline_node->last_command->num_of_output_redirection_ids = 0;
            
            //<next_command> ::= PIPE <piped_command> <next_command>
            (*current_index_address)++; /*Consume the current token PIPE*/
            result = piped_command(current_index_address, array_of_tokens, command_pipeline_node->last_command, error_msg);
            if(result) result = next_command(current_index_address, array_of_tokens, command_pipeline_node, error_msg);
            break;

        case COMMENT:
        case EOF_TOKEN:
        case BACKGROUND_EXEC:
            //<next_command> ::= (empty)
            result = true;
            break;

        case ID:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected PIPE, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.", *current_index_address + 1, array_of_tokens->array[current_index]->token_value);
            break;

        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected PIPE, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected PIPE, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.");
            break;
    }

return_result:
    return result;
}    



static bool command (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ])          
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected ID as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //<command> ::= <command_id> <resources>
            result = command_id(current_index_address, array_of_tokens, command, error_msg);
            if(result) result = resources(current_index_address, array_of_tokens, command, error_msg);
            break;

        case COMMENT:
        case EOF_TOKEN:
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected ID as next token.");
            break;
    }

return_result:
    return result;
}    


static bool command_id (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ])       
{
    enum L_token_type current_token_type;
    char *tmp_str;
    size_t current_index = *current_index_address;
    size_t id_length;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected ID as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //Save the current id as command id:
            tmp_str = array_of_tokens->array[current_index]->token_value;
            id_length = strlen(tmp_str);
            command->id = calloc(id_length + 1, sizeof *(command->id));
            if(!command->id)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Memory allocation error at file %s in function command_id() (L. %d).", __FILE__, __LINE__);
                goto return_result;
            }
            strncpy(command->id, tmp_str, id_length + 1);


            //<command_id> ::= ID
            (*current_index_address)++; /*Consume the current token ID*/
            result = true;
            break;

        case COMMENT:
        case EOF_TOKEN:
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected ID as next token.");
            break;
    }

return_result:
    return result;
}    


static bool resources (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ])        
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected PIPE, ID, IN_REDIRECT, OUT_REDIRECT, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //<resources> ::= <argument> <resources>
            result = argument(current_index_address, array_of_tokens, command, error_msg);
            if(result) result = resources(current_index_address, array_of_tokens, command, error_msg);
            break;

        case IN_REDIRECT:
            //<resources> ::= <input_redirect> <resources1>
            result = input_redirect(current_index_address, array_of_tokens, command, error_msg);
            if(result) result = resources1(current_index_address, array_of_tokens, command, error_msg);
            break;

        case OUT_REDIRECT:
            //<resources> ::= <output_redirect> <resources>
            result = output_redirect(current_index_address, array_of_tokens, command, error_msg);
            if(result) result = resources(current_index_address, array_of_tokens, command, error_msg);
            break;

        case BACKGROUND_EXEC:
        case PIPE:
        case COMMENT:
        case EOF_TOKEN:
            //<resources> ::= (empty)
            result = true;
            break;

        case RES_WORD:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected PIPE, ID, IN_REDIRECT, OUT_REDIRECT, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected PIPE, ID, IN_REDIRECT, OUT_REDIRECT, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.");
            break;
    }

return_result:
    return result;
}    


static bool resources1 (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ])       
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected PIPE, ID, OUT_REDIRECT, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //<resources1> ::= <argument> <resources1>
            result = argument(current_index_address, array_of_tokens, command, error_msg);
            if(result) result = resources1(current_index_address, array_of_tokens, command, error_msg);
            break;


        case OUT_REDIRECT:
            //<resources1> ::= <output_redirect> <resources1>
            result = output_redirect(current_index_address, array_of_tokens, command, error_msg);
            if(result) result = resources1(current_index_address, array_of_tokens, command, error_msg);
            break;

        case BACKGROUND_EXEC:
        case PIPE:
        case COMMENT:
        case EOF_TOKEN:
            //<resources1> ::= (empty)
            result = true;
            break;

        case IN_REDIRECT:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Input stream already defined for the command \"%s\". The %d-th token (\"%s\") is unexpected. Expected PIPE, ID, OUT_REDIRECT, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.", command->id, *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        case RES_WORD:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected PIPE, ID, OUT_REDIRECT, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected PIPE, ID, OUT_REDIRECT, COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.");
            break;
    }

return_result:
    return result;
}    


static bool argument (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ])         
{
    enum L_token_type current_token_type;
    char *tmp_str;
    size_t current_index = *current_index_address;
    size_t id_length;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected ID as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //Save the current id as command argument:
            tmp_str = array_of_tokens->array[current_index]->token_value;
            id_length = strlen(tmp_str);
            if(command->num_of_args  >= P_MAX_ARGS)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: the command \"%s\" (%d-th token) exceded the maximum number of arguments allowed (P_MAX_ARGS = %d).", command->id, current_index, P_MAX_ARGS);
                goto return_result;
            }
            command->args_list[command->num_of_args] = calloc(id_length + 1, sizeof *(command->args_list[command->num_of_args]));
            if(!command->args_list[command->num_of_args])
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Memory allocation error at file %s in function argument() (L. %d).", __FILE__, __LINE__);
                goto return_result;
            }
            strncpy(command->args_list[command->num_of_args], tmp_str, id_length + 1);
            command->args_list[++(command->num_of_args)] = NULL;

            //<argument> ::= ID
            (*current_index_address)++; /*Consume the current token ID*/
            result = true;
            break;

        case COMMENT:
        case EOF_TOKEN:
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected ID as next token.");
            break;
    }

return_result:
    return result;
}    



static bool input_redirect (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ])   
{
    enum L_token_type current_token_type;
    char *tmp_str;
    size_t current_index = *current_index_address;
    size_t id_length;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected IN_REDIRECT as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case IN_REDIRECT:

            //<input_redirect> ::= IN_REDIRECT ID
            (*current_index_address)++; /*Consume the current token IN_REDIRECT*/
            current_index = *current_index_address;
            current_token_type = array_of_tokens->array[current_index]->token_type;
            if(current_token_type != ID)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
                goto return_result;
            }

            //Save the current id as command input redirect argument:
            tmp_str = array_of_tokens->array[current_index]->token_value;
            id_length = strlen(tmp_str);
            command->input_redirection_id = calloc(id_length + 1, sizeof *(command->input_redirection_id));
            if(!command->input_redirection_id)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Memory allocation error at file %s in function input_redirect() (L. %d).", __FILE__, __LINE__);
                goto return_result;
            }
            strncpy(command->input_redirection_id, tmp_str, id_length + 1);

            (*current_index_address)++; /*Consume the current token ID*/
            result = true;
            break;

        case ID:
        case COMMENT:
        case EOF_TOKEN:
        case RES_WORD:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected IN_REDIRECT as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected IN_REDIRECT as next token.");
            break;
    }

return_result:
    return result;
}    


static bool output_redirect (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ])  
{
    enum L_token_type current_token_type;
    char *tmp_str;
    size_t current_index = *current_index_address;
    size_t id_length;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected OUT_REDIRECT as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case OUT_REDIRECT:

            //<output_redirect> ::= OUT_REDIRECT ID
            (*current_index_address)++; /*Consume the current token OUT_REDIRECT*/
            current_index = *current_index_address;
            current_token_type = array_of_tokens->array[current_index]->token_type;
            if(current_token_type != ID)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
                goto return_result;
            }

            //Save the current id as command output redirect argument:
            tmp_str = array_of_tokens->array[current_index]->token_value;
            id_length = strlen(tmp_str);
            if(command->num_of_output_redirection_ids  >= P_MAX_OUTPUT_REDIRECTION_IDS)
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: the command \"%s\" (%d-th token) exceded the maximum number of output redirection arguments allowed (P_MAX_OUTPUT_REDIRECTION_IDS = %d).", command->id, current_index, P_MAX_OUTPUT_REDIRECTION_IDS);
                goto return_result;
            }
            command->output_redirection_ids_list[command->num_of_output_redirection_ids] = calloc(id_length + 1, sizeof *(command->output_redirection_ids_list[command->num_of_output_redirection_ids]));
            if(!command->output_redirection_ids_list[command->num_of_output_redirection_ids])
            {
                result = false;
                snprintf(error_msg, P_ERROR_MSG_SZ, "Memory allocation error at file %s in function output_redirect() (L. %d).", __FILE__, __LINE__);
                goto return_result;
            }
            strncpy(command->output_redirection_ids_list[command->num_of_output_redirection_ids], tmp_str, id_length + 1);
            command->output_redirection_ids_list[++(command->num_of_output_redirection_ids)] = NULL;

            (*current_index_address)++; /*Consume the current token ID*/
            result = true;
            break;

        case ID:
        case COMMENT:
        case EOF_TOKEN:
        case RES_WORD:
        case PIPE:
        case IN_REDIRECT:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected OUT_REDIRECT as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected OUT_REDIRECT as next token.");
            break;
    }

return_result:
    return result;
}    

static bool piped_command (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command *command, char error_msg[P_ERROR_MSG_SZ])    
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected ID as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case ID:
            //<piped_command> ::= <command_id> <resources1>
            result = command_id(current_index_address, array_of_tokens, command, error_msg);
            if(result) result = resources1(current_index_address, array_of_tokens, command, error_msg);
            break;

        case COMMENT:
        case EOF_TOKEN:
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case BACKGROUND_EXEC:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected ID as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected ID as next token.");
            break;
    }

return_result:
    return result;
}    



static bool modifiers (size_t *current_index_address, struct L_token_array *array_of_tokens, struct P_command_pipeline_node *command_pipeline_node, char error_msg[P_ERROR_MSG_SZ])        
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case COMMENT:
        case EOF_TOKEN:
            //<modifiers> ::= (empty)
            result = true;
            break;

        case BACKGROUND_EXEC:
            //<modifiers> ::= BACKGROUND_EXEC
            (*current_index_address)++; /*Consume the current token BACKGROUND_EXEC*/
            command_pipeline_node->execute_in_background = true;
            result = true;
            break;


        case ID:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.", *current_index_address + 1, array_of_tokens->array[current_index]->token_value);
            break;


        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected COMMENT, EOF_TOKEN, or BACKGROUND_EXEC as next token.");
            break;
    }

return_result:
    return result;
}    



static bool comment_line (size_t *current_index_address, struct L_token_array *array_of_tokens, char error_msg[P_ERROR_MSG_SZ])     
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected COMMENT or EOF_TOKEN as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case EOF_TOKEN:
            //<comment_line> ::= (empty) 
            result = true;
            break;

        case COMMENT:
            //<comment_line> ::= COMMENT 
            (*current_index_address)++; /*Consume the current token COMMENT*/
            result = true;
            break;


        case ID:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected COMMENT or EOF_TOKEN as next token.", *current_index_address + 1, array_of_tokens->array[current_index]->token_value);
            break;

        case BACKGROUND_EXEC:
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected COMMENT or EOF_TOKEN as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected COMMENT or EOF_TOKEN as next token.");
            break;
    }

return_result:
    return result;
}    


static bool end (size_t *current_index_address, struct L_token_array *array_of_tokens, char error_msg[P_ERROR_MSG_SZ])              
{
    enum L_token_type current_token_type;
    size_t current_index = *current_index_address;
    bool result;

    //Insufficient token stop condition:
    if(current_index >= array_of_tokens->num_of_tokens)
    {
        result = false;
        snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: insufficient tokens. Expected EOF_TOKEN as next token.");
        goto return_result;
    } 

    current_token_type = array_of_tokens->array[current_index]->token_type;

    //Select and execute the next production:
    switch(current_token_type)
    {
        case EOF_TOKEN:
            //<end> ::= EOF_TOKEN 
            (*current_index_address)++; /*Consume the current token COMMENT*/
            result = true;
            break;


        case ID:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected EOF_TOKEN as next token.", *current_index_address + 1, array_of_tokens->array[current_index]->token_value);
            break;

        case COMMENT:
        case BACKGROUND_EXEC:
        case RES_WORD:
        case IN_REDIRECT:
        case OUT_REDIRECT:
        case PIPE:
        case NEW_LINE:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. The %d-th token (\"%s\") is unexpected. Expected EOF_TOKEN as next token.", *current_index_address + 1, get_token_type_str(current_token_type));
            break;

        default:
            result = false;
            snprintf(error_msg, P_ERROR_MSG_SZ, "Syntax error: Unexpected token. Unrecognized token. Expected EOF_TOKEN as next token.");
            break;
    }

return_result:
    return result;
}    


static char *get_token_type_str(enum L_token_type t)
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
