#ifndef ARRAY_H
#define ARRAY_H

//Libs
#include "types.h"
#include <stdlib.h>

//Declarations of functions:
T_array *A_create_array(void);
T_element *A_get_element(size_t index, T_array *current_array);
void A_append_element(T_element e, T_array *current_array);
T_element A_pop(T_array *current_array);
void A_delete_array(T_array **ptr_to_array);
#endif
