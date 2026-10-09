/**
 * @file test_stv_slice.c
 * @brief test Slicing
 */

#include "stv.h"
#include "unity/unity.h"

/* ========== stv_slice ========== */
void test_stv_slice_full_range(void) {
    strview sv = stv_literal("Hello");
    strview s  = stv_slice(sv, stv_begin, stv_end);
    TEST_ASSERT_TRUE(stv_equal(sv, s));
}

void test_stv_slice_begin(void) {
    strview sv = stv_literal("Hello");
    strview s  = stv_slice(sv, stv_begin, 3);
    TEST_ASSERT_EQUAL_size_t(3, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("Hel", s.data, 3);
}

void test_stv_slice_mid(void) {
    strview sv = stv_literal("Hello");
    strview s  = stv_slice(sv, 1, 4);
    TEST_ASSERT_EQUAL_size_t(3, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ell", s.data, 3);
}

void test_stv_slice_end(void) {
    strview sv = stv_literal("Hello");
    strview s  = stv_slice(sv, 2, stv_end);
    TEST_ASSERT_EQUAL_size_t(3, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("llo", s.data, 3);
}

void test_stv_slice_empty_result(void) {
    strview sv = stv_literal("Hello");
    strview s  = stv_slice(sv, 2, 2);
    TEST_ASSERT_EQUAL_size_t(0, s.len);
    TEST_ASSERT_TRUE(stv_empty(s));
}

void test_stv_slice_out_of_range(void) {
    strview sv = stv_literal("Hi");
    TEST_ASSERT_TRUE(stv_empty(stv_slice(sv, 0, 10)));
    TEST_ASSERT_TRUE(stv_empty(stv_slice(sv, 3, 4)));
    TEST_ASSERT_TRUE(stv_empty(stv_slice(sv, 2, 1)));
}

void test_stv_slice_null_view(void) {
    strview s = stv_slice(stv_nullstv, 0, 5);
    TEST_ASSERT_TRUE(stv_empty(s));
}

/* ========== stv_removeStart / stv_removeEnd ========== */
void test_stv_removeStart_normal(void) {
    strview s = stv_removeStart(stv_literal("Hello World"), 6);
    TEST_ASSERT_EQUAL_size_t(5, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("World", s.data, 5);
}

void test_stv_removeStart_zero(void) {
    strview sv = stv_literal("Hello");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_removeStart(sv, 0)));
}

void test_stv_removeStart_exact_length(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_removeStart(stv_literal("abc"), 3)));
}

void test_stv_removeStart_exceeds_length(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_removeStart(stv_literal("abc"), 10)));
}

void test_stv_removeStart_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_removeStart(stv_nullstv, 5)));
}

void test_stv_removeEnd_normal(void) {
    strview s = stv_removeEnd(stv_literal("Hello World"), 6);
    TEST_ASSERT_EQUAL_size_t(5, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("Hello", s.data, 5);
}

void test_stv_removeEnd_zero(void) {
    strview sv = stv_literal("Hello");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_removeEnd(sv, 0)));
}

void test_stv_removeEnd_exact_length(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_removeEnd(stv_literal("abc"), 3)));
}

void test_stv_removeEnd_exceeds_length(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_removeEnd(stv_literal("abc"), 5)));
}

void test_stv_removeEnd_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_removeEnd(stv_nullstv, 2)));
}

/* ========== removePrefix / removeSuffix ========== */
void test_stv_removePrefix_match(void) {
    strview sv     = stv_literal("http://example.com");
    strview prefix = stv_literal("http://");
    strview result = stv_removePrefix(sv, prefix, false);
    TEST_ASSERT_EQUAL_size_t(11, result.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("example.com", result.data, 11);
}

void test_stv_removePrefix_no_match(void) {
    strview sv     = stv_literal("ftp://example.com");
    strview prefix = stv_literal("http://");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_removePrefix(sv, prefix, false)));
}

void test_stv_removePrefix_empty_prefix(void) {
    strview sv = stv_literal("hello");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_removePrefix(sv, stv_nullstv, false)));
}

void test_stv_removePrefix_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_removePrefix(stv_nullstv, stv_literal("a"), false)));
}

void test_stv_removeSuffix_match(void) {
    strview sv     = stv_literal("document.txt");
    strview suffix = stv_literal(".txt");
    strview result = stv_removeSuffix(sv, suffix, false);
    TEST_ASSERT_EQUAL_size_t(8, result.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("document", result.data, 8);
}

void test_stv_removeSuffix_no_match(void) {
    strview sv     = stv_literal("document.md");
    strview suffix = stv_literal(".txt");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_removeSuffix(sv, suffix, false)));
}

void test_stv_removeSuffix_empty_suffix(void) {
    strview sv = stv_literal("hello");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_removeSuffix(sv, stv_nullstv, false)));
}

void test_stv_removeSuffix_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_removeSuffix(stv_nullstv, stv_literal("a"), false)));
}

/* ========== nocase 变体 ========== */
void test_stv_removePrefix_nocase_match(void) {
    strview result = stv_removePrefix(stv_literal("HTTP://example.com"), stv_literal("http://"), true);
    TEST_ASSERT_EQUAL_size_t(11, result.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("example.com", result.data, 11);
}

void test_stv_removePrefix_nocase_no_match(void) {
    strview sv     = stv_literal("ftp://example.com");
    strview prefix = stv_literal("http://");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_removePrefix(sv, prefix, true)));
}

void test_stv_removeSuffix_nocase_match(void) {
    strview result = stv_removeSuffix(stv_literal("document.TXT"), stv_literal(".txt"), true);
    TEST_ASSERT_EQUAL_size_t(8, result.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("document", result.data, 8);
}

void test_stv_removeSuffix_nocase_no_match(void) {
    strview sv     = stv_literal("document.md");
    strview suffix = stv_literal(".txt");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_removeSuffix(sv, suffix, true)));
}

void run_slice_tests(void) {
    RUN_TEST(test_stv_slice_full_range);
    RUN_TEST(test_stv_slice_begin);
    RUN_TEST(test_stv_slice_mid);
    RUN_TEST(test_stv_slice_end);
    RUN_TEST(test_stv_slice_empty_result);
    RUN_TEST(test_stv_slice_out_of_range);
    RUN_TEST(test_stv_slice_null_view);
    RUN_TEST(test_stv_removeStart_normal);
    RUN_TEST(test_stv_removeStart_zero);
    RUN_TEST(test_stv_removeStart_exact_length);
    RUN_TEST(test_stv_removeStart_exceeds_length);
    RUN_TEST(test_stv_removeStart_empty_view);
    RUN_TEST(test_stv_removeEnd_normal);
    RUN_TEST(test_stv_removeEnd_zero);
    RUN_TEST(test_stv_removeEnd_exact_length);
    RUN_TEST(test_stv_removeEnd_exceeds_length);
    RUN_TEST(test_stv_removeEnd_empty_view);
    RUN_TEST(test_stv_removePrefix_match);
    RUN_TEST(test_stv_removePrefix_no_match);
    RUN_TEST(test_stv_removePrefix_empty_prefix);
    RUN_TEST(test_stv_removePrefix_empty_view);
    RUN_TEST(test_stv_removeSuffix_match);
    RUN_TEST(test_stv_removeSuffix_no_match);
    RUN_TEST(test_stv_removeSuffix_empty_suffix);
    RUN_TEST(test_stv_removeSuffix_empty_view);
    RUN_TEST(test_stv_removePrefix_nocase_match);
    RUN_TEST(test_stv_removePrefix_nocase_no_match);
    RUN_TEST(test_stv_removeSuffix_nocase_match);
    RUN_TEST(test_stv_removeSuffix_nocase_no_match);
}
