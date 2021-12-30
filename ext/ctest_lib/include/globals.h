#ifndef GLOBALS_H
#define GLOBALS_H

//Include libs:
#include <stdbool.h>



//Macros:
#define HIGH 2
#define MEDIUM 1
#define LOW 0
#define MAX_RESULTS 500      //Maximum number of results per module.

//Definition of types:
typedef struct {
	bool print_progress; //Says if it is necessary to print 'Progress: %s'.
	char progress[MAX_RESULTS + 1];
	int num_of_results;
	int num_of_fails;
	int total_results;
	int total_fails;
	int num_of_ignored; 
        int total_ignored;
} ctest_info_struct;

typedef long long int integer;
typedef unsigned long long int unsigned_integer;
typedef double floating_point;

//Declare global Variables:
extern bool ignore;
extern int verbose;
extern ctest_info_struct ctest_info;

#endif
