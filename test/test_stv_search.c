/**
 * @file test_stv_search.c
 * @brief test Search
 */

#include "stv.h"
#include "unity/unity.h"
#include <ctype.h>

/* ========== first/last char ========== */
void test_firstChar_found(void) {
    strview sv = stv_literal("hello");
    TEST_ASSERT_EQUAL_size_t(0, stv_firstCh(sv, 'h', false));
    TEST_ASSERT_EQUAL_size_t(4, stv_firstCh(sv, 'o', false));
}

void test_firstChar_not_found(void) {
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstCh(stv_literal("abc"), 'x', false));
}

void test_firstNotChar(void) {
    strview sv = stv_literal("aaabc");
    TEST_ASSERT_EQUAL_size_t(3, stv_firstCh(sv, 'a', true));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstCh(stv_literal("aaa"), 'a', true));
}

void test_lastChar(void) {
    strview sv = stv_literal("abracadabra");
    TEST_ASSERT_EQUAL_size_t(10, stv_lastCh(sv, 'a', false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastCh(sv, 'z', false));
}

void test_lastNotChar(void) {
    strview sv = stv_literal("hello---");
    TEST_ASSERT_EQUAL_size_t(4, stv_lastCh(sv, '-', true));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastCh(stv_literal("---"), '-', true));
}

void test_char_empty_view(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstCh(sv, 'x', false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstCh(sv, 'x', true));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastCh(sv, 'x', false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastCh(sv, 'x', true));
}

/* ========== first/last charset ========== */
void test_firstCharset_found(void) {
    strview sv = stv_literal("abc123");
    TEST_ASSERT_EQUAL_size_t(3, stv_firstChs(sv, stv_literal("0123456789"), false));
    TEST_ASSERT_EQUAL_size_t(0, stv_firstChs(sv, stv_literal("0123456789"), true));
}

void test_firstCharset_not_found(void) {
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChs(stv_literal("abc"), stv_literal("123"), false));
}

void test_lastCharset(void) {
    strview sv = stv_literal("abc123def456");
    TEST_ASSERT_EQUAL_size_t(11, stv_lastChs(sv, stv_literal("0123456789"), false));
    TEST_ASSERT_EQUAL_size_t(8, stv_lastChs(sv, stv_literal("0123456789"), true));
}

void test_charset_empty_view(void) {
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChs(stv_nullstv, stv_whitespace, false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastChs(stv_nullstv, stv_whitespace, false));
}

/* ========== first/last charclass ========== */
void test_firstCharClass_digit(void) {
    strview sv = stv_literal("abc123");
    TEST_ASSERT_EQUAL_size_t(3, stv_firstIf(sv, isdigit, false));
    TEST_ASSERT_EQUAL_size_t(0, stv_firstIf(sv, isdigit, true));
}

void test_lastCharClass_digit(void) {
    strview sv = stv_literal("123abc456");
    TEST_ASSERT_EQUAL_size_t(8, stv_lastIf(sv, isdigit, false));
    TEST_ASSERT_EQUAL_size_t(5, stv_lastIf(sv, isdigit, true));
}

void test_charClass_empty_view(void) {
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstIf(stv_nullstv, isdigit, false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastIf(stv_nullstv, isdigit, false));
}

/* ========== generic marco (C11) ========== */
#if defined(LIB_STV_GENERIC)
void test_stv_firstIndex_char(void) {
    strview sv = stv_literal("hello");
    size_t  i  = stv_firstIndex(sv, 'l', false);
    TEST_ASSERT_EQUAL_size_t(2, i);
}

void test_stv_firstIndex_charset(void) {
    strview sv = stv_literal("abc123");
    size_t  i  = stv_firstIndex(sv, stv_literal("0123456789"), false);
    TEST_ASSERT_EQUAL_size_t(3, i);
}

void test_stv_firstIndex_class(void) {
    strview sv = stv_literal("abc123");
    size_t  i  = stv_firstIndex(sv, isdigit, false);
    TEST_ASSERT_EQUAL_size_t(3, i);
}

void test_stv_firstIndex_invert(void) {
    strview sv = stv_literal("aaa123");
    size_t  i  = stv_firstIndex(sv, 'a', true);
    TEST_ASSERT_EQUAL_size_t(3, i);
}

void test_stv_lastIndex_char(void) {
    strview sv = stv_literal("abracadabra");
    size_t  i  = stv_lastIndex(sv, 'a', false);
    TEST_ASSERT_EQUAL_size_t(10, i);
}

void test_stv_lastIndex_charset(void) {
    strview sv = stv_literal("abc123def456");
    size_t  i  = stv_lastIndex(sv, stv_literal("0123456789"), false);
    TEST_ASSERT_EQUAL_size_t(11, i);
}

void test_stv_lastIndex_class(void) {
    strview sv = stv_literal("123abc456");
    size_t  i  = stv_lastIndex(sv, isdigit, false);
    TEST_ASSERT_EQUAL_size_t(8, i);
}
#endif

/* ========== search ========== */
void test_naive_search_found(void) {
    strview text = stv_literal("hello world hello");
    strview pat  = stv_literal("world");
    size_t  pos  = stv_naiveSearch(text, pat, false);
    TEST_ASSERT_EQUAL_size_t(6, pos);
}

void test_naive_search_nocase_found(void) {
    strview text = stv_literal("hello WORLD hello");
    strview pat  = stv_literal("world");
    size_t  pos  = stv_naiveSearch(text, pat, true);
    TEST_ASSERT_EQUAL_size_t(6, pos);
}

void test_naive_search_not_found(void) {
    strview text = stv_literal("hello");
    strview pat  = stv_literal("world");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_naiveSearch(text, pat, false));
}

void test_naive_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(0, stv_naiveSearch(text, stv_nullstv, false));
}

void test_naive_search_longer_pat(void) {
    strview text = stv_literal("ab");
    strview pat  = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_naiveSearch(text, pat, false));
}

void test_sunday_search_found(void) {
    strview text = stv_literal("find the needle in haystack");
    strview pat  = stv_literal("needle");
    size_t  pos  = stv_sundaySearch(text, pat, false);
    TEST_ASSERT_EQUAL_size_t(9, pos);
}

void test_sunday_search_not_found(void) {
    strview text = stv_literal("abcde");
    strview pat  = stv_literal("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_sundaySearch(text, pat, false));
}

void test_sunday_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(0, stv_sundaySearch(text, stv_nullstv, false));
}

void test_search_uses_sunday(void) {
    strview text = stv_literal("aaaaabaaaaa");
    strview pat  = stv_literal("aabaa"); /* len=5 > 4 -> Sunday */
    size_t  pos  = stv_search(text, pat, false);
    TEST_ASSERT_EQUAL_size_t(3, pos);
}

void test_search_fallback_naive(void) {
    strview text = stv_literal("abcdef");
    strview pat  = stv_literal("cd"); /* len=2 <=4 -> naive */
    size_t  pos  = stv_search(text, pat, false);
    TEST_ASSERT_EQUAL_size_t(2, pos);
}

void test_search_empty_pat(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_search(stv_literal("text"), stv_nullstv, false));
}

/* ========== reverse search ========== */
void test_rev_search_found(void) {
    strview text = stv_literal("hello world hello");
    strview pat  = stv_literal("hello");
    size_t  pos  = stv_searchRev(text, pat, false);
    TEST_ASSERT_EQUAL_size_t(12, pos);
}

void test_rev_search_nocase(void) {
    strview text = stv_literal("hello world HELLO");
    strview pat  = stv_literal("hello");
    size_t  pos  = stv_searchRev(text, pat, true);
    TEST_ASSERT_EQUAL_size_t(12, pos);
}

void test_rev_search_not_found(void) {
    strview text = stv_literal("abc");
    strview pat  = stv_literal("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_searchRev(text, pat, false));
}

void test_rev_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(3, stv_searchRev(text, stv_nullstv, false));
}

void test_rev_search_longer_pat(void) {
    strview text = stv_literal("ab");
    strview pat  = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_searchRev(text, pat, false));
}

void test_rev_search_single_char(void) {
    strview text = stv_literal("aaaa");
    strview pat  = stv_literal("a");
    TEST_ASSERT_EQUAL_size_t(3, stv_searchRev(text, pat, false));
}

void test_rev_naive_search_found(void) {
    strview text = stv_literal("ababa");
    strview pat  = stv_literal("aba");
    TEST_ASSERT_EQUAL_size_t(2, stv_naiveSearchRev(text, pat, false));
}

void test_rev_naive_search_nocase(void) {
    strview text = stv_literal("abABA");
    strview pat  = stv_literal("aba");
    TEST_ASSERT_EQUAL_size_t(2, stv_naiveSearchRev(text, pat, true));
}

void test_rev_naive_search_not_found(void) {
    strview text = stv_literal("xyz");
    strview pat  = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_naiveSearchRev(text, pat, false));
}

void test_rev_naive_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(3, stv_naiveSearchRev(text, stv_nullstv, false));
}

void test_rev_sunday_search_found(void) {
    strview text = stv_literal("find the needle, then another needle here");
    strview pat  = stv_literal("needle");
    size_t  pos  = stv_sundaySearchRev(text, pat, false);
    TEST_ASSERT_EQUAL_size_t(30, pos);
}

void test_rev_sunday_search_not_found(void) {
    strview text = stv_literal("abcde");
    strview pat  = stv_literal("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_sundaySearchRev(text, pat, false));
}

void test_rev_sunday_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(3, stv_sundaySearchRev(text, stv_nullstv, false));
}

void run_search_tests(void) {
    RUN_TEST(test_firstChar_found);
    RUN_TEST(test_firstChar_not_found);
    RUN_TEST(test_firstNotChar);
    RUN_TEST(test_lastChar);
    RUN_TEST(test_lastNotChar);
    RUN_TEST(test_char_empty_view);
    RUN_TEST(test_firstCharset_found);
    RUN_TEST(test_firstCharset_not_found);
    RUN_TEST(test_lastCharset);
    RUN_TEST(test_charset_empty_view);
    RUN_TEST(test_firstCharClass_digit);
    RUN_TEST(test_lastCharClass_digit);
    RUN_TEST(test_charClass_empty_view);
#if defined(LIB_STV_GENERIC)
    RUN_TEST(test_stv_firstIndex_char);
    RUN_TEST(test_stv_firstIndex_charset);
    RUN_TEST(test_stv_firstIndex_class);
    RUN_TEST(test_stv_firstIndex_invert);
    RUN_TEST(test_stv_lastIndex_char);
    RUN_TEST(test_stv_lastIndex_charset);
    RUN_TEST(test_stv_lastIndex_class);
#endif
    RUN_TEST(test_naive_search_found);
    RUN_TEST(test_naive_search_nocase_found);
    RUN_TEST(test_naive_search_not_found);
    RUN_TEST(test_naive_search_empty_pattern);
    RUN_TEST(test_naive_search_longer_pat);
    RUN_TEST(test_sunday_search_found);
    RUN_TEST(test_sunday_search_not_found);
    RUN_TEST(test_sunday_search_empty_pattern);
    RUN_TEST(test_search_uses_sunday);
    RUN_TEST(test_search_fallback_naive);
    RUN_TEST(test_search_empty_pat);
    RUN_TEST(test_rev_search_found);
    RUN_TEST(test_rev_search_nocase);
    RUN_TEST(test_rev_search_not_found);
    RUN_TEST(test_rev_search_empty_pattern);
    RUN_TEST(test_rev_search_longer_pat);
    RUN_TEST(test_rev_search_single_char);
    RUN_TEST(test_rev_naive_search_found);
    RUN_TEST(test_rev_naive_search_nocase);
    RUN_TEST(test_rev_naive_search_not_found);
    RUN_TEST(test_rev_naive_search_empty_pattern);
    RUN_TEST(test_rev_sunday_search_found);
    RUN_TEST(test_rev_sunday_search_not_found);
    RUN_TEST(test_rev_sunday_search_empty_pattern);
}
