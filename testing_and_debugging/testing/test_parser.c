#include "../../ext/ctest_lib/include/ctest.h"
#include "../../src/lexer/lexer.h"
#include "../../src/parser/parser.h"
#include <stdio.h>
#include <string.h>

#define MSG_SIZE 100
#define MAX 512

#define LTP &(struct L_token)
#define LTAP &(struct L_token_array)
//Must use LTPP_start{ <content of type L_token **> }LTPP_end
#define LTPP_start &(struct L_token*[])
#define LTPP_end [0]
int main(void)
{
	char suite_name[] = "Test Parser";
	char suite_description[] = "This test suite checks the integrity of the parser that will be used by the miniSh.";
	char *functions_tested[] = {
		"P_parse",
		NULL
	};
	char error_msg[P_ERROR_MSG_SZ] = "";
	char *current_str;
	char *expected_value;
	char msg[MSG_SIZE]; 
	bool ignore_case = false;
    struct L_token_array *current_token_array;
    struct P_command_pipeline_linked_list *current_command_pipeline_linked_list;
    struct P_command_pipeline_node *tmp_node;
    struct P_command *tmp_command;


	//------------------------------------------------------------------------------
	start_suite(suite_name, suite_description, functions_tested);
		//------------------------------------------------------------------------------
		start_module("EDGE CASES", "Module for testing edge cases.", functions_tested);
            /*SYNTAX ERROR*/
            //Parse the token array: {IN_REDIRECT, EOF_TOKEN}
            current_token_array = LTAP{ LTPP_start{LTP{IN_REDIRECT}, LTP{EOF_TOKEN}}LTPP_end, 2};

            error_msg[0] = '\0';
            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_isNULL(current_command_pipeline_linked_list, __LINE__, msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse changed error_msg.", current_str);
            assert_unsigned_integer_greater(strlen(error_msg), 0, __LINE__, msg);
            //############end################

            //Parse the token array: {COMMENT}
            current_token_array = LTAP{ LTPP_start{LTP{COMMENT}}LTPP_end, 1};

            error_msg[0] = '\0';
            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_isNULL(current_command_pipeline_linked_list, __LINE__, msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse changed error_msg.", current_str);
            assert_unsigned_integer_greater(strlen(error_msg), 0, __LINE__, msg);
            //############end################

            //Parse the token array: {ID("abc"), ID("def")}
            //Desc.: The array is not terminated by EOF_TOKEN. It should return syntax error.
            current_token_array = LTAP{ LTPP_start{LTP{ID, "abc"}, LTP{ID, "def"}}LTPP_end, 2};

            error_msg[0] = '\0';
            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_isNULL(current_command_pipeline_linked_list, __LINE__, msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse changed error_msg.", current_str);
            assert_unsigned_integer_greater(strlen(error_msg), 0, __LINE__, msg);
            //############end################

            //Parse the token array: {}
            current_token_array = LTAP{.num_of_tokens=0};

            error_msg[0] = '\0';
            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_isNULL(current_command_pipeline_linked_list, __LINE__, msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse changed error_msg.", current_str);
            assert_unsigned_integer_greater(strlen(error_msg), 1, __LINE__, msg);
            //############end################

            /*NOT SYNTAX ERROR*/
            //Parse the token array: {ID("abc"), EOF_TOKEN}
            current_token_array = LTAP{ LTPP_start{LTP{ID, "abc"}, LTP{EOF_TOKEN}}LTPP_end, 2};

            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_notIsNULL(current_command_pipeline_linked_list, __LINE__, msg);

			if(current_command_pipeline_linked_list)
			{
				//Num of command pipelines:
				snprintf(msg, MSG_SIZE + 1, "Expects 1 command pipeline");
				assert_unsigned_integer_equal(current_command_pipeline_linked_list->length, 1, __LINE__, msg);
				/*First pipeline*/
                tmp_node = current_command_pipeline_linked_list->head;
				snprintf(msg, MSG_SIZE + 1, "The first pipeline must have 1 command.");
				assert_unsigned_integer_equal(tmp_node->length, 1, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first pipeline must have 1 command remaining to execute.");
				assert_unsigned_integer_equal(tmp_node->remaining_to_execute, 1, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline flag was_executed must be false.");
				assert_bool_false(tmp_node->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline flag execute_in_background must be false.");
				assert_bool_false(tmp_node->execute_in_background, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline pointer next_pipeline must be NULL.");
				assert_pointer_isNULL(tmp_node->next_pipeline, __LINE__, msg);

                /*First command*/
                tmp_command = tmp_node->first_command;
				expected_value = "abc";
				snprintf(msg, MSG_SIZE + 1, "The first command must have the id equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->id, expected_value, ignore_case, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer input_redirection_id must be NULL.");
				assert_pointer_isNULL(tmp_command->input_redirection_id, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer next_pipelined_command must be NULL.");
				assert_pointer_isNULL(tmp_command->next_pipelined_command, __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command flag was_executed must be false.");
				assert_bool_false(tmp_command->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command must have 0 num_of_args.");
				assert_unsigned_integer_equal(tmp_command->num_of_args, 0,  __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command must have 0 num_of_output_redirection_ids.");
				assert_unsigned_integer_equal(tmp_command->num_of_output_redirection_ids, 0,  __LINE__, msg);

				P_delete_command_pipeline_linked_list(&current_command_pipeline_linked_list);
			}
            //############end################

            //Parse the token array: {ID("abc"), BACKGROUND_EXEC, EOF_TOKEN}
            current_token_array = LTAP{ LTPP_start{LTP{ID, "abc"}, LTP{BACKGROUND_EXEC}, LTP{EOF_TOKEN}}LTPP_end, 3};

            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_notIsNULL(current_command_pipeline_linked_list, __LINE__, msg);

			if(current_command_pipeline_linked_list)
			{
				//Num of command pipelines:
				snprintf(msg, MSG_SIZE + 1, "Expects 1 command pipeline");
				assert_unsigned_integer_equal(current_command_pipeline_linked_list->length, 1, __LINE__, msg);
				/*First pipeline*/
                tmp_node = current_command_pipeline_linked_list->head;
				snprintf(msg, MSG_SIZE + 1, "The first pipeline must have 1 command.");
				assert_unsigned_integer_equal(tmp_node->length, 1, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first pipeline must have 1 command remaining to execute.");
				assert_unsigned_integer_equal(tmp_node->remaining_to_execute, 1, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline flag was_executed must be false.");
				assert_bool_false(tmp_node->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline flag execute_in_background must be true.");
				assert_bool_true(tmp_node->execute_in_background, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline pointer next_pipeline must be NULL.");
				assert_pointer_isNULL(tmp_node->next_pipeline, __LINE__, msg);

                /*First command*/
                tmp_command = tmp_node->first_command;
				expected_value = "abc";
				snprintf(msg, MSG_SIZE + 1, "The first command must have the id equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->id, expected_value, ignore_case, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer input_redirection_id must be NULL.");
				assert_pointer_isNULL(tmp_command->input_redirection_id, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer next_pipelined_command must be NULL.");
				assert_pointer_isNULL(tmp_command->next_pipelined_command, __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command flag was_executed must be false.");
				assert_bool_false(tmp_command->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command must have 0 num_of_args.");
				assert_unsigned_integer_equal(tmp_command->num_of_args, 0,  __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command must have 0 num_of_output_redirection_ids.");
				assert_unsigned_integer_equal(tmp_command->num_of_output_redirection_ids, 0,  __LINE__, msg);

				P_delete_command_pipeline_linked_list(&current_command_pipeline_linked_list);
			}
            //############end################



            //Parse the token array: {COMMENT, EOF_TOKEN}
            current_token_array = LTAP{ LTPP_start{LTP{COMMENT}, LTP{EOF_TOKEN}}LTPP_end, 2};

            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_notIsNULL(current_command_pipeline_linked_list, __LINE__, msg);

			if(current_command_pipeline_linked_list)
			{
				//Num of command pipelines:
				snprintf(msg, MSG_SIZE + 1, "Expects 0 command pipeline");
				assert_unsigned_integer_equal(current_command_pipeline_linked_list->length, 0, __LINE__, msg);
                snprintf(msg, MSG_SIZE + 1, "Check if head is NULL.", current_str);
                assert_pointer_isNULL(current_command_pipeline_linked_list->head, __LINE__, msg);
                snprintf(msg, MSG_SIZE + 1, "Check if tail is NULL.", current_str);
                assert_pointer_isNULL(current_command_pipeline_linked_list->tail, __LINE__, msg);

				P_delete_command_pipeline_linked_list(&current_command_pipeline_linked_list);
			}
            //############end################
            
            //Parse the token array: {EOF_TOKEN, COMMENT}
            current_token_array = LTAP{ LTPP_start{LTP{EOF_TOKEN}, LTP{COMMENT}}LTPP_end, 2};

            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_notIsNULL(current_command_pipeline_linked_list, __LINE__, msg);

			if(current_command_pipeline_linked_list)
			{
				//Num of command pipelines:
				snprintf(msg, MSG_SIZE + 1, "Expects 0 command pipeline");
				assert_unsigned_integer_equal(current_command_pipeline_linked_list->length, 0, __LINE__, msg);
                snprintf(msg, MSG_SIZE + 1, "Check if head is NULL.", current_str);
                assert_pointer_isNULL(current_command_pipeline_linked_list->head, __LINE__, msg);
                snprintf(msg, MSG_SIZE + 1, "Check if tail is NULL.", current_str);
                assert_pointer_isNULL(current_command_pipeline_linked_list->tail, __LINE__, msg);

				P_delete_command_pipeline_linked_list(&current_command_pipeline_linked_list);
			}
            //############end################

            //Parse the token array: {EOF_TOKEN}
            current_token_array = LTAP{ LTPP_start{LTP{EOF_TOKEN}}LTPP_end, 1};

            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_notIsNULL(current_command_pipeline_linked_list, __LINE__, msg);

			if(current_command_pipeline_linked_list)
			{
				//Num of command pipelines:
				snprintf(msg, MSG_SIZE + 1, "Expects 0 command pipeline");
				assert_unsigned_integer_equal(current_command_pipeline_linked_list->length, 0, __LINE__, msg);
                snprintf(msg, MSG_SIZE + 1, "Check if head is NULL.", current_str);
                assert_pointer_isNULL(current_command_pipeline_linked_list->head, __LINE__, msg);
                snprintf(msg, MSG_SIZE + 1, "Check if tail is NULL.", current_str);
                assert_pointer_isNULL(current_command_pipeline_linked_list->tail, __LINE__, msg);

				P_delete_command_pipeline_linked_list(&current_command_pipeline_linked_list);
			}
            //############end################

		end_module();
		//------------------------------------------------------------------------------
		

		//------------------------------------------------------------------------------
		start_module("DOMAIN CASES", "Module for testing domain cases.", functions_tested);


            /*SYNTAX ERROR*/
            //Parse the token array: {ID("abc"), IN_REDIRECT, ID("abc.txt"), PIPE, ID("A") IN_REDIRECT, ID("B"), EOF_TOKEN}
            //Desc.: The array is not terminated by EOF_TOKEN. It should return syntax error.
            current_token_array = LTAP{ LTPP_start{
                LTP{ID, "abc"}, 
                LTP{IN_REDIRECT}, 
                LTP{ID, "abc.txt"},
                LTP{PIPE}, 
                LTP{ID, "A"}, 
                LTP{IN_REDIRECT}, 
                LTP{ID, "B"},
                LTP{EOF_TOKEN}
                                                  }LTPP_end, 8};

            error_msg[0] = '\0';
            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_isNULL(current_command_pipeline_linked_list, __LINE__, msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse changed error_msg.", current_str);
            assert_unsigned_integer_greater(strlen(error_msg), 0, __LINE__, msg);
            //############end################

            /*NOT SYNTAX ERROR*/
            //Parse the token array: {ID("abc"), IN_REDIRECT, ID("abc.txt"), PIPE, ID("A"), OUT_REDIRECT, ID("B"), ID("-arg1"), OUT_REDIRECT, ID("B"), ID("arg2"), BACKGROUND_EXEC, EOF_TOKEN}
            //Desc.: The array is not terminated by EOF_TOKEN. It should return syntax error.
            current_token_array = LTAP{ LTPP_start{
                LTP{ID, "abc"}, 
                LTP{IN_REDIRECT}, 
                LTP{ID, "abc.txt"},
                LTP{PIPE}, 
                LTP{ID, "A"}, 
                LTP{OUT_REDIRECT}, 
                LTP{ID, "B"},
                LTP{ID, "-arg1"}, 
                LTP{OUT_REDIRECT}, 
                LTP{ID, "B"},
                LTP{ID, "arg2"}, 
                LTP{BACKGROUND_EXEC},
                LTP{EOF_TOKEN}
                                                  }LTPP_end, 13};

            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_notIsNULL(current_command_pipeline_linked_list, __LINE__, msg);

			if(current_command_pipeline_linked_list)
			{
				//Num of command pipelines:
				snprintf(msg, MSG_SIZE + 1, "Expects 1 command pipeline");
				assert_unsigned_integer_equal(current_command_pipeline_linked_list->length, 1, __LINE__, msg);
				/*First pipeline*/
                tmp_node = current_command_pipeline_linked_list->head;
				snprintf(msg, MSG_SIZE + 1, "The first pipeline must have 2 command.");
				assert_unsigned_integer_equal(tmp_node->length, 2, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first pipeline must have 2 command remaining to execute.");
				assert_unsigned_integer_equal(tmp_node->remaining_to_execute, 2, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline flag was_executed must be false.");
				assert_bool_false(tmp_node->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline flag execute_in_background must be true.");
				assert_bool_true(tmp_node->execute_in_background, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline pointer next_pipeline must be NULL.");
				assert_pointer_isNULL(tmp_node->next_pipeline, __LINE__, msg);

                /*First command*/
                tmp_command = tmp_node->first_command;
				expected_value = "abc";
				snprintf(msg, MSG_SIZE + 1, "The first command must have the id equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->id, expected_value, ignore_case, __LINE__, msg);

				expected_value = "abc.txt";
				snprintf(msg, MSG_SIZE + 1, "The first command pointer input_redirection_id must have value equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->input_redirection_id, expected_value, ignore_case, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer next_pipelined_command must not be NULL.");
				assert_pointer_notIsNULL(tmp_command->next_pipelined_command, __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command flag was_executed must be false.");
				assert_bool_false(tmp_command->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command must have 0 num_of_args.");
				assert_unsigned_integer_equal(tmp_command->num_of_args, 0,  __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command must have 0 num_of_output_redirection_ids.");
				assert_unsigned_integer_equal(tmp_command->num_of_output_redirection_ids, 0,  __LINE__, msg);

                /*Second command*/
                tmp_command = tmp_node->first_command->next_pipelined_command;
				expected_value = "A";
				snprintf(msg, MSG_SIZE + 1, "The first command must have the id equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->id, expected_value, ignore_case, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer input_redirection_id must be NULL.");
				assert_pointer_isNULL(tmp_command->input_redirection_id, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer next_pipelined_command must be NULL.");
				assert_pointer_isNULL(tmp_command->next_pipelined_command, __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command flag was_executed must be false.");
				assert_bool_false(tmp_command->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command must have 2 num_of_args.");
				assert_unsigned_integer_equal(tmp_command->num_of_args, 2,  __LINE__, msg);
				expected_value = "-arg1"; 
				snprintf(msg, MSG_SIZE + 1, "The first arg must have the value equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->args_list[0], expected_value, ignore_case, __LINE__, msg);
				expected_value = "arg2"; 
				snprintf(msg, MSG_SIZE + 1, "The second arg must have the value equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->args_list[1], expected_value, ignore_case, __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command must have 2 num_of_output_redirection_ids.");
				assert_unsigned_integer_equal(tmp_command->num_of_output_redirection_ids, 2,  __LINE__, msg);
				expected_value = "B"; 
				snprintf(msg, MSG_SIZE + 1, "The first output redirection id must have the value equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->output_redirection_ids_list[0], expected_value, ignore_case, __LINE__, msg);

				expected_value = "B"; 
				snprintf(msg, MSG_SIZE + 1, "The second output redirection id must have the value equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->output_redirection_ids_list[1], expected_value, ignore_case, __LINE__, msg);


				P_delete_command_pipeline_linked_list(&current_command_pipeline_linked_list);
			}
            //############end################



		end_module();
		//------------------------------------------------------------------------------

		start_module("ARBITRARY CASES", "Module for testing arbitrary cases.", functions_tested);

            //Parse the token array: {ID("hello world"), EOF_TOKEN}
            current_token_array = LTAP{ LTPP_start{LTP{ID, "hello world"}, LTP{EOF_TOKEN}}LTPP_end, 2};

            current_command_pipeline_linked_list = P_parse(current_token_array, error_msg);
			snprintf(msg, MSG_SIZE + 1, "Check if P_parse returned NULL.", current_str);
			assert_pointer_notIsNULL(current_command_pipeline_linked_list, __LINE__, msg);

			if(current_command_pipeline_linked_list)
			{
				//Num of command pipelines:
				snprintf(msg, MSG_SIZE + 1, "Expects 1 command pipeline");
				assert_unsigned_integer_equal(current_command_pipeline_linked_list->length, 1, __LINE__, msg);
				/*First pipeline*/
                tmp_node = current_command_pipeline_linked_list->head;
				snprintf(msg, MSG_SIZE + 1, "The first pipeline must have 1 command.");
				assert_unsigned_integer_equal(tmp_node->length, 1, __LINE__, msg);
				snprintf(msg, MSG_SIZE + 1, "The first pipeline must have 1 command remaining to execute.");
				assert_unsigned_integer_equal(tmp_node->remaining_to_execute, 1, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline flag was_executed must be false.");
				assert_bool_false(tmp_node->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline flag execute_in_background must be false.");
				assert_bool_false(tmp_node->execute_in_background, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first pipeline pointer next_pipeline must be NULL.");
				assert_pointer_isNULL(tmp_node->next_pipeline, __LINE__, msg);

                /*First command*/
                tmp_command = tmp_node->first_command;
				expected_value = "hello world";
				snprintf(msg, MSG_SIZE + 1, "The first command must have the id equals to \"%s\".", expected_value);
				assert_string_equal(tmp_command->id, expected_value, ignore_case, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer input_redirection_id must be NULL.");
				assert_pointer_isNULL(tmp_command->input_redirection_id, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command pointer next_pipelined_command must be NULL.");
				assert_pointer_isNULL(tmp_command->next_pipelined_command, __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command flag was_executed must be false.");
				assert_bool_false(tmp_command->was_executed, __LINE__, msg);

				snprintf(msg, MSG_SIZE + 1, "The first command must have 0 num_of_args.");
				assert_unsigned_integer_equal(tmp_command->num_of_args, 0,  __LINE__, msg);
                
				snprintf(msg, MSG_SIZE + 1, "The first command must have 0 num_of_output_redirection_ids.");
				assert_unsigned_integer_equal(tmp_command->num_of_output_redirection_ids, 0,  __LINE__, msg);

				P_delete_command_pipeline_linked_list(&current_command_pipeline_linked_list);
			}



		end_module();
		//------------------------------------------------------------------------------


	end_suite();
	//------------------------------------------------------------------------------
}
