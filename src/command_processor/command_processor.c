#include "../parser/parser.h"
#include "command_processor.h"
#include "../aux/shared_alloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/prctl.h>
#include <ctype.h>

#define STD_PATH "/"
#define COMMAND_SEARCH_CONF_FILE_PATH "../../conf/command_search_order.conf"
#define INPUT_REDIRECTION_CONF_FILE_PATH "../../conf/input_redirection_order.conf"
#define OUTPUT_REDIRECTION_CONF_FILE_PATH "../../conf/output_redirection_order.conf"
#define READ_END 0
#define WRITE_END 1

//Types:
/*There are three kinds of defaults for search order related to id searching.*/
enum default_search_order_flag 
{
    COMMAND_ID_DEFAULT,
    INPUT_REDIRECTION_DEFAULT,
    OUTPUT_REDIRECTION_DEFAULT
};

//Global variables:
static struct CP_status current_session_status = {NULL, NULL, NULL};
extern char **environ;

//Local function declaration:
static bool execute_pipeline(struct P_command_pipeline_node *pipeline);
static void execute_command(struct P_command *cmd, size_t *counter_addr);
static char **build_argv(struct P_command *cmd);
static struct CP_search_order_node *read_search_order_from_file(char filename[], enum default_search_order_flag flag);
static struct CP_search_order_node *create_CP_search_order_node(enum CP_search_type type, char folder[]);
static bool get_next_search_rule(char *line, enum CP_search_type *search_type_p, char folder[]);

//Function definitions:
bool CP_execute_commands(struct P_command_pipeline_linked_list *cmd_pipeline_list)
/**
 * Description: This function executes every command of every command pipeline 
 * from 'cmd_pipeline_list'. If every command pipeline is either executed or has
 * pipeline->execute_in_background true,
 * this function returns true, otherwise, it returns false.
 *
 * Error Handling: If an error happens during the execution of a command, an
 * error message and its return status (if any) are stored into 'cmd_pipeline_list'.
 * If input is NULL, this function exits with failure status.
 *
 * Memory Issues: It is important that the 'cmd_pipeline_list' was allocated
 * using SA_malloc or SA_calloc, otherwise, its memory location will not 
 * be shared with its children and the data stored by this structure will
 * not be properly updated.
 */
{
    bool result;
    struct P_command_pipeline_node *tmp_pipeline;
    size_t i;

    if(!cmd_pipeline_list) 
    {
        fprintf(stderr, "Value Error(L. %d): input of CP_execute_commands() may not be NULL.", __LINE__);
        goto error;
    }


    i = 0;
    result = true;
    tmp_pipeline  = cmd_pipeline_list->head;
    while(i++ < cmd_pipeline_list->length)
    {
        if(!tmp_pipeline)
        {
            fprintf(stderr, "Value Error(L. %d): input of CP_execute_commands() is corrupted.", __LINE__);
            goto error;
        }

        //Execute the current_pipeline:
        if(!execute_pipeline(tmp_pipeline)) result = false;

        tmp_pipeline = tmp_pipeline->next_pipeline;
    }


return_result:
    return result;
error:
    exit(EXIT_FAILURE);
}


void CP_init_current_session_status(char *init_working_directory)
{
    //Get the command search order configuration:
    current_session_status.first_node_for_command_id = read_search_order_from_file(COMMAND_SEARCH_CONF_FILE_PATH, COMMAND_ID_DEFAULT);

    //Get the command search order configuration:
    current_session_status.first_node_for_in_redirect_id= read_search_order_from_file(INPUT_REDIRECTION_CONF_FILE_PATH, INPUT_REDIRECTION_DEFAULT);

    //Get the command search order configuration:
    current_session_status.first_node_for_out_redirect_id = read_search_order_from_file(OUTPUT_REDIRECTION_CONF_FILE_PATH, OUTPUT_REDIRECTION_DEFAULT);
}

void CP_finish_current_session_status(void)
{
    struct CP_search_order_node *tmp;

    //Free allocated memory:
    tmp = current_session_status.first_node_for_command_id;
    while(tmp)
    {
        current_session_status.first_node_for_command_id = tmp->next_node;
        free(tmp);
        tmp = current_session_status.first_node_for_command_id;
    }

    tmp = current_session_status.first_node_for_in_redirect_id;
    while(tmp)
    {
        current_session_status.first_node_for_in_redirect_id = tmp->next_node;
        free(tmp);
        tmp = current_session_status.first_node_for_in_redirect_id;
    }

    tmp = current_session_status.first_node_for_out_redirect_id;
    while(tmp)
    {
        current_session_status.first_node_for_out_redirect_id = tmp->next_node;
        free(tmp);
        tmp = current_session_status.first_node_for_out_redirect_id;
    }
}


//Define local functions:
static bool execute_pipeline(struct P_command_pipeline_node *pipeline)
{
    struct P_command *tmp_command;
    pid_t pid, desc_pid;
    bool execute_in_background = pipeline->execute_in_background;
    bool result;
    size_t *commands_executed_p = SA_malloc(sizeof *commands_executed_p);

    tmp_command = pipeline->first_command;

    
    //Set child subreaper:
    if(prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0) == -1)
    {
        perror("prctl");
        goto error;
    }

    pid = fork(); 

    if(pid == 0) //Child
    {
        execute_command(tmp_command, commands_executed_p); 
    }
    else if (pid > 0) //Parent
    {
        if(!execute_in_background) 
        {
            while(desc_pid = wait(NULL) > 0);
            if(desc_pid == -1 && errno != ECHILD)
            {
                perror("wait");
                goto error;
            }
                
            pipeline->remaining_to_execute -= *commands_executed_p;
            if(!pipeline->remaining_to_execute) 
            {
                result = true;
                pipeline->was_executed = true;
            }
            else result = false;
        }
        else result = true;

    }
    else //Error
    {
        perror("fork");
        goto error;
    }

    SA_free(commands_executed_p);
return_result:
    return result;
error:
    exit(EXIT_FAILURE);
}


static void execute_command(struct P_command *cmd, size_t *counter_addr)
{
    pid_t pid;
    size_t result;
    size_t i;
    char **envp = environ;
    char **argv;
    int pipe_fd[2];


    //Update the executed counter:
    (*counter_addr)++;

    //Generate the output redirection processess:
    //...

    //Create the pipe:
    if(pipe(pipe_fd) == -1)
    {
        perror("pipe");
        goto error;
    }

    //Save the current process pid:
    cmd->pid = getpid();
    
    if(cmd->next_pipelined_command)
    {
        pid = fork(); 
        if(pid == 0) //Child
        {
            //Close write end of pipe
            if(close(pipe_fd[WRITE_END]) == -1)
            {
                perror("close");
                goto error;
            }

            if(dup2(pipe_fd[READ_END], STDIN_FILENO) == -1)
            {
                perror("dup2");
                goto error;
            }
            execute_command(cmd->next_pipelined_command, counter_addr); 
        }
        else if (pid > 0) //Parent
        {
            //Close read end of pipe
            if(close(pipe_fd[READ_END]) == -1)
            {
                perror("close");
                goto error;
            }

            if(dup2(pipe_fd[WRITE_END], STDOUT_FILENO) == -1)
            {
                perror("dup2");
                goto error;
            }
        }
        else //Error
        {
            perror("fork");
            goto error;
        }
    }

    cmd->was_executed = true;

    //Prepare to call execve:
    argv = build_argv(cmd);
    if(execve(argv[0], argv, envp) == -1) perror("execve");

    for(i = 0; i < cmd->num_of_args + 1; i++) if(argv[i]) free(argv[i]);
    free(argv);

error:
    exit(EXIT_FAILURE);

}

static char **build_argv(struct P_command *cmd)
/**
 * This function allocates a NULL terminated array of strings. The
 * first element is the filename argument for execve(2) and the array
 * itself is the second argument for execve(2).
 *
 * Memory issues: After using this function, the user must deallocate
 * the allocated element returned.
 */
{
    char **argv;
    size_t i;
    size_t length;
    char *filename;
    argv = calloc(cmd->num_of_args + 2, sizeof *argv);
    //filename = find_filename(cmd->id, COMMAND_ID);
    //argv[0] = filename;

    //Provisory
    length = strlen(cmd->id);
    argv[0] = calloc(length + 1, sizeof **argv);
    strncpy(argv[0], cmd->id, length + 1);
    //End provisory
    for(i = 0; i < cmd->num_of_args; i++)
    {
        length = strlen(cmd->args_list[i]);
        argv[i + 1] = calloc(length + 1, sizeof **argv);
        strncpy(argv[i + 1], cmd->args_list[i], length + 1);
    }
    argv[i + 1] = NULL;

    return argv;
}


static struct CP_search_order_node *read_search_order_from_file(char filename[], enum default_search_order_flag flag)
/**
 * Description: This function checks if there is any file configuration with 
 * name 'filename' to define the order in which the id of a 
 * command/input_redirection/output_redirection will be searched. If 
 * the file is empty or if there is no file, the default order is specified 
 * by the 'flag'.
 *
 * Input:
 *
 *      (enum default_search_order_flag) flag -> Specify the kind of default 
 *      that will be returned, if necessary. May be one of the following:
 *          - COMMAND_ID_DEFAULT: 
 *                      (1) - <built_in>; 
 *                      (2) - <default>; 
 *                      (3) - <current_working_directory>; 
 *          - INPUT_REDIRECTION_DEFAULT
 *                      (1) - <built_in>; 
 *                      (2) - <default>; 
 *                      (3) - <current_working_directory>; 
 *          - OUTPUT_REDIRECTION_DEFAULT
 *                      (1) - <built_in>; 
 *                      (2) - <default>; 
 *                      (3) - <current_working_directory>; 
 *
 * Error Handle: If either the file exists but may not be opened, any memory
 * allocation error happens, or the file is corrupted, this function exits and 
 * prints the error msg to stderr.
 *
 * Memory Issues: After using the returned value from this function, the user
 * must free it if non-NULL value was returned.
 */
{
    enum CP_search_type tmp_search_type;
    struct CP_search_order_node *result_node = NULL;
    struct CP_search_order_node **tmp_node_p = &result_node;
    FILE *file_ptr = NULL;
    char *line = NULL;
    char folder[CP_MAX_PATH_SZ];
    size_t n = 0;
    bool choose_default = false;

    if(!access(filename, F_OK)) //There is a file to parse
    {
        //Open the file:
        if(file_ptr = fopen(filename, "r"))
        {
            while(getline(&line, &n, file_ptr) != -1)
            {
                //Process the current line:
                if(get_next_search_rule(line, &tmp_search_type, folder))
                {
                    //Create next node:
                    *tmp_node_p = create_CP_search_order_node(tmp_search_type, folder);
                    tmp_node_p = &((*tmp_node_p)->next_node);
                }
                else //Invalid line
                {
                    fprintf(stderr, "Error(f.%s, l. %d): corrupted file %s.\n", __FILE__, __LINE__, filename);
                    exit(EXIT_FAILURE);
                }

                free(line);
                line = NULL;
                n = 0;
            }
            free(line);
            if(fclose(file_ptr))
            {
                perror("fclose");
                exit(EXIT_FAILURE);
            }

            if(!result_node) //File is empty
                choose_default = true;

        }
        else //Error
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }
    else //Get the desired default:
        choose_default = true;

    if(choose_default) //file 'filename' is empty or does not exist.
    {
        switch(flag)
        {
            case COMMAND_ID_DEFAULT:
            case INPUT_REDIRECTION_DEFAULT:
            case OUTPUT_REDIRECTION_DEFAULT:
                //The order of search is:
                //(1) - <built_in>
                //(2) - <default>
                //(3) - <current_working_directory>

                //Create node <built_in>:
                *tmp_node_p = create_CP_search_order_node(CP_BUILT_IN, "");
                tmp_node_p = &((*tmp_node_p)->next_node);

                //Create node <default>:
                *tmp_node_p = create_CP_search_order_node(CP_DEFAULT, "");
                tmp_node_p = &((*tmp_node_p)->next_node);

                //Create node <current_working_directory>:
                *tmp_node_p = create_CP_search_order_node(CP_CURRENT_WORKING_DIRECTORY, "");
                tmp_node_p = &((*tmp_node_p)->next_node);

                break;
            default:
                fprintf(stderr, "Error(f. %s, l. %d): Invalid flag.", __FILE__, __LINE__);
                exit(EXIT_FAILURE);
        }
    }

    return result_node;
}


static struct CP_search_order_node *create_CP_search_order_node(enum CP_search_type type, char folder[])
{
    struct CP_search_order_node *new_node;
    //Create next node:
    new_node = malloc(sizeof *new_node);
    if(!new_node)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    //Initialize the node just created:
    new_node->type = type;
    strncpy(new_node->folder, folder, CP_MAX_PATH_SZ);
    new_node->next_node = NULL;

    return new_node;
}
    

static bool get_next_search_rule(char *line, enum CP_search_type *search_type_p, char folder[])
/**
 * Description: This function parses the string 'line' and returns true if there is
 * a correct rule in the line. The type is stored into *search_type_p and the value,
 * if there is one, is stored into 'folder', overwriting its content. Otherwise,
 * it returns false.
 *
 */
{
    size_t i, initial_index, final_index;
    const size_t path_offset = sizeof "path:" - 1;
    char *tmp;
    //Implement
    initial_index = 0;
    while(isblank(line[initial_index])) initial_index++; /*Consume blank*/
    if(line[initial_index++] != '<') return false; /*Consume '<'*/
    final_index = initial_index;
    while(line[final_index] != '>' && line[final_index] != '\0') final_index++; /*Consume the content <...>*/
    i = final_index;
    if(i != '>') return false;
    while(isblank(line[i])) i++; /*Consume blank*/
    if(line[i] == '/' && line[i + 1] == '/') 
        while(line[i] != '\0') i++; /*Consume comments*/
    if(line[i] != '\0') return false; 


    tmp = calloc(final_index - initial_index + 1, sizeof *tmp);
    if(!tmp)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    memcpy(tmp, line + initial_index, final_index - initial_index);
    tmp[final_index - initial_index] = '\0';

    //Check for each possible rule:
    if(!strcmp(tmp, "built_in"))
    {
        *search_type_p = CP_BUILT_IN;
        folder[0] = '\0';
    }
    else if (!strcmp(tmp, "default"))
    {
        *search_type_p = CP_DEFAULT;
        folder[0] = '\0';
    }
    else if (!strcmp(tmp, "current_working_directory"))
    {
        *search_type_p = CP_CURRENT_WORKING_DIRECTORY;
        folder[0] = '\0';
    }
    else if (!strncmp(tmp, "path:", path_offset))
    {
        //Check if the size of folder supports the value of the path:
        if(CP_MAX_PATH_SZ < final_index - initial_index + 1) return false;

        *search_type_p = CP_PATH;
        memcpy(folder, tmp + path_offset, strlen(tmp) - path_offset);
    }
    else 
    {
        free(tmp);
        return false;
    }

    free(tmp);
    return true;
}

