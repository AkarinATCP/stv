/**
 * @file test_stv.c
 * @brief stv.h
 */

#define LIB_STV_IMPL
#include "stv.h"
#include "unity/unity.h"

extern void run_create_tests(void);
extern void run_slice_tests(void);
extern void run_split_tests(void);
extern void run_trim_tests(void);
extern void run_search_tests(void);
extern void run_compare_diff_tests(void);
extern void run_count_predicate_tests(void);
extern void run_utils_tests(void);
extern void run_cstr_tests(void);
extern void run_numeric_tests(void);

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    run_create_tests();
    run_slice_tests();
    run_split_tests();
    run_trim_tests();
    run_search_tests();
    run_compare_diff_tests();
    run_count_predicate_tests();
    run_utils_tests();
    run_cstr_tests();
    run_numeric_tests();
    UNITY_END();
}
