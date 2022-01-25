
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
#include "built_in_commands/built_in_commands.h"


//Define:
#define MAX_SIZE  1000 
#define MAX_WORDS 100


int main(){
	struct L_token_array *my_array;
    struct P_command_pipeline_linked_list *my_pipeline_linked_list;
    struct P_command_pipeline_node *pipeline;
	int i;
	char line[MAX_SIZE + 1];
	char error_msg[L_ERROR_MSG_SZ + P_ERROR_MSG_SZ] = "";

	//Initialize the shell:
    BIC_init_built_in_functions();
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
            //Parse the commands:
            my_pipeline_linked_list = P_parse(my_array, error_msg);
            if(my_pipeline_linked_list == NULL)
            {
                printf("%s\n", error_msg);
                error_msg[0] = '\0'; 
            }
            else
            {
                //Execute commands:
                CP_execute_commands(my_pipeline_linked_list);
                P_delete_command_pipeline_linked_list(&my_pipeline_linked_list);
            }
			L_delete_token_array(&my_array);
		}
		//Free the allocated memory:
		printf("\n>>> ");

	}
    BIC_finish_built_in_functions();
    CP_finish_current_session_status();

	return 0;
}
