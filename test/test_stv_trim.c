/**
 * @file test_stv_trim.c
 * @brief test Trimming
 */

#include "stv.h"
#include "unity/unity.h"
#include <ctype.h>

/* ========== trim charset ========== */
void test_trimChs_both(void) {
    strview trimmed = stv_trimChs(stv_literal("  \t  hello \t "), stv_whitespace);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_trimChs_start(void) {
    strview trimmed = stv_trimStartChs(stv_literal("///path//"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(6, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("path//", trimmed.data, 6);
}

void test_trimChs_end(void) {
    strview trimmed = stv_trimEndChs(stv_literal("hello---"), stv_literal("-"));
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_trimChs_no_op(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_trimChs(sv, stv_nullstv)));
}

void test_trimChs_all_removed(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_trimChs(stv_literal("xxxx"), stv_literal("x")).len);
}

void test_trimChs_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_trimChs(stv_nullstv, stv_whitespace)));
}

/* ========== trim charclass ========== */
void test_trimIf_both(void) {
    strview trimmed = stv_trimIf(stv_literal("  \t  hello \t "), isspace);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_trimIf_null_handle(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_trimIf(sv, NULL)));
}

void test_trimIf_all_removed(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_trimIf(stv_literal(" \t\n\r"), isspace).len);
}

void test_trimIf_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_trimIf(stv_nullstv, isspace)));
}

void test_trimStartIf(void) {
    strview trimmed = stv_trimStartIf(stv_literal("   text"), isspace);
    TEST_ASSERT_EQUAL_size_t(4, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("text", trimmed.data, 4);
}

void test_trimStartIf_null_handle(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_trimStartIf(sv, NULL)));
}

void test_trimStartIf_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_trimStartIf(stv_nullstv, isspace)));
}

void test_trimEndIf(void) {
    strview trimmed = stv_trimEndIf(stv_literal("text   "), isspace);
    TEST_ASSERT_EQUAL_size_t(4, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("text", trimmed.data, 4);
}

void test_trimEndIf_null_handle(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_trimEndIf(sv, NULL)));
}

void test_trimEndIf_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_trimEndIf(stv_nullstv, isspace)));
}

/* ========== generic marco (C11) ========== */
#if defined(LIB_STV_GENERIC)
void test_stv_trim_macro_charset(void) {
    strview trimmed = stv_trim(stv_literal("  hi  "), stv_whitespace);
    TEST_ASSERT_EQUAL_size_t(2, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hi", trimmed.data, 2);
}

void test_stv_trim_macro_class(void) {
    strview trimmed = stv_trim(stv_literal("  123  "), isspace);
    TEST_ASSERT_EQUAL_size_t(3, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("123", trimmed.data, 3);
}

void test_stv_trimStart_macro(void) {
    strview trimmed = stv_trimStart(stv_literal("///path"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(4, trimmed.len);
}

void test_stv_trimEnd_macro(void) {
    strview trimmed = stv_trimEnd(stv_literal("path---"), stv_literal("-"));
    TEST_ASSERT_EQUAL_size_t(4, trimmed.len);
}
#endif

void run_trim_tests(void) {
    RUN_TEST(test_trimChs_both);
    RUN_TEST(test_trimChs_start);
    RUN_TEST(test_trimChs_end);
    RUN_TEST(test_trimChs_no_op);
    RUN_TEST(test_trimChs_all_removed);
    RUN_TEST(test_trimChs_empty_view);
    RUN_TEST(test_trimIf_both);
    RUN_TEST(test_trimIf_null_handle);
    RUN_TEST(test_trimIf_all_removed);
    RUN_TEST(test_trimIf_empty_view);
    RUN_TEST(test_trimStartIf);
    RUN_TEST(test_trimStartIf_null_handle);
    RUN_TEST(test_trimStartIf_empty_view);
    RUN_TEST(test_trimEndIf);
    RUN_TEST(test_trimEndIf_null_handle);
    RUN_TEST(test_trimEndIf_empty_view);
#if defined(LIB_STV_GENERIC)
    RUN_TEST(test_stv_trim_macro_charset);
    RUN_TEST(test_stv_trim_macro_class);
    RUN_TEST(test_stv_trimStart_macro);
    RUN_TEST(test_stv_trimEnd_macro);
#endif
}
