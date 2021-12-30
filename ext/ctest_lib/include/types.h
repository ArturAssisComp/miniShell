#ifndef TYPES_H
#define TYPES_H

//Include libs:
#include "globals.h"
#include <stdbool.h>
#include <stdlib.h>

//Macro constants:

//Declaration of types:

/*Element*/
typedef enum 
{
    //Individual elements:
    UNSIGNED_INTEGER,
    INTEGER,
    FLOATING_POINT,
    BOOL,
    CHAR,
    STRING,
    //Group elements:
    ARRAY, //Array of generic elements
    DICT,  //Dict of generic elements
    NULL_TYPE
} T_data_type;


struct T_element;
struct T_linked_list;
typedef size_t T_hash_type;

typedef struct
{
    struct T_element *el_array;
    size_t num_of_elements;
    size_t capacity;
} T_array;

typedef struct 
{
        struct T_linked_list **table;
        T_hash_type table_size;
        size_t num_of_items;
}T_dict;

struct T_element 
{
    union {
        unsigned_integer u_i;
        integer i;
        floating_point f_p;
        bool b;
        char c;
        char *str; //A string is considered an individual element. 
        T_array *arr;
        T_dict *dct;
    } value;
    T_data_type type;
};
typedef struct T_element T_element;


/*Dict element*/
typedef struct 
{
    T_element key;
    T_element value;
    T_hash_type hash;
} T_dict_item; //Pair of a key associated with a value.

/*Linked list of dict elements*/
struct T_linked_list_element 
{
    T_dict_item item;
    struct T_linked_list_element *next_element;
};
typedef struct T_linked_list_element T_linked_list_element;

struct T_linked_list
{
    T_linked_list_element *head;
    size_t num_of_elements;

};
typedef struct T_linked_list T_linked_list;


//Function declarations:
bool T_is_equal(T_element e1, T_element e2);
void T_free_element(T_element *e);


#endif
