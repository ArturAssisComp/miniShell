#include "../../ext/ctest_lib/include/ctest.h"
#include "../../src/lexer/lexer.h"

//DEBUG:
#include <stdio.h>

int main(void)
{
	char suite_name[] = "Test Lexer";
	char suite_description[] = "This test suite checks the integrity of the lexer that will be used by the miniSh.";
	char *functions_tested[] = {
		"L_read_tokens",
		NULL
	};
	struct L_token_array *token_array;
	char *error_msg;

	//------------------------------------------------------------------------------
	start_suite(suite_name, suite_description, functions_tested);
		//------------------------------------------------------------------------------
		start_module("EDGE CASES", "Module for testing edge cases.", functions_tested);
			token_array = L_read_tokens("", &error_msg);
			assert_unsigned_integer_equal(token_array->num_of_tokens, 1, __LINE__, "Expects 1 token from empty string.");
			assert_unsigned_integer_equal(token_array->array[0]->token_type, EOF_TOKEN, __LINE__, "That token must be EOF_TOKEN.");
			L_delete_token_array(&token_array);
		end_module();
		//------------------------------------------------------------------------------
	end_suite();
	//------------------------------------------------------------------------------
}
