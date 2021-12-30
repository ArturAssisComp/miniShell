#ifndef CTEST_FUNCTIONS_H
#define CTEST_FUNCTIONS_H

#include "globals.h"

//Macros:
#define MAX_NAME 32          //Maximum number of chars per name.
#define MAX_DESCRIPTION 1024 //Maximum number of chars per description.

//Definition of types:
typedef struct {
	char suite_progress[MAX_RESULTS + 1];
	char suite_name[MAX_NAME];
	char module_name[MAX_NAME];
	int num_of_suite_results;
	int num_of_suite_fails;
	int num_of_suite_ignored;
} suite_ctest_info_struct;


//Declare global variables:
extern suite_ctest_info_struct suite_ctest_info;

//Declaration of functions:
void start_suite(char *name, char *description, char *func_ids[]);
void end_suite();
void start_module(char *name, char *description, char *func_ids[]);
void end_module();
#endif
