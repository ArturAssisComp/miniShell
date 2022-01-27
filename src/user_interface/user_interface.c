#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "user_interface.h"
#include "../project_configurations.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../command_processor/command_processor.h"
#include "../built_in_commands/built_in_commands.h"

//Macros
#define COMMAND_HISTORY_LOG_FILE "conf/command_history.log"
#define MAX_SIZE  1000 
#define MAX_WORDS 100


//Local function declaration:
static bool parse_str_in_txt(const char str[], const char txt[]);

//Function definitions:
void UI_init_shell(void)
{
    BIC_init_built_in_functions();
    CP_init_current_session_status();
}

void UI_finish_shell(void)
{
    BIC_finish_built_in_functions();
    CP_finish_current_session_status();
}

void UI_prompt_commands(void)
{
	struct L_token_array *my_array;
    struct P_command_pipeline_linked_list *my_pipeline_linked_list;
    struct P_command_pipeline_node *pipeline;
	int i;
	char line[MAX_SIZE + 1];
	char last_line[MAX_SIZE + 1];
	char error_msg[L_ERROR_MSG_SZ + P_ERROR_MSG_SZ] = "";
    FILE *history_command_file;

    //Open the history command file:
    history_command_file = fopen(PC_PROJECT_PATH COMMAND_HISTORY_LOG_FILE, "a+");
    if(!history_command_file)
    {
        perror("fopen");
        goto error;
    }


	printf("\nminiSh\n");
    printf(">>> ");
    last_line[0] = '\0';
	while(CP_shell_is_interactive() && fgets(line, MAX_SIZE, stdin))
	{
        error_msg[0] = '\0';

        //Check for history commands:
        if(parse_str_in_txt("!!", line))
        {
            if(last_line[0] == '\0') 
            {
                printf("No commands in history.");
                my_array = NULL;
            }
            else 
            {
                printf("Last command: %s", last_line);
                my_array = L_read_tokens(last_line, error_msg);
            }
        }
        else my_array = L_read_tokens(line, error_msg);

		if(!my_array && error_msg[0] != '\0')
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

        //Save the command to command history:
        fprintf(history_command_file, "%s", line);
        if(fflush(history_command_file) == EOF)
        {
            perror("fflush");
            goto error;
        }

        snprintf(last_line, sizeof last_line, "%s", line);


        if(CP_shell_is_interactive()) printf(">>> ");
	}

    if(fclose(history_command_file) == EOF)
    {
        perror("fclose");
        goto error;
    }


    return;

error:
    exit(EXIT_FAILURE);
}

//Local function definition:
static bool parse_str_in_txt(const char str[], const char txt[])
/**
 * Description: This function parses the 'str' into the 'txt'. It ignores spaces
 * before or after the 'str'. If the string 'str' is found, it returns true,
 * otherwise, false.
 *
 * Input: Both 'str' and 'txt' are '\0' terminated strings. Otherwise, behaviour
 * is undefined.
 */
{
    size_t i = 0;
    size_t txt_len = strlen(txt);
    size_t str_len = strlen(str);

    while(i < txt_len && isspace(txt[i])) i++;
    if(txt_len - i >= str_len && !memcmp(txt + i, str, str_len)) 
    {
        i += str_len;
        while(i < txt_len && isspace(txt[i])) i++;
        if(txt[i] == '\0') return true;
    }
    return false;
}

