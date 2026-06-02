/**
 * @file test_stv_compare_diff.c
 * @brief test compare, diff, and equal
 */

#include "stv.h"
#include "unity/unity.h"

/* ========== stv_compare / stv_compareNocase ========== */
void test_compare_equal(void) {
    TEST_ASSERT_EQUAL_INT(0, stv_compare(stv_literal("abc"), stv_literal("abc")));
}

void test_compare_less(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_literal("abc"), stv_literal("abd")) < 0);
}

void test_compare_greater(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_literal("abd"), stv_literal("abc")) > 0);
}

void test_compare_shorter(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_literal("ab"), stv_literal("abc")) < 0);
}

void test_compareNocase_equal(void) {
    TEST_ASSERT_EQUAL_INT(0, stv_compareNocase(stv_literal("abc"), stv_literal("ABC")));
    TEST_ASSERT_EQUAL_INT(0, stv_compareNocase(stv_literal("Hello"), stv_literal("hello")));
}

void test_compareNocase_less(void) {
    TEST_ASSERT_TRUE(stv_compareNocase(stv_literal("abc"), stv_literal("ABD")) < 0);
    TEST_ASSERT_TRUE(stv_compareNocase(stv_literal("a"), stv_literal("B")) < 0);
}

void test_compareNocase_greater(void) {
    TEST_ASSERT_TRUE(stv_compareNocase(stv_literal("abd"), stv_literal("ABC")) > 0);
}

void test_compareNocase_shorter_prefix(void) {
    TEST_ASSERT_TRUE(stv_compareNocase(stv_literal("ab"), stv_literal("ABC")) < 0);
}

void test_compareNocase_empty(void) {
    TEST_ASSERT_TRUE(stv_compareNocase(stv_nullstv, stv_literal("a")) < 0);
    TEST_ASSERT_TRUE(stv_compareNocase(stv_literal("a"), stv_nullstv) > 0);
    TEST_ASSERT_EQUAL_INT(0, stv_compareNocase(stv_nullstv, stv_nullstv));
}

/* ========== stv_firstDiff / stv_lastDiff ========== */
void test_firstDiff_same(void) {
    strview a = stv_literal("abc");
    strview b = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstDiff(a, b, false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstDiff(a, b, true));
}

void test_firstDiff_at_start(void) {
    strview a = stv_literal("abc");
    strview b = stv_literal("xbc");
    TEST_ASSERT_EQUAL_size_t(0, stv_firstDiff(a, b, false));
}

void test_firstDiff_length_mismatch(void) {
    strview a = stv_literal("abc");
    strview b = stv_literal("ab");
    TEST_ASSERT_EQUAL_size_t(2, stv_firstDiff(a, b, false));
}

void test_firstDiff_nocase(void) {
    strview a = stv_literal("Hello");
    strview b = stv_literal("hello");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstDiff(a, b, true));
    TEST_ASSERT_EQUAL_size_t(0, stv_firstDiff(a, b, false));
}

void test_lastDiff(void) {
    strview a = stv_literal("hello");
    strview b = stv_literal("hallo");
    TEST_ASSERT_EQUAL_size_t(1, stv_lastDiff(a, b, false));
}

void test_lastDiff_nocase(void) {
    strview a = stv_literal("hello");
    strview b = stv_literal("HELLO");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastDiff(a, b, true));
    TEST_ASSERT_EQUAL_size_t(4, stv_lastDiff(a, b, false));
}

void test_lastDiff_length_mismatch(void) {
    strview a = stv_literal("abc");
    strview b = stv_literal("abcd");
    TEST_ASSERT_EQUAL_size_t(3, stv_lastDiff(a, b, false));
}

void test_diff_with_empty(void) {
    strview a = stv_literal("abc");
    strview e = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(0, stv_firstDiff(a, e, false));
    TEST_ASSERT_EQUAL_size_t(2, stv_lastDiff(a, e, false));
}

void test_diff_both_empty(void) {
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstDiff(stv_nullstv, stv_nullstv, false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastDiff(stv_nullstv, stv_nullstv, false));
}

/* ========== stv_equal / stv_equalNocase ========== */
void test_equal(void) {
    TEST_ASSERT_TRUE(stv_equal(stv_literal("abc"), stv_literal("abc")));
    TEST_ASSERT_FALSE(stv_equal(stv_literal("abc"), stv_literal("ab")));
}

void test_equalNocase(void) {
    TEST_ASSERT_TRUE(stv_equalNocase(stv_literal("abc"), stv_literal("ABC")));
    TEST_ASSERT_FALSE(stv_equalNocase(stv_literal("abc"), stv_literal("abd")));
}

void run_compare_diff_tests(void) {
    RUN_TEST(test_compare_equal);
    RUN_TEST(test_compare_less);
    RUN_TEST(test_compare_greater);
    RUN_TEST(test_compare_shorter);
    RUN_TEST(test_compareNocase_equal);
    RUN_TEST(test_compareNocase_less);
    RUN_TEST(test_compareNocase_greater);
    RUN_TEST(test_compareNocase_shorter_prefix);
    RUN_TEST(test_compareNocase_empty);
    RUN_TEST(test_firstDiff_same);
    RUN_TEST(test_firstDiff_at_start);
    RUN_TEST(test_firstDiff_length_mismatch);
    RUN_TEST(test_firstDiff_nocase);
    RUN_TEST(test_lastDiff);
    RUN_TEST(test_lastDiff_nocase);
    RUN_TEST(test_lastDiff_length_mismatch);
    RUN_TEST(test_diff_with_empty);
    RUN_TEST(test_diff_both_empty);
    RUN_TEST(test_equal);
    RUN_TEST(test_equalNocase);
}
