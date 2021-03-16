#include <stdio.h>
#include "../components/manipulate_string.h"
#include "../components/memory_allocation.h"


int main(){
	/*
	char line[100];
	int index, i;
	token *my_token;
	my_token = (token *) xmalloc(sizeof *my_token);
	token new_token;
	while(fgets(line, 99, stdin))
	{
		index = 0;
		i = 0;
		while (index != -1)
		{
			index = _get_next_token(&new_token, line, index);

			//Empty string:
			if (index == 0)
				break;

			//Print the next word:
			printf("word[%d] = %s\n", ++i, new_token.string);
		}
	}
	*/
	token **my_array;
	int i;
	char line[10000];

	printf("\nYourShell!\n");
	printf(">>> ");
	while(fgets(line, 10000, stdin))
	{
		my_array = get_tokens(line);
		if(my_array == NULL)
		{
			printf("\n>>> ");
			continue;
		}
		i = 0;
		while(my_array[i])
		{
			printf("--> my_array[%d]->string = %s\n", i, my_array[i]->string);
			i++;
		}
		printf("\n>>> ");

	}

		
	return 0;
}
