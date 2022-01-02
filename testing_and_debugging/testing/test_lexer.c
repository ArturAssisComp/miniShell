#include "../../ext/ctest_lib/include/ctest.h"
#include "../../src/lexer/lexer.h"
#include <stdio.h>

#define MSG_SIZE 100

int main(void)
{
	char suite_name[] = "Test Lexer";
	char suite_description[] = "This test suite checks the integrity of the lexer that will be used by the miniSh.";
	char *functions_tested[] = {
		"L_read_tokens",
		NULL
	};
	char error_msg[L_ERROR_MSG_SZ] = "";
	char *current_str;
	char *expected_value;
	char msg[MSG_SIZE]; 
	struct L_token_array *token_array;
	bool ignore_case = false;

	//------------------------------------------------------------------------------
	start_suite(suite_name, suite_description, functions_tested);
		//------------------------------------------------------------------------------
		start_module("EDGE CASES", "Module for testing edge cases.", functions_tested);
			//Empty string:		
			current_str = "";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 1 token from the %s.", "empty string");
				assert_unsigned_integer_equal(token_array->num_of_tokens, 1, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}

			//White space:		
			current_str = "  \n";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 1 token from the %s.", "empty string");
				assert_unsigned_integer_equal(token_array->num_of_tokens, 1, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}

			//Invalid char:
			/*abc %*/
			current_str = "abc %";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_isNULL(token_array, __LINE__, msg);

			/*   \t*/
			current_str = "   \t^";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_isNULL(token_array, __LINE__, msg);


			/*invalid \"\"*/
			current_str = "invalid \"\"";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_isNULL(token_array, __LINE__, msg);

			/*\"\"*/
			current_str = "\"\"";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_isNULL(token_array, __LINE__, msg);

			/*\"   \"*/
			current_str = "\"   \"";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_isNULL(token_array, __LINE__, msg);



			//RES_WORD:
			//(...)

			//ID:
			/*Var1*/
			current_str = "Var1";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, ID, __LINE__, msg);
				expected_value = "Var1";
				snprintf(msg, MSG_SIZE + 1, "The first token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[0]->token_value, expected_value, ignore_case, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


			/*-abc/de*/
			current_str = "-abc/de";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, ID, __LINE__, msg);
				expected_value = "-abc/de";
				snprintf(msg, MSG_SIZE + 1, "The first token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[0]->token_value, expected_value, ignore_case, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}

			
			/*_*/
			current_str = "_";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, ID, __LINE__, msg);
				expected_value = "_";
				snprintf(msg, MSG_SIZE + 1, "The first token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[0]->token_value, expected_value, ignore_case, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


			//quoted ID:
			/*\"C:/My_folder/file 1.pdf\"*/
			current_str = "\"C:/My_folder/file 1.pdf\"";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, ID, __LINE__, msg);
				expected_value = "C:/My_folder/file 1.pdf";
				snprintf(msg, MSG_SIZE + 1, "The first token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[0]->token_value, expected_value, ignore_case, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


			//IN_REDIRECT:
			/*<*/
			current_str = "<";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type IN_REDIRECT.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, IN_REDIRECT, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


			//OUT_REDIRECT:
			/*>*/
			current_str = ">";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type OUT_REDIRECT.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, OUT_REDIRECT, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


			//PIPE:
			/*|*/
			current_str = "|";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type PIPE.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, PIPE, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}



			//BACKGROUND_EXEC:
			/*&*/
			current_str = "&";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type BACKGROUND_EXEC.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, BACKGROUND_EXEC, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


			
			//NEW_LINE:
			/*\*/
			current_str = "\\";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type NEW_LINE.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, NEW_LINE, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}



			//COMMENT:
			/* ** */
			current_str = "**";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type COMMENT.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, COMMENT, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


			/* ** Some comment here*/
			current_str = "** Some comment here";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type COMMENT.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, COMMENT, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}

			/* ** @#4jkalfh#$-_*/
			current_str = "** @#4jkalfh#$-_";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 2 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type COMMENT.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, COMMENT, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}

			//String with 2 tokens:
			/*command1 -abc */
			current_str = "command1 -abc ";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				//Num of tokens:
				snprintf(msg, MSG_SIZE + 1, "Expects 3 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 3, __LINE__, msg);
				//First token:
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, ID, __LINE__, msg);
				expected_value = "command1";
				snprintf(msg, MSG_SIZE + 1, "The first token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[0]->token_value, expected_value, ignore_case, __LINE__, msg);

				//Second token:
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, ID, __LINE__, msg);
				expected_value = "-abc";
				snprintf(msg, MSG_SIZE + 1, "The second token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[1]->token_value, expected_value, ignore_case, __LINE__, msg);

				//Third token:
				snprintf(msg, MSG_SIZE + 1, "The third token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[2]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}





		end_module();
		//------------------------------------------------------------------------------
		

		//------------------------------------------------------------------------------
		start_module("DOMAIN CASES", "Module for testing domain cases.", functions_tested);
			//String 
			/*Command1 -abc /src/123 > out.txt<in.txt | "command 2" ** comment*/		
			current_str = "Command1 -abc /src/123 > out.txt<in.txt | \"command 2\" ** comment";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				//Num of tokens:
				snprintf(msg, MSG_SIZE + 1, "Expects 11 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 11, __LINE__, msg);
				//First token (Command1):
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, ID, __LINE__, msg);
				expected_value = "Command1";
				snprintf(msg, MSG_SIZE + 1, "The first token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[0]->token_value, expected_value, ignore_case, __LINE__, msg);

				//Second token (-abc):
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, ID, __LINE__, msg);
				expected_value = "-abc";
				snprintf(msg, MSG_SIZE + 1, "The second token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[1]->token_value, expected_value, ignore_case, __LINE__, msg);

				//Third token (/src/123):
				snprintf(msg, MSG_SIZE + 1, "The third token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[2]->token_type, ID, __LINE__, msg);
				expected_value = "/src/123";
				snprintf(msg, MSG_SIZE + 1, "The third token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[2]->token_value, expected_value, ignore_case, __LINE__, msg);
				

				//Fourth token (>):
				snprintf(msg, MSG_SIZE + 1, "The fourth token must have the type OUT_REDIRECT.");
				assert_unsigned_integer_equal(token_array->array[3]->token_type, OUT_REDIRECT, __LINE__, msg);


				//Fifth token (out.txt):
				snprintf(msg, MSG_SIZE + 1, "The fifth token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[4]->token_type, ID, __LINE__, msg);
				expected_value = "out.txt";
				snprintf(msg, MSG_SIZE + 1, "The fifth token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[4]->token_value, expected_value, ignore_case, __LINE__, msg);
				
				//Sixth token (<):
				snprintf(msg, MSG_SIZE + 1, "The sixth token must have the type IN_REDIRECT.");
				assert_unsigned_integer_equal(token_array->array[5]->token_type, IN_REDIRECT, __LINE__, msg);


				//Seventh token (in.txt):
				snprintf(msg, MSG_SIZE + 1, "The seventh token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[6]->token_type, ID, __LINE__, msg);
				expected_value = "in.txt";
				snprintf(msg, MSG_SIZE + 1, "The seventh token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[6]->token_value, expected_value, ignore_case, __LINE__, msg);


				//Eigth token (|):
				snprintf(msg, MSG_SIZE + 1, "The eigth token must have the type PIPE.");
				assert_unsigned_integer_equal(token_array->array[7]->token_type, PIPE, __LINE__, msg);


				//Ninth token (command 2):
				snprintf(msg, MSG_SIZE + 1, "The ninth token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[8]->token_type, ID, __LINE__, msg);
				expected_value = "command 2";
				snprintf(msg, MSG_SIZE + 1, "The ninth token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[8]->token_value, expected_value, ignore_case, __LINE__, msg);

				//Tenth token (comment)
				snprintf(msg, MSG_SIZE + 1, "The tenth token must have the type COMMENT.");
				assert_unsigned_integer_equal(token_array->array[9]->token_type, COMMENT, __LINE__, msg);


				//Eleventh token (EOF)
				snprintf(msg, MSG_SIZE + 1, "The eleventh token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[10]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


			/*>|command_123 & ***/
			current_str = ">|command_123 & **";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				//Num of tokens:
				snprintf(msg, MSG_SIZE + 1, "Expects 6 tokens from the string %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 6, __LINE__, msg);
				//First token (>):
				snprintf(msg, MSG_SIZE + 1, "The first token must have the type OUT_REDIRECT.");
				assert_unsigned_integer_equal(token_array->array[0]->token_type, OUT_REDIRECT, __LINE__, msg);

				//Second token (|):
				snprintf(msg, MSG_SIZE + 1, "The second token must have the type PIPE.");
				assert_unsigned_integer_equal(token_array->array[1]->token_type, PIPE, __LINE__, msg);

				//Third token (command_123):
				snprintf(msg, MSG_SIZE + 1, "The third token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[2]->token_type, ID, __LINE__, msg);
				expected_value = "command_123";
				snprintf(msg, MSG_SIZE + 1, "The ninth token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[2]->token_value, expected_value, ignore_case, __LINE__, msg);

				//Fourth token (&):
				snprintf(msg, MSG_SIZE + 1, "The fourth token must have the type BACKGROUND_EXEC.");
				assert_unsigned_integer_equal(token_array->array[3]->token_type, BACKGROUND_EXEC, __LINE__, msg);

				//Fifth token (**):
				snprintf(msg, MSG_SIZE + 1, "The fifth token must have the type comment.");
				assert_unsigned_integer_equal(token_array->array[4]->token_type, COMMENT, __LINE__, msg);

				//Sixth token (EOF):
				snprintf(msg, MSG_SIZE + 1, "The sixth token must have the type EOF_TOKEN.");
				assert_unsigned_integer_equal(token_array->array[5]->token_type, EOF_TOKEN, __LINE__, msg);
				L_delete_token_array(&token_array);
			}


		end_module();
		//------------------------------------------------------------------------------

		start_module("ARBITRARY CASES", "Module for testing arbitrary cases.", functions_tested);
			//Empty string:		
			current_str = "||> \\ \"  9\" ";
			token_array = L_read_tokens(current_str, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if L_read_tokens returned NULL.", current_str);
			assert_pointer_notIsNULL(token_array, __LINE__, msg);
			if(token_array)
			{
				snprintf(msg, MSG_SIZE + 1, "Expects 6 tokens from the %s.", current_str);
				assert_unsigned_integer_equal(token_array->num_of_tokens, 6, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The fifth token must have the type ID.");
				assert_unsigned_integer_equal(token_array->array[4]->token_type, ID, __LINE__, msg);
				expected_value = "  9";
				snprintf(msg, MSG_SIZE + 1, "The fifth token must have the value %s.", expected_value);
				assert_string_equal(token_array->array[4]->token_value, expected_value, ignore_case, __LINE__, msg);
				L_delete_token_array(&token_array);
			}
		end_module();
		//------------------------------------------------------------------------------


	end_suite();
	//------------------------------------------------------------------------------
}
