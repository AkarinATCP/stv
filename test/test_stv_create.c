/**
 * @file test_stv_create.c
 * @brief stv_new / stv_create
 */

#include "stv.h"
#include "unity/unity.h"

void test_stv_new_normal(void) {
    strview sv = stv_new("hello");
    TEST_ASSERT_EQUAL_STRING("hello", sv.data);
    TEST_ASSERT_EQUAL_size_t(5, sv.len);
}

void test_stv_new_empty(void) {
    strview sv = stv_new("");
    TEST_ASSERT_NOT_NULL(sv.data);
    TEST_ASSERT_EQUAL_size_t(0, sv.len);
}

void test_stv_new_null(void) {
    strview sv = stv_new(NULL);
    TEST_ASSERT_NULL(sv.data);
    TEST_ASSERT_EQUAL_size_t(0, sv.len);
}

void test_stv_create_stop_at_char(void) {
    strview sv = stv_create("abc:def", ':', 10);
    TEST_ASSERT_EQUAL_size_t(3, sv.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abc", sv.data, 3);
}

void test_stv_create_maxlen_cut(void) {
    strview sv = stv_create("abcdef", '\0', 3);
    TEST_ASSERT_EQUAL_size_t(3, sv.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abc", sv.data, 3);
}

void test_stv_create_null(void) {
    strview sv = stv_create(NULL, 'x', 10);
    TEST_ASSERT_NULL(sv.data);
    TEST_ASSERT_EQUAL_size_t(0, sv.len);
}

void run_create_tests(void) {
    RUN_TEST(test_stv_new_normal);
    RUN_TEST(test_stv_new_empty);
    RUN_TEST(test_stv_new_null);
    RUN_TEST(test_stv_create_stop_at_char);
    RUN_TEST(test_stv_create_maxlen_cut);
    RUN_TEST(test_stv_create_null);
}
