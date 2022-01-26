#include "types.h"
#include "array.h"
#include <stdlib.h>
#include <stdio.h>


//Definitions of functions:
T_array *A_create_array(void)
/**
 * Description: This function creates a T_array and returns the pointer to that 
 * T_array. After using, the user must call A_delete_array unless in the situation in which this T_array will 
 * be an T_element of another T_element that will be deleted.
 *
 * Input: (void)
 *
 * Output: (T_array *)
 */
{
	//Variables:
	T_array *new_array;
	char *error_msg;

	//Allocate memory:
	new_array = malloc(sizeof *new_array);

	if(!new_array)
	{
		error_msg = "Error while allocating memory for the T_array.";
		goto error;
	}

	//Initialize the T_array:
	new_array->el_array = NULL;
	new_array->num_of_elements = 0;
	new_array->capacity = 0;

result:
	return new_array;

error:
	fprintf(stderr, "%s", error_msg);
	exit(EXIT_FAILURE);
}



T_element *A_get_element(size_t index, T_array *current_array)
/**
 * Description: this function returns a pointer that point to the T_element of 
 * 'current_array' with index 'index'. If the 'index' is greater than the 
 * number of elements of 'current_array', NULL is returned.
 *
 * Input: (size_t) index
 *        (T_array *) current_array
 *
 * Output: (T_element *) --> the T_element of 'current_array' with index equals 
 *         to 'index'.
 */
{
	//Variables:
	T_element *found_element = NULL;

	//Try to get the T_element:
	if(index < current_array->num_of_elements) found_element = &(current_array->el_array[index]);


	return found_element;
}


void A_append_element(T_element e, T_array *current_array)
/**
 * Description: This function appends the T_element 'e' at the end of the T_array 
 * 'current_array'. If the T_array does not have the capacity to receive this new 
 * T_element, it allocates more memory.
 *
 * Input: (T_element) e
 *        (T_array *) current_array
 *
 * Output: (void)
 */
{

	//Variables:
	size_t new_capacity, index;
	char *error_message;
	T_element *temp;
	

	//Check if capacity is enough:
	if(current_array->num_of_elements + 1 > current_array->capacity)
	{
		//Allocate more memory:
		new_capacity = 2 * current_array->capacity + 5;
		temp = realloc(current_array->el_array, new_capacity * sizeof *current_array->el_array);

		if(temp)
		{
			current_array->el_array = temp;
			current_array->capacity = new_capacity;
		}
		else
		{
			A_delete_array(&current_array);
			error_message = "Error while expanding the capacity of the T_array.\n";
			goto error;
		}
	}

	//Add the new T_element:
	index = current_array->num_of_elements;
	current_array->el_array[index] = e;
	current_array->num_of_elements++;

result:
	return;

error:
	fprintf(stderr, "%s", error_message);
	exit(EXIT_FAILURE);
}


T_element A_pop(T_array *current_array)
/**
 * Description: This function deletes the last T_element of 'current_array' and 
 * returns a copy of that T_element. The user must call T_free_element after using
 * the T_element.
 *
 * Input: (T_array *) current_array
 *
 * Output: (T_element)
 */
{
	//Variables:
	T_element e;
	size_t new_capacity;
	char *error_message;
	T_element *temp;

	//Copy the T_element:
	if(current_array->num_of_elements > 0) e = current_array->el_array[--current_array->num_of_elements];
	else 
	{
		error_message = "There is no T_element to A_pop";
		goto error;
	}

	//Reduce the T_array if necessary:
	if(5 * current_array->num_of_elements < current_array->capacity)
	{
		//Allocate more memory:
		new_capacity = 1 + current_array->capacity / 5;
		temp = realloc(current_array->el_array, new_capacity * sizeof *current_array->el_array);

		if(temp)
		{
			current_array->el_array = temp;
			current_array->capacity = new_capacity;
		}
		else
		{
			A_delete_array(&current_array);
			error_message = "Error while reducing the capacity of the T_array.\n";
			goto error;
		}
	}

result:
	return e;

error:
	fprintf(stderr, "%s", error_message);
	exit(EXIT_FAILURE);
}

void A_delete_array(T_array **ptr_to_array)
{
	//Variables:
	T_array *current_array;
	size_t i;


	//Free all elements:
	current_array = *ptr_to_array;
	for(i = 0; i < current_array->num_of_elements; i++) T_free_element(&(current_array->el_array[i]));

	//Free the el_array:
	if(current_array->capacity > 0)	free(current_array->el_array);
	
	//Free the T_array itself:
	free(current_array);
	*ptr_to_array = NULL;
}



