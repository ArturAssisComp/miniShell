#include "built_in_commands.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../aux/types.h"
#include "../aux/dict.h"


#define NUM_OF_BUILT_IN_COMMANDS 2

//Types:
typedef int (*built_in_function) (char *const [], char *const []);


//Global variables:
static T_dict *built_in_functions_dictionary = NULL;

//Declaration of static functions:
static bool __is_built_in_command(const char id[]);

//Declaration of built in functions:
static int BUILT_IN_cd(char *const argv[], char *const envp[]);
static int BUILT_IN_exit(char *const argv[], char *const envp[]);





//Definition of functions:
bool BIC_is_built_in_command(const char id[])
{
    //Add prefix BUILT_IN_:
    char full_id[strlen("BUILT_IN_") + strlen(id) + 1];
    sprintf(full_id, "%s%s", "BUILT_IN_", id);
    return __is_built_in_command(full_id);
}


int BIC_exec_built_in_cmd(const char *built_in_cmd_id, char *const argv[], char *const envp[])
{
    T_element *tmp = NULL;
    tmp = D_get_value((T_element){{.str=(char *)built_in_cmd_id}, STRING}, built_in_functions_dictionary);
    return (*(built_in_function)(tmp->value.void_p))(argv, envp);

}

//Definition of static functions:
static bool __is_built_in_command(const char id[])
{
    if(D_get_value((T_element){{.str=(char *)id}, STRING}, built_in_functions_dictionary)) return true;
    else return false;
}

void BIC_init_built_in_functions(void)
{
    built_in_functions_dictionary = D_create_dict(NUM_OF_BUILT_IN_COMMANDS/2 + 3);
    built_in_function current_built_in_function = NULL;

    //Add cd command:
    current_built_in_function = &BUILT_IN_cd;
    D_assign_value_to_key((T_element){{.str="BUILT_IN_cd"}, STRING}, (T_element){{.void_p=(void*)current_built_in_function}, VOID_POINTER}, built_in_functions_dictionary);

    //Add exit command:
    current_built_in_function = &BUILT_IN_exit;
    D_assign_value_to_key((T_element){{.str="BUILT_IN_exit"}, STRING}, (T_element){{.void_p=(void*)current_built_in_function}, VOID_POINTER}, built_in_functions_dictionary);


}

void BIC_finish_built_in_functions(void)
{
    D_delete_dict(&built_in_functions_dictionary);
}

//Definitions for builtin functions:
static int BUILT_IN_cd(char *const argv[], char *const envp[])
{
    char *tmp;
    if(argv[1]) 
    {
        tmp = getcwd(NULL, 0); 
        setenv("OLDPWD", tmp, true);
        free(tmp);
        chdir(argv[1]);
        tmp = getcwd(NULL, 0); 
        setenv("PWD", tmp, true);
        free(tmp);
    }
    return 0;
}

static int BUILT_IN_exit(char *const argv[], char *const envp[])
{
    exit(EXIT_SUCCESS);
}
