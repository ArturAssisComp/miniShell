#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

//Libs:
#include "types.h"
#include <stdbool.h>
#include <stdlib.h>


//Declaration of functions:
bool HF_is_hashable(T_element e);
T_hash_type HF_generate_hash(T_element e, T_hash_type max_limit);



#endif
