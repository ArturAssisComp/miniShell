#include "../parser/parser.h"
#include "command_processor.h"
#include "../aux/shared_alloc.h"
#include "../built_in_commands/built_in_commands.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/prctl.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>

#define PROJECT_PATH "/home/artur/software_system_development/miniShell/"
#define TMP_FILE_PATH "/tmp/"
#define MAX_FILENAME_LENGTH 512
#define COMMAND_SEARCH_CONF_FILE_PATH "conf/command_search_order.conf"
#define INPUT_REDIRECTION_CONF_FILE_PATH "conf/input_redirection_order.conf"
#define OUTPUT_REDIRECTION_CONF_FILE_PATH "conf/output_redirection_order.conf"
#define READ_END 0
#define WRITE_END 1
#define MAX_BUFFER 4096

//Types:
/*There are three kinds of defaults for search order related to id searching.*/
enum default_search_order_flag 
{
    COMMAND_ID_DEFAULT,
    INPUT_REDIRECTION_DEFAULT,
    OUTPUT_REDIRECTION_DEFAULT
};

enum search_type_flag
{
    COMMAND_ID,
    INPUT_REDIRECTION_ID,
    OUTPUT_REDIRECTION_ID  
};

//Global variables:
static struct CP_status current_session_status = {NULL, NULL, NULL, true};
extern char **environ;

//Local function declaration:
static bool execute_pipeline(struct P_command_pipeline_node *pipeline);
static void execute_command(struct P_command *cmd, size_t *counter_addr, const char *tmp_path_filename);
static void __execute_command(const struct P_command *cmd, const size_t commands_executed);
static char **build_argv(const struct P_command *cmd);
static struct CP_search_order_node *read_search_order_from_file(char filename[], enum default_search_order_flag flag);
static struct CP_search_order_node *create_CP_search_order_node(enum CP_search_type type, char folder[]);
static bool get_next_search_rule(char *line, enum CP_search_type *search_type_p, char folder[CP_MAX_PATH_SZ]);
static char *find_meaningful_full_name(const char *id, enum search_type_flag flag);
static char *concatenate_str(const char *prefix, const char *infix, const char *suffix);
static bool is_built_in_stream(const char *name);
static char *get_base_dir(const char *filename);

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


void CP_init_current_session_status()
{
    //Get the command search order configuration:
    current_session_status.first_node_for_command_id = read_search_order_from_file(PROJECT_PATH COMMAND_SEARCH_CONF_FILE_PATH, COMMAND_ID_DEFAULT);

    //Get the command search order configuration:
    current_session_status.first_node_for_in_redirect_id= read_search_order_from_file(PROJECT_PATH INPUT_REDIRECTION_CONF_FILE_PATH, INPUT_REDIRECTION_DEFAULT);

    //Get the command search order configuration:
    current_session_status.first_node_for_out_redirect_id = read_search_order_from_file(PROJECT_PATH OUTPUT_REDIRECTION_CONF_FILE_PATH, OUTPUT_REDIRECTION_DEFAULT);
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

void CP_deactivate_shell(void)
{
    current_session_status.shell_is_interactive = false;
}

bool CP_shell_is_interactive(void)
{
    return current_session_status.shell_is_interactive;
}

//Define local functions:
static bool execute_pipeline(struct P_command_pipeline_node *pipeline)
{
    struct P_command *tmp_command;
    pid_t pid, desc_pid;
    bool execute_in_background = pipeline->execute_in_background;
    bool first_cmd_is_built_in;
    bool result;
    size_t *commands_executed_p = SA_malloc(sizeof *commands_executed_p); //This variable must be allocated using shared memory between parent process and its children.
    char temp_filename[MAX_FILENAME_LENGTH];
    char **envp = environ;
    char **argv;
    size_t i;
    int pipe_fd[2];

    tmp_command = pipeline->first_command;

    
    //Set child subreaper:
    if(prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0) == -1)
    {
        perror("prctl");
        goto error;
    }

    //Create the name of the temporary file to store content from streams that will be
    //used multiple times:
    snprintf(temp_filename, MAX_FILENAME_LENGTH, "%sminiSh_stream_content_storage_pid_%d", TMP_FILE_PATH, getpid());


    //Check if the first command is built in:
    argv = build_argv(tmp_command);
    if(!argv[0])
    {
        fprintf(stderr, "Semantic error: command \"%s\" was not found.\n", tmp_command->id); //Semantic error
        goto error;
    }

    first_cmd_is_built_in = !strncmp(argv[0], "BUILT_IN_", strlen("BUILT_IN_"));

    if(first_cmd_is_built_in)
    {
        //Update the current command:
        tmp_command = tmp_command->next_pipelined_command;

        if(tmp_command) //There is a next pipelined command
        {
            //Create the pipe:
            if(pipe(pipe_fd) == -1)
            {
                perror("pipe");
                goto error;
            }
        }
    }

    if(tmp_command) pid = fork(); 
    else pid = 1; //Avoid creating the child process

    if(pid == 0) //Child
    {
        if(first_cmd_is_built_in)
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
        }
        execute_command(tmp_command, commands_executed_p, temp_filename); 
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0) //Parent
    {
        if(first_cmd_is_built_in)
        {
            if(tmp_command) //There is a next pipelined command
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

            //Execute the built-in command:
            if(BIC_exec_built_in_cmd(argv[0], argv, envp) == -1) goto error;


            if(tmp_command) //There is a next pipelined command
            {
                //Close write end of pipe
                if(close(pipe_fd[WRITE_END]) == -1)
                {
                    perror("close");
                    goto error;
                }
            }
        }

        if(execute_in_background) result = true;
        else 
        {
            while((desc_pid = wait(NULL)) > 0);
            if(desc_pid == -1 && errno != ECHILD)
            {
                perror("wait");
                goto error;
            }

            //Unlink temp_file:
            if(unlink(temp_filename) == -1 && errno != ENOENT)
            {
                perror("unlink");
                goto error;
            }
                
            if(first_cmd_is_built_in) (*commands_executed_p)++; //Count with the built in command executed.
            pipeline->remaining_to_execute -= *commands_executed_p;
            if(!pipeline->remaining_to_execute) 
            {
                result = true;
                pipeline->was_executed = true;
            }
            else result = false;
        }

    }
    else //Error
    {
        perror("fork");
        goto error;
    }

    SA_free(commands_executed_p);

    for(i = 0; i < pipeline->first_command->num_of_args + 1; i++) if(argv[i]) free(argv[i]);
    free(argv);

return_result:
    return result;
error:
    exit(EXIT_FAILURE);
}


static void execute_command(struct P_command *cmd, size_t *counter_addr, const char *tmp_path_filename)
/**
 * Description: This function executes a pipeline of commands calling execve for
 * each or calling the built-in function for each. This function never return. 
 * Instead, it exits with EXIT_FAILURE or EXIT_SUCCESS status.
 *
 * Input:
 *      (size_t *) counter_addr -> This pointer points to the address of a
 *      variable that stores the number of commands executed. It must be
 *      allocated using SA_malloc.
 *
 */
{
    pid_t pid;
    size_t result;
    size_t i;
    size_t commands_executed = *counter_addr;
    ssize_t bytes_read, bytes_written;
    char *tmp_str = NULL;
    char bff[MAX_BUFFER];
    int pipe_fd[2];
    int tmp_fd;
    bool has_output_redirection = cmd->num_of_output_redirection_ids > 0;
    bool has_input_redirection = (bool) cmd->input_redirection_id;
    bool has_next_pipeline_command = (bool) cmd->next_pipelined_command;

    //Update the executed counter:
    (*counter_addr)++;

    //Save the current process pid:
    cmd->pid = getpid();

    //Add input redirection, if that exists:
    if(has_input_redirection)
    {
        tmp_str = find_meaningful_full_name(cmd->input_redirection_id, INPUT_REDIRECTION_ID);
        if(!tmp_str)
        {
            fprintf(stderr, "Semantic error: invalid input redirection id \"%s\" for command \"%s\".\n", cmd->input_redirection_id, cmd->id); //Semantic error
            goto error;
        }
        if((tmp_fd = open(tmp_str, O_RDONLY)) == -1)
        {
            perror("open");
            goto error;
        }

        if(dup2(tmp_fd, STDIN_FILENO) == -1)
        {
            perror("dup2");
            goto error;
        }
        free(tmp_str);
        tmp_str = NULL;
        tmp_fd = -1;
    }

    //Generate the output redirection processess:
    if(has_output_redirection)
    {
        //copy the content from stdin fd to a temp file:
        if(!has_input_redirection && (commands_executed > 0))
        {
            if((tmp_fd = open(tmp_path_filename, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU, S_IRWXG)) == -1)
            {
                perror("open");
                goto error;
            }
            do
            {
                if((bytes_read = read(STDIN_FILENO, bff, MAX_BUFFER)) == -1)
                {
                    perror("read");
                    goto error;
                }
                if((bytes_written = write(tmp_fd, bff, bytes_read)) == -1)
                {
                    perror("write");
                    goto error;
                }
                if(bytes_read != bytes_written)
                {
                    fprintf(stderr, "Error(f. %s, l. %zu): number of bytes read (%d) different from the number of bytes written (%d).\n", __FILE__, __LINE__, bytes_read, bytes_written);
                    goto error;
                }
            }while(bytes_read);


            if(dup2(tmp_fd, STDIN_FILENO) == -1)
            {
                perror("dup2");
                goto error;
            }
            tmp_fd = -1;
        }

        for(i = 0; i < cmd->num_of_output_redirection_ids; i++)
        {
            pid = fork(); 
            if(pid == 0) //Child
            {
                tmp_str = find_meaningful_full_name(cmd->output_redirection_ids_list[i], OUTPUT_REDIRECTION_ID);
                if(!tmp_str)
                {
                    fprintf(stderr, "Semantic error: invalid %zu-th output redirection id \"%s\" for command \"%s\".\n", i + 1, cmd->output_redirection_ids_list[i], cmd->id); //Semantic error
                    goto error;
                }
                if((tmp_fd = open(tmp_str, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU, S_IRWXG)) == -1)
                {
                    perror("open");
                    goto error;
                }

                if(dup2(tmp_fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    goto error;
                }
                free(tmp_str);
                tmp_str = NULL;
                tmp_fd = -1;
                __execute_command(cmd, commands_executed);
            }
            else if (pid > 0) //Parent
            {
                if(waitpid(pid, NULL, 0) == -1)
                {
                    perror("waitpid");
                    goto error;
                }
            }
            else //Error
            {
                perror("fork");
                goto error;
            }
        }
    }

    if(has_next_pipeline_command)
    {
        //Create the pipe:
        if(pipe(pipe_fd) == -1)
        {
            perror("pipe");
            goto error;
        }

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
            execute_command(cmd->next_pipelined_command, counter_addr, tmp_path_filename); 
            goto exit_success;
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

        cmd->was_executed = true;
        __execute_command(cmd, commands_executed);
    }

    if(!has_output_redirection && !has_next_pipeline_command)
    {
        cmd->was_executed = true;
        __execute_command(cmd, commands_executed);
    }

exit_success:
    exit(EXIT_SUCCESS);

error:
    exit(EXIT_FAILURE);
}

static void __execute_command(const struct P_command *cmd, const size_t commands_executed)
/**
 * This function never returns. It executes only the current command 'cmd'.
 */
{
    bool has_output_redirection = cmd->num_of_output_redirection_ids > 0;
    bool has_input_redirection = (bool) cmd->input_redirection_id;
    char **envp = environ;
    char **argv;
    size_t i;

    //Prepare to call execve or to call a built in procedure:
    argv = build_argv(cmd);
    if(!argv[0])
    {
        fprintf(stderr, "Semantic error: command \"%s\" was not found.\n", cmd->id); //Semantic error
        goto error;
    }

    if(has_input_redirection || (has_output_redirection && (commands_executed > 0)))
    {
            if(lseek(STDIN_FILENO, 0, SEEK_SET) == -1)
            {
                perror("lseek");
                goto error;
            }
    }

    //check if the command is built-in:
    if(!strncmp(argv[0], "BUILT_IN_", strlen("BUILT_IN_")))
    {
        if(BIC_exec_built_in_cmd(argv[0], argv, envp) == -1) 
        {
            for(i = 0; i < cmd->num_of_args + 1; i++) if(argv[i]) free(argv[i]);
            free(argv);
            goto error;
        }
        exit(EXIT_SUCCESS);//Make sure the built in command will exit.
    }

    //Execute a file:
    if(execve(argv[0], argv, envp) == -1) 
    {
        perror("execve");
        for(i = 0; i < cmd->num_of_args + 1; i++) if(argv[i]) free(argv[i]);
        free(argv);
        goto error;
    }

error:
    exit(EXIT_FAILURE); 
}

static char **build_argv(const struct P_command *cmd)
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

    filename = find_meaningful_full_name(cmd->id, COMMAND_ID);
    argv[0] = filename;

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

                free(line);
                line = NULL;
                n = 0;
            }
            if(line) free(line);
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
    

static bool get_next_search_rule(char *line, enum CP_search_type *search_type_p, char folder[CP_MAX_PATH_SZ])
/**
 * Description: This function parses the string 'line' and returns true if there is
 * a correct rule in the line. The type is stored into *search_type_p and the value,
 * if there is one, is stored into 'folder', overwriting its content. Otherwise,
 * it returns false.
 *
 * Error handle: if the line is corrupted, this function exits.
 *
 */
{
    size_t i, initial_index, final_index;
    const size_t path_offset = sizeof "path:" - 1;
    char *tmp;

    //Consume blank space:
    initial_index = 0;
    while(isblank(line[initial_index])) initial_index++; /*Consume blank*/

    //Check for comment line:
    if(line[initial_index] == '/' && line[initial_index + 1] == '/') return false;

    //Start consuming the rule:
    if(line[initial_index++] != '<')  /*Consume '<'*/
    {
        fprintf(stderr, "Error(f. %s, l. %d): corrupted line: %s", __FILE__, __LINE__, line);
        exit(EXIT_FAILURE);
    }

    final_index = initial_index;
    while(line[final_index] != '>' && line[final_index] != '\0') final_index++; /*Consume the content <...>*/
    if(line[final_index] != '>') 
    {
        fprintf(stderr, "Error(f. %s, l. %d): corrupted line: %s", __FILE__, __LINE__, line);
        exit(EXIT_FAILURE);
    }

    i = final_index + 1;
    while(isspace(line[i])) i++; /*Consume blank or space*/
    if(line[i] == '/' && line[i + 1] == '/') 
        while(line[i] != '\0') i++; /*Consume comments*/

    if(line[i] != '\0') 
    {
        fprintf(stderr, "Error(f. %s, l. %d): corrupted line.", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }


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
        memcpy(folder, tmp + path_offset, strlen(tmp) - path_offset + 1);
    }
    else 
    {
        fprintf(stderr, "Error(f. %s, l. %d): corrupted line.", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    free(tmp);
    return true;
}



static char *find_meaningful_full_name(const char *id, enum search_type_flag flag)
/**
 * Description: This function searches for the correct prefix for id in order
 * to return a meaningful full name. This name may be a full path name of a file,
 * or a std built-in name for the miniShell. The function uses the global variable
 * current_session_satus as a reference to search for the prefix.
 * 
 *
 * Error Handling: This function exits if invalid flag is passed as argument or
 * if an error happens while allocating memory. 
 * It returns NULL if no valid id may be formed.
 *
 * Memory issues: After using the returned value, if it is not NULL, the caller
 * of this function must free this value.
 *
 * Input:
 *      (enum search_type_flag) flag -> Defines the kind of id that will be built.
 *      May be COMMAND_ID, INPUT_REDIRECTION_ID, or OUTPUT_REDIRECTION_ID.
 *      If the flag is COMMAND_ID, this function searches for the first valid command
 *      following the order specified by current_session_status' list for command ids.
 *      If no valid command is found, NULL is returned.
 *      If the flag is INPUT_REDIRECTION_ID, this function searches for the first valid
 *      filename/stream name available following the order specified by current_session_status' list
 *      for input redirection ids. Otherwise, it returns NULL.
 *      If the flag is OUTPUT_REDIRECTION_ID, this function searches for the first valid
 *      filename/stream name available following the order specified by current_session_status' list
 *      for output redirection ids. Otherwise, it returns the first possible filename that may be
 *      created.
 */
{
    char *meaningful_name = NULL;
    char *tmp_meaningful_name = NULL; 
    char *current_working_directory = NULL;
    char *tmp_str = NULL;
    struct CP_search_order_node *tmp_node = NULL;
    size_t n;
    DIR *tmp_dir;

    switch(flag)
    {
        case COMMAND_ID:
            tmp_node = current_session_status.first_node_for_command_id;
            while(tmp_node)
            {
                switch(tmp_node->type)
                {
                    case CP_BUILT_IN:
                        if(BIC_is_built_in_command(id))
                        {
                            meaningful_name = concatenate_str("BUILT_IN_", id, "");
                            goto return_result;
                        }
                        break;
                    case CP_DEFAULT:
                        if(id[0] == '.' && id[1] == '/' )
                        {
                            current_working_directory = getcwd(NULL, 0);
                            meaningful_name = concatenate_str(current_working_directory, "/", id);
                            free(current_working_directory);
                            
                            //Check if the file exists:
                            if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                            {
                                free(meaningful_name);
                                meaningful_name = NULL;
                            }
                            else goto return_result;
                        }
                        else if (id[0] == '/')
                        {
                            meaningful_name = concatenate_str("", id, "");
                            //Check if the file exists:
                            if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                            {
                                free(meaningful_name);
                                meaningful_name = NULL;
                            }
                            else goto return_result;
                        }
                        break;
                    case CP_CURRENT_WORKING_DIRECTORY:
                        current_working_directory = getcwd(NULL, 0);
                        meaningful_name = concatenate_str(current_working_directory, "/", id);
                        free(current_working_directory);
                        //Check if the file exists:
                        if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                        {
                            free(meaningful_name);
                            meaningful_name = NULL;
                        }
                        else goto return_result;
                        break;
                    case CP_PATH:
                        meaningful_name = concatenate_str(tmp_node->folder, "/", id);
                        //Check if the file exists:
                        if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                        {
                            free(meaningful_name);
                            meaningful_name = NULL;
                        }
                        else goto return_result;
                        break;
                }
                tmp_node = tmp_node->next_node;

            }


            break;
        case INPUT_REDIRECTION_ID:
            tmp_node = current_session_status.first_node_for_in_redirect_id;
            while(tmp_node)
            {
                switch(tmp_node->type)
                {
                    case CP_BUILT_IN:
                        if(is_built_in_stream(id))
                        {
                            meaningful_name = concatenate_str("STREAM_", id, "");
                            goto return_result;
                        }
                        break;
                    case CP_DEFAULT:
                        if(id[0] == '.' && id[1] == '/' )
                        {
                            current_working_directory = getcwd(NULL, 0);
                            meaningful_name = concatenate_str(current_working_directory, "/", id);
                            free(current_working_directory);
                            
                            //Check if the file exists:
                            if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                            {
                                free(meaningful_name);
                                meaningful_name = NULL;
                            }
                            else goto return_result;
                        }
                        else if (id[0] == '/')
                        {
                            meaningful_name = concatenate_str("", id, "");
                            //Check if the file exists:
                            if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                            {
                                free(meaningful_name);
                                meaningful_name = NULL;
                            }
                            else goto return_result;
                        }
                        break;
                    case CP_CURRENT_WORKING_DIRECTORY:
                        current_working_directory = getcwd(NULL, 0);
                        meaningful_name = concatenate_str(current_working_directory, "/", id);
                        free(current_working_directory);
                        
                        //Check if the file exists:
                        if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                        {
                            free(meaningful_name);
                            meaningful_name = NULL;
                        }
                        else goto return_result;
                        break;
                    case CP_PATH:
                        meaningful_name = concatenate_str(tmp_node->folder, "/", id);
                        //Check if the file exists:
                        if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                        {
                            free(meaningful_name);
                            meaningful_name = NULL;
                        }
                        else goto return_result;
                        break;
                }
                tmp_node = tmp_node->next_node;

            }
            break;
        case OUTPUT_REDIRECTION_ID:
            tmp_node = current_session_status.first_node_for_out_redirect_id;
            while(tmp_node)
            {
                switch(tmp_node->type)
                {
                    case CP_BUILT_IN:
                        if(is_built_in_stream(id))
                        {
                            meaningful_name = concatenate_str("STREAM_", id, "");
                            goto return_result;
                        }
                        break;
                    case CP_DEFAULT:
                        if(id[0] == '.' && id[1] == '/' )
                        {
                            current_working_directory = getcwd(NULL, 0);
                            meaningful_name = concatenate_str(current_working_directory, "/", id);
                            
                            //Check if the file exists:
                            if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                            {
                                //Check if dir is valid:
                                if((tmp_dir = opendir(current_working_directory)) && !tmp_meaningful_name) 
                                {
                                    tmp_meaningful_name = meaningful_name;
                                    if(closedir(tmp_dir) == -1)
                                    {
                                        perror("closedir");
                                        exit(EXIT_FAILURE);
                                    }
                                }
                                else free(meaningful_name);
                                meaningful_name = NULL;
                            }
                            else goto return_result;
                            free(current_working_directory);
                        }
                        else if (id[0] == '/')
                        {
                            meaningful_name = concatenate_str("", id, "");
                            //Check if the file exists:
                            if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                            {
                                tmp_str = get_base_dir(id);
                                //Check if dir is valid:
                                if((tmp_dir = opendir(tmp_str)) && !tmp_meaningful_name) 
                                {
                                    tmp_meaningful_name = meaningful_name;
                                    if(closedir(tmp_dir) == -1)
                                    {
                                        perror("closedir");
                                        exit(EXIT_FAILURE);
                                    }
                                }
                                else free(meaningful_name);
                                meaningful_name = NULL;
                            }
                            else goto return_result;
                            free(tmp_dir);
                        }
                        break;
                    case CP_CURRENT_WORKING_DIRECTORY:
                        current_working_directory = getcwd(NULL, 0);
                        meaningful_name = concatenate_str(current_working_directory, "/", id);
                        
                        //Check if the file exists:
                        if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                        {
                            //Check if dir is valid:
                            if((tmp_dir = opendir(current_working_directory)) && !tmp_meaningful_name) 
                            {
                                tmp_meaningful_name = meaningful_name;
                                if(closedir(tmp_dir) == -1)
                                {
                                    perror("closedir");
                                    exit(EXIT_FAILURE);
                                }
                            }
                            else free(meaningful_name);
                            meaningful_name = NULL;
                        }
                        else goto return_result;
                        free(current_working_directory);
                        break;
                    case CP_PATH:
                        meaningful_name = concatenate_str(tmp_node->folder, "/", id);
                        //Check if the file exists:
                        if(access(meaningful_name, F_OK) == -1) //File doeas not exist
                        {
                            //Check if dir is valid:
                            if((tmp_dir = opendir(tmp_node->folder)) && !tmp_meaningful_name) 
                            {
                                tmp_meaningful_name = meaningful_name;
                                if(closedir(tmp_dir) == -1)
                                {
                                    perror("closedir");
                                    exit(EXIT_FAILURE);
                                }
                            }
                            else free(meaningful_name);
                            meaningful_name = NULL;
                        }
                        else goto return_result;
                        break;
                }
                tmp_node = tmp_node->next_node;

            }
            if(tmp_meaningful_name && !meaningful_name) meaningful_name = tmp_meaningful_name;
            break;
        default:
            fprintf(stderr, "find_meaningful_full_name(f. %s, l. %d): invalid flag", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
            break;
    }

return_result:
    return meaningful_name;
}

static char *concatenate_str(const char *prefix, const char *infix, const char *suffix)
/**
 * Description: this function allocates memory for the following concatenation:
 * prefix + infix + suffix. All of the inputs must be strings '\0' terminated.
 * They may not be NULL. If the user wants to ignore any of the inputs, he/she
 * must use the empty string ("") as input.
 *
 * Memory issues: After using the returned string, the user must free it.
 *
 * Error handling: if any of the inputs is NULL or if a memory allocation
 * error occurs, this function prints an error msg to stderr and exits. If any
 * of the inputs is not '\0' terminated, the behaviour is undefined.
 */
{
    size_t bff_sz = strlen(prefix) + strlen(infix) + strlen(suffix) + 1;
    char *bff = NULL;
    bff = calloc(bff_sz, sizeof *bff);
    if(!bff)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    if(snprintf(bff, bff_sz, "%s%s%s", prefix, infix, suffix) < 0)
    {
        fprintf(stderr, "snprintf: fail to print to buffer.\n");
        exit(EXIT_FAILURE);
    }
    return bff;
}

static bool is_built_in_stream(const char *name)
{
    //Implement
    return false;
}

static char *get_base_dir(const char *filename)
/**
 * Description: returns the prefix of 'filename'
 * before the last '/'.
 */
{
    size_t last_index;
    size_t sz = 0;
    char *bff;
    int i;
    for(i = strlen(filename) - 1; i >= 0; i--)
    {
        if(filename[i] == '/') break;

    } 
    if(i >= 0) last_index = i;
    else last_index = 0;

    sz = last_index; //last_index - 0
    bff = calloc(sz + 1, sizeof *bff);
    if(!bff)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    memcpy(bff, filename, sz);
    bff[sz] = '\0';
    return bff;
}
