#include "types.h"
#include "dict.h"
#include "linked_list.h"
#include "hash_function.h"
#include <stdio.h>
#include <stdlib.h>


//Definitions of the functions:
T_dict *D_create_dict(T_hash_type table_size)
/**
 * Description: This function creates a T_dict, allocating the memory necessary. 
 * The user must call D_delete_dict after using it.
 *
 * Input: (T_hash_type) table_size --> The size of the table that will be used to 
 *         store the elements. The table is an array of linked lists.
 *
 * Output: (T_dict *) --> The created T_dict.
 */
{
	//Variables:
	T_dict *created_dict;
	char *error_message;
	T_hash_type i;

	//Allocate memory for the T_dict:
	created_dict = malloc(sizeof (*created_dict));
	if(!created_dict)
	{
		error_message = "Problems while allocating memory for the T_dict.\n";
		goto error;
	}

	//Initialize the T_dict:
	created_dict->table = calloc(table_size, sizeof (T_linked_list*));
	if(!created_dict->table)
	{
		error_message = "Problems while allocating memory for the table of the T_dict.\n";
		goto error;
	}

	for(i = 0; i < table_size; i++)
	{
		created_dict->table[i] = LL_create_linked_list();
		if(!created_dict->table[i])
		{
			error_message = "Problems while allocating memory for the linked list of the table of the T_dict.\n";
			goto error;
		}

	}
	
	created_dict->table_size = table_size;
	created_dict->num_of_items = 0;
	
result:
	return created_dict;

error:
	fprintf(stderr, "%s", error_message);
	exit(EXIT_FAILURE);

}

void D_delete_dict(T_dict **ptr_to_dict)
{
	//Variables:
	T_dict *current_dict;
	T_linked_list *current_linked_list;
	T_hash_type i;
	char *error_msg;

	//Free each linked list from the table:
	current_dict = *ptr_to_dict;
	for(i = 0; i < current_dict->table_size; i++)
	{
		current_linked_list = current_dict->table[i];
		current_dict->num_of_items -= current_linked_list->num_of_elements;
		LL_delete_linked_list(&current_linked_list);
		if(current_linked_list)
		{
			error_msg = "Error while deleting linked list from table of T_dict.";
			goto error;
		}
	}
	if(current_dict->num_of_items != 0)
	{
		error_msg = "Error while the table of T_dict.";
		goto error;
	}

	free(current_dict->table);
	free(current_dict);
	*ptr_to_dict = NULL;

result:
	return;

error:
	fprintf(stderr, "%s", error_msg);
	exit(EXIT_FAILURE);

}



void D_assign_value_to_key(T_element key, T_element value, T_dict *target_dict)
/**
 * Description: This function assigns the value 'value' to the key 'key' into 
 * the T_dict 'target_dict'. If key is not hashable, an error occurs.
 *
 * Input: (T_element) key, value --> A copy of the key and the value that will be 
 *         used as reference to create an item key:value and to add it into the 
 *         target T_dict.
 *         (T_dict *) target_dict;
 *
 * Output: (void)
 */
{
	//Variables:
	T_dict_item dict_item_to_add;
	T_hash_type hash_value;
	T_linked_list *current_linked_list;
	T_linked_list_element *found_element;

	//Generate the hash value:
	hash_value  = HF_generate_hash(key, target_dict->table_size);
	

	//Check if the T_element already exist:
	current_linked_list = target_dict->table[hash_value];
	found_element = LL_get_linked_list_element(key, current_linked_list);
	if(found_element)
	{
		T_free_element(&(found_element->item.value));
		found_element->item.value = value;
	}
	else
	{
		//Initialize the T_dict that will be added:
		dict_item_to_add.hash  = hash_value;
		dict_item_to_add.key   = key;
		dict_item_to_add.value = value;

		//Add the T_element into the linked list of the T_dict:
		LL_insert_item(dict_item_to_add, current_linked_list);
		target_dict->num_of_items++;
	}


}

T_element *D_get_value(T_element key, T_dict *target_dict)
/**
 * Description: This function returns a pointer to the value of the item of 
 * 'target_dict' that has key equals to 'key'. If the T_element with key 'key'
 * is not found, NULL is returned. 'key' must be a hashable T_element, otherwise
 * an error is raised.
 *
 * Input: (T_element) key
 *        (T_dict *) target_dict
 *
 * Output: (T_element *)
 */
{
	//Variables:
	T_element *found_value = NULL;
	T_linked_list_element *found_linked_list_element;
	T_hash_type hash_value;

	//Calculate the hash:
	hash_value  = HF_generate_hash(key, target_dict->table_size);

	//Get the T_element:
	found_linked_list_element = LL_get_linked_list_element(key, target_dict->table[hash_value]);

	if(found_linked_list_element) found_value = &(found_linked_list_element->item.value);

	return found_value;
}

bool D_delete_element(T_element key, T_dict *target_dict)
/**
 * Description: This function tries to delete the T_element of 'target_dict' with 
 * key 'key' if this T_element exists. If the elment exists and is deleted, it 
 * returns true. If the T_element does not exist, it returns false.
 *
 * Input: (T_element) key
 *        (T_dict *) target_dict
 *
 * Output: (bool)
 */
{
	//Variables:
	T_hash_type hash_value;
	T_linked_list_element *found_element;
	bool result = false;

	//Generate the hash value:
	hash_value  = HF_generate_hash(key, target_dict->table_size);
	

	//Try to delete the T_element:
	found_element = LL_get_and_delete_linked_list_element(key, target_dict->table[hash_value]);

	//Check if it was deleted:
	if(found_element)
	{
		T_free_element(&(found_element->item.key));
		T_free_element(&(found_element->item.value));

		target_dict->num_of_items--;
		result = true;
	}

	return result;

}
