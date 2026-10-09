/**
 * @file test_stv_count_predicate.c
 * @brief test Count, Predicates, conntains, and start/ends with
 */

#include "stv.h"
#include "unity/unity.h"
#include <ctype.h>

/* ========== count ========== */
void test_countIf_digits(void) {
    strview sv = stv_literal("abc123def456");
    TEST_ASSERT_EQUAL_size_t(6, stv_countIf(sv, isdigit));
}

void test_countIf_empty(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_countIf(stv_nullstv, isdigit));
}

void test_countIf_null_handle(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_countIf(stv_literal("abc"), NULL));
}

void test_countChar_normal(void) {
    strview sv = stv_literal("hello world");
    TEST_ASSERT_EQUAL_size_t(3, stv_countCh(sv, 'l'));
}

void test_countChar_empty(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_countCh(stv_nullstv, 'a'));
}

void test_countSubstr_normal(void) {
    strview sv  = stv_literal("abcabcdeabcabed");
    strview sub = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(3, stv_countSubstr(sv, sub));
}

void test_countSubstr_empty(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(3, stv_countSubstr(sv, stv_nullstv));
}

void test_countSubstr_empty_source(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_countSubstr(stv_nullstv, stv_literal("a")));
}

/* ========== every / some ========== */
void test_everyIf_digit(void) {
    TEST_ASSERT_TRUE(stv_everyIf(stv_literal("12345"), isdigit));
}

void test_everyIf_not_all_digit(void) {
    TEST_ASSERT_FALSE(stv_everyIf(stv_literal("123a"), isdigit));
}

void test_everyIf_empty(void) {
    TEST_ASSERT_FALSE(stv_everyIf(stv_nullstv, isdigit));
}

void test_everyChar_true(void) {
    TEST_ASSERT_TRUE(stv_everyCh(stv_literal("aaaa"), 'a'));
}

void test_everyChar_false(void) {
    TEST_ASSERT_FALSE(stv_everyCh(stv_literal("aaab"), 'a'));
}

void test_everyChar_empty(void) {
    TEST_ASSERT_FALSE(stv_everyCh(stv_nullstv, 'x'));
}

void test_someIf_digit(void) {
    TEST_ASSERT_TRUE(stv_someIf(stv_literal("abc1xyz"), isdigit));
}

void test_someIf_no_digit(void) {
    TEST_ASSERT_FALSE(stv_someIf(stv_literal("abcdef"), isdigit));
}

void test_someIf_empty(void) {
    TEST_ASSERT_FALSE(stv_someIf(stv_nullstv, isdigit));
}

void test_someChar_found(void) {
    TEST_ASSERT_TRUE(stv_someCh(stv_literal("hello"), 'e'));
}

void test_someChar_not_found(void) {
    TEST_ASSERT_FALSE(stv_someCh(stv_literal("hello"), 'x'));
}

void test_someChar_empty(void) {
    TEST_ASSERT_FALSE(stv_someCh(stv_nullstv, 'a'));
}

/* ========== startsWith / endsWith / contains ========== */
void test_startsWith_true(void) {
    strview text = stv_literal("Hello World!");
    strview pat  = stv_literal("Hello");
    TEST_ASSERT_TRUE(stv_startsWith(text, pat, false));
    TEST_ASSERT_TRUE(stv_startsWith(text, stv_literal("hello"), true));
}

void test_startsWith_false(void) {
    strview text = stv_literal("Hello");
    strview pat  = stv_literal("World");
    TEST_ASSERT_FALSE(stv_startsWith(text, pat, false));
    TEST_ASSERT_FALSE(stv_startsWith(text, pat, true));
}

void test_startsWith_empty_pat(void) {
    TEST_ASSERT_TRUE(stv_startsWith(stv_literal("abc"), stv_nullstv, false));
    TEST_ASSERT_TRUE(stv_startsWith(stv_literal("abc"), stv_nullstv, true));
}

void test_endsWith(void) {
    strview text = stv_literal("document.txt");
    strview pat  = stv_literal(".txt");
    TEST_ASSERT_TRUE(stv_endsWith(text, pat, false));
    TEST_ASSERT_TRUE(stv_endsWith(text, stv_literal(".TXT"), true));
    TEST_ASSERT_FALSE(stv_endsWith(text, stv_literal(".doc"), false));
}

void test_endsWith_empty(void) {
    TEST_ASSERT_TRUE(stv_endsWith(stv_literal("any"), stv_nullstv, false));
    TEST_ASSERT_TRUE(stv_endsWith(stv_literal("any"), stv_nullstv, true));
}

void test_contains(void) {
    strview text = stv_literal("the quick brown fox");
    TEST_ASSERT_TRUE(stv_contains(text, stv_literal("quick"), false));
    TEST_ASSERT_TRUE(stv_contains(text, stv_literal("QUICK"), true));
    TEST_ASSERT_FALSE(stv_contains(text, stv_literal("slow"), false));
}

void test_contains_empty_pat(void) {
    TEST_ASSERT_TRUE(stv_contains(stv_literal("abc"), stv_nullstv, false));
    TEST_ASSERT_TRUE(stv_contains(stv_literal("abc"), stv_nullstv, true));
}

/* ========== generic macro (C11) ========== */
#if defined(LIB_STV_GENERIC)
void test_stv_count_char(void) {
    size_t cnt = stv_count(stv_literal("hello"), 'l');
    TEST_ASSERT_EQUAL_size_t(2, cnt);
}

void test_stv_count_if(void) {
    size_t cnt = stv_count(stv_literal("abc123"), isdigit);
    TEST_ASSERT_EQUAL_size_t(3, cnt);
}

void test_stv_every_macro(void) {
    TEST_ASSERT_TRUE(stv_every(stv_literal("abc"), isalpha));
    TEST_ASSERT_FALSE(stv_every(stv_literal("abc1"), isalpha));
}

void test_stv_some_macro(void) {
    TEST_ASSERT_TRUE(stv_some(stv_literal("abc1"), isdigit));
    TEST_ASSERT_FALSE(stv_some(stv_literal("abc"), isdigit));
}
#endif

void run_count_predicate_tests(void) {
    RUN_TEST(test_countIf_digits);
    RUN_TEST(test_countIf_empty);
    RUN_TEST(test_countIf_null_handle);
    RUN_TEST(test_countChar_normal);
    RUN_TEST(test_countChar_empty);
    RUN_TEST(test_countSubstr_normal);
    RUN_TEST(test_countSubstr_empty);
    RUN_TEST(test_countSubstr_empty_source);
    RUN_TEST(test_everyIf_digit);
    RUN_TEST(test_everyIf_not_all_digit);
    RUN_TEST(test_everyIf_empty);
    RUN_TEST(test_everyChar_true);
    RUN_TEST(test_everyChar_false);
    RUN_TEST(test_everyChar_empty);
    RUN_TEST(test_someIf_digit);
    RUN_TEST(test_someIf_no_digit);
    RUN_TEST(test_someIf_empty);
    RUN_TEST(test_someChar_found);
    RUN_TEST(test_someChar_not_found);
    RUN_TEST(test_someChar_empty);
    RUN_TEST(test_startsWith_true);
    RUN_TEST(test_startsWith_false);
    RUN_TEST(test_startsWith_empty_pat);
    RUN_TEST(test_endsWith);
    RUN_TEST(test_endsWith_empty);
    RUN_TEST(test_contains);
    RUN_TEST(test_contains_empty_pat);
#if defined(LIB_STV_GENERIC)
    RUN_TEST(test_stv_count_char);
    RUN_TEST(test_stv_count_if);
    RUN_TEST(test_stv_every_macro);
    RUN_TEST(test_stv_some_macro);
#endif
}
