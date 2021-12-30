/**
 * Author: Artur Assis Alves
 * Title: Lib --> Memory Allocation
 */

#include "memory_allocation.h"

//Function definitions:

void *xcalloc(size_t num_of_elements, size_t size_of_element)
/**
 * Description: This function is a subroutine to call calloc. The function tries
 * to allocate memory. If the memory allocation is successful, it returns the 
 * resulting pointer. Otherwise, it raises an error.
 *
 *
 * Input: (size_t) num_of_elements --> Number of elements that will be allocated
 *                                     in the array.
 *        (size_t) size_of_element --> The size of each element of the array.
 *
 * Output: (void *) --> Pointer to the first address of the allocated array.
 *
 */
{
	//Try to allocate memory:
	void *value = calloc(num_of_elements, size_of_element);

	//Check if the memory was successful allocated:
	if(value == NULL)
	{
		perror("Virtual memory exhausted");
		exit(EXIT_FAILURE);
	}


	//Return the pointer:
	return value;
}



void *xmalloc(size_t size)
/**
 * Description: This function is a subroutine to call malloc. The function tries
 * to allocate memory. If the memory allocation is successful, it returns the 
 * resulting pointer. Otherwise, it raises an error.
 *
 * Input: (size_t) size --> The size in bytes of the memory space that will be allocated.
 * 
 * Output: (void *) --> Pointer to the first address of the allocated array.
 */
{
	//Try to allocate memory:
	void *value = malloc(size);

	//Check if the memory was successful allocated:
	if(value == NULL)
	{
		perror("Virtual memory exhausted");
		exit(EXIT_FAILURE);
	}

	//Return the pointer:
	return value;
}




void *xrealloc(void *ptr, size_t size)
/**
 * Description: This function is a subroutine to call realloc. The function tries
 * to reallocate memory. If the memory reallocation is successful, it returns the 
 * resulting pointer. Otherwise, it raises an error.
 *
 * Input: (void *) ptr --> The pointer that points to the memory that will be reallocated.
 *        (size_t) size --> The size in bytes of the memory space that will be allocated.
 * 
 * Output: (void *) --> Pointer to the first address of the reallocated array.
 */
{
	//Try to allocate memory:
	ptr = realloc(ptr, size);

	//Check if the memory was successful reallocated:
	if(ptr == NULL)
	{
		perror("Virtual memory exhausted");
		exit(EXIT_FAILURE);
	}

	//Return the pointer:
	return ptr;
}

