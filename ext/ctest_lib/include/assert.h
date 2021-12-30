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

/*STD assert functions for char type*/
void assert_char_equal        (char target, char reference, bool ingnore_case, int line_number, char custom_message[]);
void assert_char_notEqual     (char target, char reference, bool ingnore_case, int line_number, char custom_message[]);
void assert_char_greater      (char target, char reference, bool ingnore_case, int line_number, char custom_message[]);
void assert_char_greaterEqual (char target, char reference, bool ingnore_case, int line_number, char custom_message[]);
void assert_char_less         (char target, char reference, bool ingnore_case, int line_number, char custom_message[]);
void assert_char_lessEqual    (char target, char reference, bool ingnore_case, int line_number, char custom_message[]);


/*STD assert functions for files*/
void assert_files_binaryContentEqual           (char target_file_path[], char reference_file_path[], int line_number, char custom_message[]);
void assert_files_contentEqual                 (char target_file_path[], char reference_file_path[], bool ignore_case, int line_number, char custom_message[]);
void assert_file_path_contentEqual             (char target_file_path[], char reference_content[], bool ignore_case, int line_number, char custom_message[]);

/*STD assert functions for void pointers*/
void assert_pointer_isNULL    (void *target, int line_number, char custom_message[]);
void assert_pointer_notIsNULL (void *target, int line_number, char custom_message[]);

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
void assert_unsigned_integerArray_setEqual                 (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_setIn                    (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notSetIn                 (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_setHas                   (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notSetHas                (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_isSubarray               (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notIsSubarray            (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_hasSubarray              (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_unsigned_integerArray_notHasSubarray           (unsigned_integer target[], size_t target_size, unsigned_integer reference[], size_t reference_size, int line_number, char custom_message[]);


/*STD assert functions for integerArray type*/
void assert_integerArray_equal                    (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_notEqual                 (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_sorted                   (integer target[], size_t target_size, bool ascending_order,  int line_number, char custom_message[]);
void assert_integerArray_notSorted                (integer target[], size_t target_size, bool ascending_order,  int line_number, char custom_message[]);
void assert_integerArray_permutation              (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_notPermutation           (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_isPartialPermutation     (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_notIsPartialPermutation  (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_hasPartialPermutation    (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_notHasPartialPermutation (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_setEqual                 (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_setIn                    (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_notSetIn                 (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_setHas                   (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_notSetHas                (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_isSubarray               (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_notIsSubarray            (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_hasSubarray              (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_integerArray_notHasSubarray           (integer target[], size_t target_size, integer reference[], size_t reference_size, int line_number, char custom_message[]);


/*STD assert functions for boolArray type*/
void assert_boolArray_equal                    (bool target[], size_t target_size, bool reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_boolArray_notEqual                 (bool target[], size_t target_size, bool reference[], size_t reference_size, int line_number, char custom_message[]);
void assert_boolArray_allTrue                  (bool target[], size_t target_size, int line_number, char custom_message[]);
void assert_boolArray_anyFalse                 (bool target[], size_t target_size, int line_number, char custom_message[]);
void assert_boolArray_allFalse                 (bool target[], size_t target_size, int line_number, char custom_message[]);
void assert_boolArray_anyTrue                  (bool target[], size_t target_size, int line_number, char custom_message[]);




/*STD assert functions for charArray type*/
void assert_charArray_equal                    (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notEqual                 (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_sorted                   (char target[], size_t target_size, bool ascending_order,  bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notSorted                (char target[], size_t target_size, bool ascending_order,  bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_permutation              (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notPermutation           (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_isPartialPermutation     (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notIsPartialPermutation  (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_hasPartialPermutation    (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notHasPartialPermutation (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_setEqual                 (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_setIn                    (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notSetIn                 (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_setHas                   (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notSetHas                (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_isSubarray               (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notIsSubarray            (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_hasSubarray              (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);
void assert_charArray_notHasSubarray           (char target[], size_t target_size, char reference[], size_t reference_size, bool ingnore_case, int line_number, char custom_message[]);


/*STD assert functions for string type*/
void assert_string_equal                    (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notEqual                 (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_greater                  (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_greaterEqual             (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_less                     (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_lessEqual                (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_sizeEqual                (char target[], size_t reference_size, int line_number, char custom_message[]);
void assert_string_notSizeEqual             (char target[], size_t reference_size, int line_number, char custom_message[]);
void assert_string_sizeGreater              (char target[], size_t reference_size, int line_number, char custom_message[]);
void assert_string_sizeGreaterEqual         (char target[], size_t reference_size, int line_number, char custom_message[]);
void assert_string_sizeLess                 (char target[], size_t reference_size, int line_number, char custom_message[]);
void assert_string_sizeLessEqual            (char target[], size_t reference_size, int line_number, char custom_message[]);
void assert_string_hasChar                  (char target[], char reference_char, bool ignore_case, int line_number, char custom_message[]);
void assert_string_notHasChar               (char target[], char reference_char, bool ignore_case, int line_number, char custom_message[]);
void assert_string_sameSize                 (char target[], char reference[], int line_number, char custom_message[]);
void assert_string_equal                    (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notEqual                 (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_sorted                   (char target[], bool ascending_order,  bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notSorted                (char target[], bool ascending_order,  bool ingnore_case, int line_number, char custom_message[]);
void assert_string_permutation              (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notPermutation           (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_isPartialPermutation     (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notIsPartialPermutation  (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_hasPartialPermutation    (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notHasPartialPermutation (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_setEqual                 (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_setIn                    (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notSetIn                 (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_setHas                   (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notSetHas                (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_isSubarray               (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notIsSubarray            (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_hasSubarray              (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_notHasSubarray           (char target[], char reference[], bool ingnore_case, int line_number, char custom_message[]);
void assert_string_empty                    (char target[], int line_number, char custom_message[]);
void assert_string_notEmpty                 (char target[], int line_number, char custom_message[]);



#endif
