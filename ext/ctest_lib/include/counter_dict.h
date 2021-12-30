#ifndef COUNTER_DICT_H
#define COUNTER_DICT_H

//Include libs:
#include "types.h"



//Macro constants:

//Declaration of types:

//Declaration of functions:
T_dict *CD_count_elements(T_array *target_array);
size_t CD_pop_next_index(T_element key, T_dict *target_dict);
size_t CD_get_count(T_element key, T_dict *target_dict);

#endif
