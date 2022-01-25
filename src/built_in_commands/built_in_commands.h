#ifndef BUILT_IN_COMMANDS_H
#define BUILT_IN_COMMANDS_H
#include <stdbool.h>

bool BIC_is_built_in_command(const char id[]);
int BIC_exec_built_in_cmd(const char *built_in_cmd_id, char *const argv[], char *const envp[]);
void BIC_init_built_in_functions(void);
void BIC_finish_built_in_functions(void);



#endif
