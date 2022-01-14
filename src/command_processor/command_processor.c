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

#define STD_PATH "/"
#define COMMAND_SEARCH_CONF_FILE_PATH "../../conf/command_search_order.conf"
#define INPUT_REDIRECTION_CONF_FILE_PATH "../../conf/input_redirection_order.conf"
#define OUTPUT_REDIRECTION_CONF_FILE_PATH "../../conf/output_redirection_order.conf"
#define READ_END 0
#define WRITE_END 1

//Global variables:
static struct CP_status current_session_status = {STD_PATH, NULL, NULL, NULL};
char **environ;

//Local function declaration:
static bool execute_pipeline(struct P_command_pipeline_node *pipeline);
static void execute_command(struct P_command *cmd, size_t *counter_addr);
static char **build_argv(struct P_command *cmd);

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
    //Implement the order of search for commands, in and out redirects
    // COMMAND_SEARCH_CONF_FILE_PATH 
    // INPUT_REDIRECTION_CONF_FILE_PATH 
    // OUTPUT_REDIRECTION_CONF_FILE_PATH 

    if(init_working_directory) strncpy(current_session_status.current_working_directory, init_working_directory, CP_MAX_PATH_SZ); 


    
}

void CP_finish_current_session_status(void)
{
    ;
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

