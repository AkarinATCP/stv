/**
 * @file test_stv_utils.c
 * @brief test Utilities
 */

#include "stv.h"
#include "unity/unity.h"
#include <stdio.h>

/* ========== front / back / at ========== */
void test_front_back(void) {
    strview sv = stv_literal("Hello");
    TEST_ASSERT_EQUAL_CHAR('H', stv_front(sv));
    TEST_ASSERT_EQUAL_CHAR('o', stv_back(sv));
}

void test_front_back_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_CHAR('\0', stv_front(sv));
    TEST_ASSERT_EQUAL_CHAR('\0', stv_back(sv));
}

void test_at_valid(void) {
    strview sv = stv_literal("Hello");
    TEST_ASSERT_EQUAL_CHAR('H', stv_at(sv, 0));
    TEST_ASSERT_EQUAL_CHAR('o', stv_at(sv, 4));
}

void test_at_out_of_bounds(void) {
    strview sv = stv_literal("Hi");
    TEST_ASSERT_EQUAL_CHAR('\0', stv_at(sv, 2));
    TEST_ASSERT_EQUAL_CHAR('\0', stv_at(sv, 100));
}

void test_at_empty(void) {
    TEST_ASSERT_EQUAL_CHAR('\0', stv_at(stv_nullstv, 0));
}

/* ========== stv_swap ========== */
void test_swap(void) {
    strview a = stv_literal("first");
    strview b = stv_literal("second");
    stv_swap(&a, &b);
    TEST_ASSERT_EQUAL_STRING("second", a.data);
    TEST_ASSERT_EQUAL_size_t(6, a.len);
    TEST_ASSERT_EQUAL_STRING("first", b.data);
    TEST_ASSERT_EQUAL_size_t(5, b.len);
}

void test_swap_null_pointers(void) {
    strview a = stv_literal("a");
    stv_swap(&a, NULL);
    TEST_ASSERT_EQUAL_STRING("a", a.data);
}

/* ========== stv_hash ========== */
void test_hash_empty(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_hash(stv_nullstv));
    TEST_ASSERT_EQUAL_size_t(0, stv_hash_FNV1a(stv_nullstv));
}

void test_hash_nonempty(void) {
    strview sv = stv_literal("hello");
    size_t  h  = stv_hash(sv);
    TEST_ASSERT_NOT_EQUAL(0, h);
    TEST_ASSERT_EQUAL_size_t(h, stv_hash_FNV1a(sv));
}

void test_hash_consistent(void) {
    strview a = stv_literal("test");
    strview b = stv_literal("test");
    TEST_ASSERT_EQUAL_size_t(stv_hash(a), stv_hash(b));
}

void test_hash_different(void) {
    strview a = stv_literal("abc");
    strview b = stv_literal("xyz");
    TEST_ASSERT_TRUE(stv_hash(a) != stv_hash(b) || stv_equal(a, b));
}

/* ========== stv_forEach / stv_forEachRev ========== */
static int     foreach_call_count;
static char    foreach_last_char;
static size_t  foreach_last_index;
static strview foreach_last_view;

static void foreach_test_callback(char ch, size_t idx, strview ctx) {
    foreach_call_count++;
    foreach_last_char  = ch;
    foreach_last_index = idx;
    foreach_last_view  = ctx;
}

void test_forEach_empty(void) {
    foreach_call_count = -1;
    stv_forEach(stv_nullstv, foreach_test_callback);
    TEST_ASSERT_EQUAL_INT(-1, foreach_call_count);
}

void test_forEach_normal(void) {
    strview sv         = stv_literal("abc");
    foreach_call_count = 0;
    stv_forEach(sv, foreach_test_callback);
    TEST_ASSERT_EQUAL_INT(3, foreach_call_count);
    TEST_ASSERT_EQUAL_CHAR('c', foreach_last_char);
    TEST_ASSERT_EQUAL_size_t(2, foreach_last_index);
    TEST_ASSERT_TRUE(stv_equal(sv, foreach_last_view));
}

void test_forEach_single_char(void) {
    strview sv         = stv_literal("X");
    foreach_call_count = 0;
    stv_forEach(sv, foreach_test_callback);
    TEST_ASSERT_EQUAL_INT(1, foreach_call_count);
    TEST_ASSERT_EQUAL_CHAR('X', foreach_last_char);
    TEST_ASSERT_EQUAL_size_t(0, foreach_last_index);
}

void test_forEach_does_not_modify_source(void) {
    strview sv = stv_literal("hello");
    stv_forEach(sv, foreach_test_callback);
    TEST_ASSERT_EQUAL_STRING("hello", sv.data);
    TEST_ASSERT_EQUAL_size_t(5, sv.len);
}

void test_forEachRev_normal(void) {
    strview sv         = stv_literal("abc");
    foreach_call_count = 0;
    stv_forEachRev(sv, foreach_test_callback);
    TEST_ASSERT_EQUAL_INT(3, foreach_call_count);
    TEST_ASSERT_EQUAL_CHAR('a', foreach_last_char);
    TEST_ASSERT_EQUAL_size_t(0, foreach_last_index);
}

void test_forEachRev_empty(void) {
    foreach_call_count = -1;
    stv_forEachRev(stv_nullstv, foreach_test_callback);
    TEST_ASSERT_EQUAL_INT(-1, foreach_call_count);
}

void test_forEachRev_null_callback(void) {
    strview sv = stv_literal("abc");
    stv_forEachRev(sv, NULL);
    TEST_ASSERT_TRUE(true);
}

/* ========== stv_PFFMT / stv_PFARG ========== */
void test_printf_macro(void) {
    strview sv = stv_literal("test");
    char    buf[20];
    sprintf(buf, "[" stv_PFFMT "]", stv_PFARG(sv));
    TEST_ASSERT_EQUAL_STRING("[test]", buf);
}

void test_printf_macro_empty(void) {
    strview sv = stv_nullstv;
    char    buf[20];
    sprintf(buf, "[" stv_PFFMT "]", stv_PFARG(sv));
    TEST_ASSERT_EQUAL_STRING("[]", buf);
}

/* ========== stv_same / stv_empty ========== */
void test_same(void) {
    char    buf1[] = "abc";
    char    buf2[] = "abc";
    strview a      = stv_makestv(buf1, 3);
    strview b      = stv_makestv(buf1, 3);
    strview c      = stv_makestv(buf2, 3);
    TEST_ASSERT_TRUE(stv_same(a, b));
    TEST_ASSERT_FALSE(stv_same(a, c));
}

void test_empty(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_nullstv));
    TEST_ASSERT_TRUE(stv_empty(stv_literal("")));
    TEST_ASSERT_FALSE(stv_empty(stv_literal("a")));
}

void run_utils_tests(void) {
    RUN_TEST(test_front_back);
    RUN_TEST(test_front_back_empty);
    RUN_TEST(test_at_valid);
    RUN_TEST(test_at_out_of_bounds);
    RUN_TEST(test_at_empty);
    RUN_TEST(test_swap);
    RUN_TEST(test_swap_null_pointers);
    RUN_TEST(test_hash_empty);
    RUN_TEST(test_hash_nonempty);
    RUN_TEST(test_hash_consistent);
    RUN_TEST(test_hash_different);
    RUN_TEST(test_forEach_empty);
    RUN_TEST(test_forEach_normal);
    RUN_TEST(test_forEach_single_char);
    RUN_TEST(test_forEach_does_not_modify_source);
    RUN_TEST(test_forEachRev_normal);
    RUN_TEST(test_forEachRev_empty);
    RUN_TEST(test_forEachRev_null_callback);
    RUN_TEST(test_printf_macro);
    RUN_TEST(test_printf_macro_empty);
    RUN_TEST(test_same);
    RUN_TEST(test_empty);
}
