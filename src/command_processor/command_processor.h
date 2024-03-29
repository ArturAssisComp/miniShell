#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include "../parser/parser.h"
#include <stdbool.h>

#define CP_MAX_PATH_SZ 4096

enum CP_search_type
{
    CP_BUILT_IN,
    CP_DEFAULT,
    CP_CURRENT_WORKING_DIRECTORY,
    CP_PATH
};

struct CP_search_order_node
{
    enum CP_search_type type;
    char folder[CP_MAX_PATH_SZ]; //Semantic value in case type == CP_PATH
    struct CP_search_order_node *next_node;

};

struct CP_status
{
    struct CP_search_order_node *first_node_for_command_id;
    struct CP_search_order_node *first_node_for_out_redirect_id;
    struct CP_search_order_node *first_node_for_in_redirect_id;
    bool shell_is_interactive;
};



bool CP_execute_commands(struct P_command_pipeline_linked_list *cmd_pipeline_list);
void CP_init_current_session_status(void);
void CP_finish_current_session_status(void);
void CP_deactivate_shell(void);
bool CP_shell_is_interactive(void);

#endif
