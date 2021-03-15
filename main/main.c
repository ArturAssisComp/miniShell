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

int main()
{
	char *buffer, **words;
	int num_of_args;
	buffer = (char *)xcalloc(MAX_SIZE + 1, sizeof (char));
	buffer = (char **)xcalloc(MAX_WORDS, sizeof (char *));
	
	while(fgets(buffer, MAX_SIZE, stdin))
	{
		num_of_args = get_words(words, buffer);
		int i = 0;
		for (; i < num_of_args; i++)
			printf("Word[%d] = '%s'\n", i + 1, words[i]);
	}
	free(buffer);
	return 0;
}
