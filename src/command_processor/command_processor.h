#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include "../parser/parser.h"
#include <stdbool.h>

#define CP_MAX_PATH_SZ 4096

enum CP_search_type
{
    CP_FULL_PATH,
    CP_REL_PATH,
    CP_BUILT_IN,
    CP_PATH
};

struct search_order_node
{
    enum CP_search_type type;
    char folder[CP_MAX_PATH_SZ]; //Semantic value in case type == CP_PATH
    struct search_order_node *next_node;

};

struct CP_status
{
    char current_working_directory[CP_MAX_PATH_SZ + 1];
    struct search_order_node *first_node_for_command_id;
    struct search_order_node *first_node_for_out_redirect_id;
    struct search_order_node *first_node_for_in_redirect_id;
};



bool CP_execute_commands(struct P_command_pipeline_linked_list *cmd_pipeline_list);
void CP_init_current_session_status(char *init_working_directory);
void CP_finish_current_session_status(void);

#endif
