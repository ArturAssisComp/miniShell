/**
 * Author: Artur Assis Alves
 * Title: MiniShell - Main Function
 */

//Include:
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "command_processor/command_processor.h"


//Define:
#define MAX_SIZE  1000 
#define MAX_WORDS 100

char *get_token_type_str(enum L_token_type t);
void print_command(struct P_command *cmd, size_t n);
void print_pipeline(struct P_command_pipeline_node *pipeline, size_t n);

int main(){
	struct L_token_array *my_array;
    struct P_command_pipeline_linked_list *my_pipeline_linked_list;
    struct P_command_pipeline_node *pipeline;
	int i;
	char line[MAX_SIZE + 1];
	char error_msg[L_ERROR_MSG_SZ + P_ERROR_MSG_SZ] = "";

	//Initialize the shell:
    CP_init_current_session_status();
	printf("\nminiSh\n");
	printf(">>> ");
	while(fgets(line, MAX_SIZE, stdin))
	{
        error_msg[0] = '\n';
		my_array = L_read_tokens(line, error_msg);
		if(!my_array)
		{
			printf("%s\n", error_msg);
			error_msg[0] = '\0'; 
		}
		else
		{
			i = 0;
			while(my_array->array[i]->token_type != EOF_TOKEN) 
			{
				printf("--> Token %d: %s", i + 1, get_token_type_str(my_array->array[i]->token_type));
				if(my_array->array[i]->token_value) printf(" (V: %s)", my_array->array[i]->token_value);
				printf("\n");
				i++;
			}

            
            //Parse the commands:
            my_pipeline_linked_list = P_parse(my_array, error_msg);
            if(my_pipeline_linked_list == NULL)
            {
                printf("%s\n", error_msg);
                error_msg[0] = '\0'; 
            }
            else
            {
                pipeline = my_pipeline_linked_list->head;
                printf("Before executing the commands:\n");
                if(pipeline)
                {
                    i = 1;
                    while(pipeline)
                    {
                        print_pipeline(pipeline, i);

                        i++;
                        pipeline = pipeline->next_pipeline;
                    }
                }
                //Execute commands:
                CP_execute_commands(my_pipeline_linked_list);
                printf("After executing the commands:\n");
                pipeline = my_pipeline_linked_list->head;
                if(pipeline)
                {
                    i = 1;
                    while(pipeline)
                    {
                        print_pipeline(pipeline, i);

                        i++;
                        pipeline = pipeline->next_pipeline;
                    }
                }

                P_delete_command_pipeline_linked_list(&my_pipeline_linked_list);
            }
			L_delete_token_array(&my_array);
		}
		//Free the allocated memory:
		printf("\n>>> ");

	}

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
void print_pipeline(struct P_command_pipeline_node *pipeline, size_t n)
{
    size_t i;
    struct P_command *tmp_cmd;
    printf("-> Pipeline %d\n", n);
    printf("-> length:  %d\n", pipeline->length);
    printf("-> remaining_to_execute:  %d\n", pipeline->remaining_to_execute);
    printf("-> was_executed: \"%s\"\n", pipeline->was_executed?"true":"false");
    printf("-> execute_in_background: \"%s\"\n", pipeline->execute_in_background?"true":"false");
    printf("##Commands##\n");
    tmp_cmd = pipeline->first_command;
    i = 1;
    while(tmp_cmd)
    {
        print_command(tmp_cmd, i);

        i++;
        tmp_cmd = tmp_cmd->next_pipelined_command;
    }
    printf("############\n");


    printf("\n", n);
}

void print_command(struct P_command *cmd, size_t n)
{
    size_t i;
    printf("%d-th command\n", n);
    printf("    id: \"%s\"\n", cmd->id);
    printf("    num_of_args: %d\n", cmd->num_of_args);
    if(cmd->num_of_args > 0)
    {
        printf("    args: { %s", cmd->args_list[0]);
        for(i=1; i < cmd->num_of_args; i++) printf(", %s", cmd->args_list[i]);
        printf("}\n");
    }
    printf("    num_of_output_redirection_ids: %d\n", cmd->num_of_output_redirection_ids);
    if(cmd->num_of_output_redirection_ids > 0)
    {
        printf("    output_redirect_ids: { %s", cmd->output_redirection_ids_list[0]);
        for(i=1; i < cmd->num_of_output_redirection_ids; i++) printf(", %s", cmd->output_redirection_ids_list[i]);
        printf("}\n");
    }
    printf("    input_redirect_id: \"%s\"\n", cmd->input_redirection_id?cmd->input_redirection_id:"NULL");
    printf("    was_executed: \"%s\"\n", cmd->was_executed?"true":"false");
    printf("    pid: \"%d\"\n", cmd->pid);
}
