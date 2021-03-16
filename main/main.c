/**
 * Author: Artur Assis Alves
 * Title: MiniShell - Main Function
 */

//Include:
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../components/memory_allocation.h"
#include "../components/manipulate_string.h"


//Define:
#define MAX_SIZE  1000 
#define MAX_WORDS 100

int main(){
	token **my_array;
	int i;
	char line[MAX_SIZE + 1];

	//Initialize the shell:
	printf("\nYourShell!\n");
	printf(">>> ");
	while(fgets(line, MAX_SIZE, stdin))
	{
		my_array = get_tokens(line);
		if(my_array == NULL)
		{
			printf("\n>>> ");
		}
		else
		{
			i = 0;
			while(my_array[i])
			{
				printf("--> my_array[%d]->string = %s\n", i, my_array[i]->string);
				i++;
			}
			printf("\n>>> ");
		}
	}

	//Free the allocated memory:
	i = 0;
	while (my_array[i])
	{
		if (my_array[i]->string)
			free(my_array[i]->string);
	}
	free(my_array);
	return 0;
}
