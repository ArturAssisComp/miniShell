#include <stdio.h>
#include "../components/manipulate_string.h"
#include "../components/memory_allocation.h"


int main(){
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
			printf("\nMy array: NULL\n>>> ");
			continue;
		}
		i = 0;
		while(my_array[i])
		{
			printf("--> my_array[%d]->string = %s\n", i, my_array[i]->string);
			i++;
		}
		if(my_array != NULL)
			delete_token_array(&my_array);
		if(my_array == NULL)
			printf("Array successful deleted\n");
		printf("\n>>> ");

	}

		
	return 0;
}
