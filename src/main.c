
/**
 * Author: Artur Assis Alves
 * Title: MiniShell - Main Function
 */

//Include:
#include "user_interface/user_interface.h"



int main()
{
    UI_init_shell();
    UI_prompt_commands();
    UI_finish_shell();
	return 0;
}
