#include "parser.h"
#include "grammar_rules.h"
#include "../lexer/lexer.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>


//Declaration of local functions:
static bool start_parsing_process(struct L_token_array *array_of_tokens, struct P_command_pipeline_linked_list *command_pipeline_linked_list, char error_msg[P_ERROR_MSG_SZ]);


//Function definitions:
struct P_command_pipeline_linked_list *P_parse(struct L_token_array *array_of_tokens, char error_msg[P_ERROR_MSG_SZ])
/**
 * Description: this function parses the array of tokens generating a linked
 * list with command pipelines. It uses the grammar described in the miniShell
 * docs version 1.
 *
 * Error handling: If this function fails to execute, it returns NULL and a 
 * meaningful error message stored into 'error_msg'. 
 *
 * Memory issues: after using the command pipeline linked list it is necessary 
 * to call P_delete_command_pipeline_linked_list passing the address of the 
 * command pipeline linked list in order to free the allocated memory.
 *
 *
 * Input:   (struct L_token_array *) array_of_tokens -> array with tokens returned by
 * the lexer. 
 *          (char []) error_msg -> 'error_msg' is a char array with at least the 
 * size P_ERROR_MSG_SZ, otherwise the behaviour is undefined. It stores the 
 * error message, if there is any. It is not changed when no error happens.
 */
{
    struct P_command_pipeline_linked_list *command_pipeline_linked_list = NULL;

    command_pipeline_linked_list = malloc(sizeof *command_pipeline_linked_list);
    if(!command_pipeline_linked_list)
    {
        snprintf(error_msg, P_ERROR_MSG_SZ, "Memory allocation error at P_command_pipeline_linked_list (L. %d).", __LINE__);
        goto return_result;
    }
    command_pipeline_linked_list->head = NULL;
    command_pipeline_linked_list->tail = NULL;
    command_pipeline_linked_list->length = 0;

    //Start parsing process:

    if(!GR_start(array_of_tokens, command_pipeline_linked_list, error_msg)) P_delete_command_pipeline_linked_list(&command_pipeline_linked_list);

return_result:
    return command_pipeline_linked_list;

}

void P_delete_command_pipeline_linked_list(struct P_command_pipeline_linked_list **command_pipeline_linked_list_pp)
{
    *command_pipeline_linked_list_pp = NULL;
    //IMPLEMENTATION###################
    //#################################
}


