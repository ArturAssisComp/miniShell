#include <stdio.h>
#include "../components/manipulate_string.h"
#include "../components/memory_allocation.h"


//Function declarations:
void print_command(command *command, int n);

int main(){
	token **my_token_array;
	command **my_command_array;
	int i;
	char line[10000];

	printf("\nYourShell!\n");
	printf(">>> ");
	while(fgets(line, 10000, stdin))
	{
		//Get tokens:
		my_token_array = get_tokens(line);
		if(my_token_array != NULL && my_token_array[0] != NULL)
		{
			i = 0;
			while(my_token_array[i])
			{
				printf("--> my_array[%d]->string = %s\n", i, my_token_array[i]->string);
				i++;
			}
		}
		else
		{
			if(my_token_array != NULL)
				delete_token_array(&my_token_array);
			if(my_token_array == NULL)
				printf("Token array successfully deleted\n");
			printf(">>> ");
			continue;
		}

		//Get commands:
		printf("\nGet command\n");
		my_command_array = get_commands(my_token_array);
		if(my_command_array == NULL)
		{
			if(my_token_array != NULL)
				delete_token_array(&my_token_array);
			if(my_token_array == NULL)
				printf("Token array successfully deleted\n");
			printf("\n>>> ");
			continue;
		}
		i = 0;
		while(my_command_array[i])
		{
			print_command(my_command_array[i], i);
			i++;
		}

		if(my_token_array != NULL)
			delete_token_array(&my_token_array);
		if(my_token_array == NULL)
			printf("Token array successfully deleted\n");
		if(my_command_array != NULL)
			delete_command_array(&my_command_array);
		if(my_command_array == NULL)
			printf("Command array successfully deleted\n");
		printf("\n>>> ");

	}

		
	return 0;
}


void print_command(command *command, int n)
/**
 * Description: Debugging function that prints all elements of the command 'command'.
 *
 * Input: (command *) command --> pointer to the command.
 *         (int) n --> the number related to the command. It is used to customize
 *                     the message. -1 to ignore it.
 *
 * Output: (void)
 */
{
	int argc = 0, out_red_c = 0;
	if(n == -1)
		printf("Printing the next command:\n");
	else
		printf("Command %d :\n", n);

	printf("--> string: %s\n", command->command);
	if(command->argc < 2)
		printf("--> There is %d args.\n", command->argc);
	else
		printf("--> There are %d args.\n", command->argc);
	while(argc < command->argc)
	{
		printf("  * argv[%d] = %s\n", argc, command->argv[argc]);
		argc++;
	}
	if (command->input_redirect)
		printf("--> input redirect: %s\n", command->input_redirect);
	else
		printf("--> There is no input redirect.\n", command->input_redirect);

	if(command->out_red_c < 2)
		printf("--> There is %d output redirect args.\n", command->out_red_c);
	else
		printf("--> There are %d output redirect args.\n", command->out_red_c);
	while(out_red_c < command->out_red_c)
	{
		printf("  * output_redirect[%d] = %s\n", out_red_c, command->output_redirect[out_red_c]);
		out_red_c++;
		}
	printf("\n");

}
