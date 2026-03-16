#include "test_runner.h"
#include <stdio.h>

int _tests_run = 0;
int _tests_passed = 0;
int _tests_failed = 0;

void run_ice_math_tests(void);
void run_memory_tests(void);
void run_parser_tests(void);
void run_nlg_tests(void);
void run_nlg_quality_tests(void);
void run_ordinal_matrix_tests(void);
void run_mcp_client_tests(void);

int main(void) {
    printf("Starting OrdAGI Test Suite...\n");
    run_ice_math_tests();
    run_memory_tests();
    run_parser_tests();
    run_nlg_tests();
    run_nlg_quality_tests();
    run_ordinal_matrix_tests();
    run_mcp_client_tests();
    PRINT_TEST_SUMMARY();
    return (_tests_failed > 0) ? 1 : 0;
}
