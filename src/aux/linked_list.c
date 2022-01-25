//Libs:
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "linked_list.h"


//Declaration of local functions:
static void _delete_next_element(T_linked_list_element *previous_element, T_linked_list *target_linked_list);
static void _delete_head_element(T_linked_list *target_linked_list);



//Definition of functions:
T_linked_list *LL_create_linked_list(void)
/**
 * Description: This function creates a new linked list allocating memory and 
 * initializing it with 0 elements. The user must delete the linked list calling 
 * the function delete_linked_list after using it.
 *
 * Input: (none)
 *
 * Output: (T_linked_list *) --> Pointer to the linked list that was created.
 */
{
	//Variables:
	T_linked_list *new_linked_list;
	char *error_msg;

	//Allocate memory:
	new_linked_list = malloc(sizeof (*new_linked_list));
	if(!new_linked_list)
	{
		error_msg = "Error while allocating memory for the linked list.\n";
		goto error;
	}

	//Initialize the allocated linked list:
	new_linked_list->head = NULL;
	new_linked_list->num_of_elements = 0;

	//Return the result:
result:
	return new_linked_list;


error:
	//Error:
	fprintf(stderr, error_msg);
	exit(EXIT_FAILURE);
	
}



T_linked_list_element *LL_insert_item(T_dict_item item, T_linked_list *target_linked_list)
/**
 * Description: This function inserts a copy of the 'item' into the linked list 
 * pointed by 'target_linked_list', after its head. The user may not free the
 * element returned.
 *
 * Input: (T_dict_item) item --> The item that will be inserted into the 
 *        linked list.
 *        (T_linked_list *) target_linked_list --> Pointer to the linked list into
 *        which the new item will be added.
 *
 * Output: (dict_item *) --> Pointer to the interted element.
 */
{

	//Variables:
	T_linked_list_element *lk_element;
	char *error_msg;

	//Allocate memory:
	lk_element = malloc(sizeof (*lk_element));
	if(!lk_element)
	{
		error_msg = "Error while allocating memory for the linked list element.\n";
		goto error;
	}

	//Initialize the new element:
	lk_element->item = item;
	lk_element->next_element = target_linked_list->head;

	//Add the element:
	target_linked_list->head = lk_element;
	target_linked_list->num_of_elements++;

	//Return the result:
result:
	return lk_element;


error:
	//Error:
	fprintf(stderr, error_msg);
	exit(EXIT_FAILURE);
}


T_linked_list_element *LL_get_linked_list_element(T_element key, T_linked_list *target_linked_list)
/**
 * Description: This function returns a pointer to the first element into the 
 * target linked list that has key equals to 'key' during a traverse through 
 * the linked list, starting from the head until its tail.
 *
 * Input: (T_element) key --> The reference value for the key.
 *        (T_linked_list *) target_linked_list
 *
 * Output: (T_linked_list_element) --> If an element with the key equals to 'key'
 *         was found, its pointer will be returned. NULL is returned otherwise.
 */
{
	//Variables:
	T_linked_list_element *found_element = NULL;
	T_linked_list_element *current_element;

	//Search for the element:
	current_element = target_linked_list->head;
	while(current_element)
	{
		if (T_is_equal(key, current_element->item.key))
		{
			found_element = current_element;
			break;
		}
		else current_element = current_element->next_element;
	}

result:
	return found_element;
}


T_linked_list_element *LL_get_and_delete_linked_list_element(T_element key, T_linked_list *target_linked_list)
/**
 * Description: This function returns a pointer to the first element into the 
 * target linked list that has key equals to 'key' during a traverse through 
 * the linked list, starting from the head until its tail. It deletes this
 * element after finding it.
 *
 * Input: (T_element) key --> The reference value for the key.
 *        (T_linked_list *) target_linked_list
 *
 * Output: (T_linked_list_element) --> If an element with the key equals to 'key'
 *         was found, its pointer will be returned. NULL is returned otherwise.
 */
{
	//Variables:
	T_linked_list_element *found_element = NULL;
	T_linked_list_element *previous_element, *head_element;

	//Search for the element:
	head_element = target_linked_list->head;
	/*Check for the first element*/
	if (!head_element) goto result;
	if(T_is_equal(key, head_element->item.key))
	{
		found_element = head_element;
		_delete_head_element(target_linked_list);
		goto result;
	}

	/*Check for the remaining elements*/
	previous_element = head_element;

	while(previous_element->next_element)
	{
		if (T_is_equal(key, previous_element->next_element->item.key))
		{
			found_element = previous_element->next_element;
			//Deletes the element:
			_delete_next_element(previous_element, target_linked_list);
			goto result;
		}
		else previous_element = previous_element->next_element;
	}

result:
	return found_element;
}


void LL_delete_linked_list(T_linked_list **ptr_to_target_linked_list)
/**
 * Description: This function deletes the linked list pointed by 'ptr_to_target_linked_list'. 
 * It frees all memeory allocated by each element and frees the pointer that is 
 * pointed by 'ptr_to_taget_linked_list', assgning NULL value to it.
 *
 * Input: (T_linked_list **) ptr_to_target_linked_list --> Pointer to the pointer 
 *        that points to the target linked list.
 *
 * Output: (void)
 */
{
	//Variables:
	T_linked_list_element *current_linked_list_element_to_free;
	T_dict_item current_item;
	T_linked_list *target_linked_list;

	//Free memory allocated for each element of the linked list:
	target_linked_list = *ptr_to_target_linked_list;
	while(target_linked_list->num_of_elements > 0)
	{
		current_linked_list_element_to_free = target_linked_list->head;
		_delete_head_element(target_linked_list);
		//Free the linked list element:
		current_item = current_linked_list_element_to_free->item;
		T_free_element(&(current_item.key));
		T_free_element(&(current_item.value));
		free(current_linked_list_element_to_free);
	}

	//Free the linked list pointer:
	free(target_linked_list);
	*ptr_to_target_linked_list = NULL;
}

//Definition of local functions:
static void _delete_next_element(T_linked_list_element *previous_element, T_linked_list *target_linked_list)
{
	previous_element->next_element = previous_element->next_element->next_element;
	target_linked_list->num_of_elements--;
}



static void _delete_head_element(T_linked_list *target_linked_list)
{
	target_linked_list->head = target_linked_list->head->next_element;
	target_linked_list->num_of_elements--;
}
