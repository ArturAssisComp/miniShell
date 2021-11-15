#ifndef ASSERT_H
#define ASSERT_H
//Includes: 
#include <stdlib.h>
#include <stdbool.h>
#include "globals.h"
#include "std_assert_macros.h"

//Constants:
#define MAX_CHARS 4096 //The maximum number of chars for a message.


//Declarations of structs:
struct assert_result_struct
{
	bool was_successful;
	int line_number;
	char result_details[MAX_CHARS];
	char *assert_name;
	char *std_message;
	char *custom_message;
};


typedef struct assert_result_struct assert_result_struct;



//Declaration of auxiliary functions:
void print_assert_result(assert_result_struct assert_result);


//Declaration of functions:
/*Single data type*/
/*STD assert functions for unsigned_integer type*/
void assert_unsigned_integer_equal        (unsigned_integer target, unsigned_integer reference, int line_number, char custom_message[]);
void assert_unsigned_integer_notEqual     (unsigned_integer target, unsigned_integer reference, int line_number, char custom_message[]);
void assert_unsigned_integer_greater      (unsigned_integer target, unsigned_integer reference, int line_number, char custom_message[]);
void assert_unsigned_integer_greaterEqual (unsigned_integer target, unsigned_integer reference, int line_number, char custom_message[]);
void assert_unsigned_integer_less         (unsigned_integer target, unsigned_integer reference, int line_number, char custom_message[]);
void assert_unsigned_integer_lessEqual    (unsigned_integer target, unsigned_integer reference, int line_number, char custom_message[]);
void assert_unsigned_integer_bitMaskEqual (unsigned_integer target, unsigned_integer reference, unsigned_integer mask, int line_number, char custom_message[]);

/*STD assert functions for integer type*/
void assert_integer_equal        (integer target, integer reference, int line_number, char custom_message[]);
void assert_integer_notEqual     (integer target, integer reference, int line_number, char custom_message[]);
void assert_integer_greater      (integer target, integer reference, int line_number, char custom_message[]);
void assert_integer_greaterEqual (integer target, integer reference, int line_number, char custom_message[]);
void assert_integer_less         (integer target, integer reference, int line_number, char custom_message[]);
void assert_integer_lessEqual    (integer target, integer reference, int line_number, char custom_message[]);

/*STD assert functions for floating_point type*/
void assert_floating_point_almostEqual    (floating_point target, floating_point reference, floating_point max_diff, int line_number, char custom_message[]);
void assert_floating_point_notAlmostEqual (floating_point target, floating_point reference, floating_point min_diff, int line_number, char custom_message[]);
void assert_floating_point_greater        (floating_point target, floating_point reference, int line_number, char custom_message[]);
void assert_floating_point_greaterEqual   (floating_point target, floating_point reference, int line_number, char custom_message[]);
void assert_floating_point_less           (floating_point target, floating_point reference, int line_number, char custom_message[]);
void assert_floating_point_lessEqual      (floating_point target, floating_point reference, int line_number, char custom_message[]);

/*STD assert functions for bool type*/
void assert_bool_equal    (bool target, bool reference, int line_number, char custom_message[]);
void assert_bool_notEqual (bool target, bool reference, int line_number, char custom_message[]);
void assert_bool_true     (bool target, int line_number, char custom_message[]);
void assert_bool_false    (bool target, int line_number, char custom_message[]);

/*Arrays data type*/
/*STD assert functions for unsigned_integerArray type*/
void assert_unsigned_integerArray_equal                    (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notEqual                 (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_sorted                   (unsigned_integer target[], size_t target_size, bool ascending_order,  int line_number, char custom_message[]);
void assert_unsigned_integerArray_notSorted                (unsigned_integer target[], size_t target_size, bool ascending_order,  int line_number, char custom_message[]);
void assert_unsigned_integerArray_permutation              (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notPermutation           (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_isPartialPermutation     (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notIsPartialPermutation  (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_hasPartialPermutation    (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notHasPartialPermutation (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_setIn                    (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notSetIn                 (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_setHas                   (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notSetHas                (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_isSubarray               (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notIsSubarray            (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_hasSubarray              (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notHasSubarray           (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);



#endif
