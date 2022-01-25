#ifndef LINKED_LIST_H
#define LINKED_LIST_H

//Libs:
#include <stdbool.h>
#include "types.h"


//Declaration of functions:

T_linked_list *LL_create_linked_list(void);
void LL_delete_linked_list(T_linked_list **ptr_to_target_linked_list);
T_linked_list_element *LL_insert_item(T_dict_item item, T_linked_list *target_linked_list);
T_linked_list_element *LL_get_and_delete_linked_list_element(T_element key, T_linked_list *target_linked_list);
T_linked_list_element *LL_get_linked_list_element(T_element key, T_linked_list *target_linked_list);



#endif
