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
    struct P_command_pipeline_linked_list *tmp_linked_list = *command_pipeline_linked_list_pp;
    struct P_command_pipeline_node *tmp_node;
    struct P_command *tmp_command;
    size_t i;

    if(tmp_linked_list)
    {
        while(tmp_linked_list->head)
        {
            tmp_node = tmp_linked_list->head;
            tmp_linked_list->head = tmp_linked_list->head->next_pipeline;

            //Free the selected node
            while(tmp_node->first_command)
            {
                tmp_command = tmp_node->first_command;
                tmp_node->first_command = tmp_node->first_command->next_pipelined_command;

                //Free the selected command:
                if(tmp_command->id) free(tmp_command->id);
                if(tmp_command->input_redirection_id) free(tmp_command->input_redirection_id);
                for(i = 0; i < tmp_command->num_of_args; i++)
                {
                    if(tmp_command->args_list[i]) free(tmp_command->args_list[i]);
                }
                for(i = 0; i < tmp_command->num_of_output_redirection_ids; i++)
                {
                    if(tmp_command->output_redirection_ids_list[i]) free(tmp_command->output_redirection_ids_list[i]);
                }
                free(tmp_command);
                tmp_command = NULL;
            }
            free(tmp_node);
            tmp_node = NULL;
        }
        free(tmp_linked_list);
    }
    *command_pipeline_linked_list_pp = NULL;
}


