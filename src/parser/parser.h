#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include "../lexer/lexer.h"

#define P_ERROR_MSG_SZ 2048
#define P_MAX_ARGS 512
#define P_MAX_OUTPUT_REDIRECTION_IDS 512

struct P_command
{
    char *id;
    char *args_list[P_MAX_ARGS + 1];
    char *input_redirection_id;
    char *output_redirection_ids_list[P_MAX_OUTPUT_REDIRECTION_IDS + 1];
    struct P_command *next_pipelined_command;
    bool was_executed;
    size_t num_of_args;
    size_t num_of_output_redirection_ids;
    pid_t pid;
};

struct P_command_pipeline_node
{
    struct P_command *first_command;
    struct P_command *last_command;
    size_t length; //The number of commands in this pipeline.
    size_t remaining_to_execute;
    bool was_executed;
    bool execute_in_background;
    struct P_command_pipeline_node *next_pipeline;
};

struct P_command_pipeline_linked_list
{
    struct P_command_pipeline_node *head; //Points to the first pipeline
    struct P_command_pipeline_node *tail; //Points to the last pipeline
    size_t length; //The number of pipelines of commands.
};


//Function declarations:
struct P_command_pipeline_linked_list *P_parse(struct L_token_array *array_of_tokens, char error_msg[P_ERROR_MSG_SZ]);
void P_delete_command_pipeline_linked_list(struct P_command_pipeline_linked_list **command_pipeline_linked_list_pp);

#endif
