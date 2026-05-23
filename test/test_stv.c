/**
 * @file test_stv.c
 * @brief Unity tests for the stv.h
 */

#include <stdlib.h>
#include <string.h>

#include "unity/unity.h"

#define LIB_STV_IMPL
#include "stv.h"

void setUp(void) {}
void tearDown(void) {}

/* ========================================================================== */
/*  stv_new / stv_create                                                      */
/* ========================================================================== */

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

/* ========================================================================== */
/*  stv_slice                                                                 */
/* ========================================================================== */

void test_stv_slice_full_range(void) {
    strview sv = stv_new("Hello");
    strview s  = stv_slice(sv, stv_begin, stv_end);
    TEST_ASSERT_TRUE(stv_same(sv, s));
}

void test_stv_slice_begin(void) {
    strview sv = stv_new("Hello");
    strview s  = stv_slice(sv, stv_begin, 3);
    TEST_ASSERT_EQUAL_size_t(3, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("Hel", s.data, 3);
}

void test_stv_slice_mid(void) {
    strview sv = stv_new("Hello");
    strview s  = stv_slice(sv, 1, 4);
    TEST_ASSERT_EQUAL_size_t(3, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ell", s.data, 3);
}

void test_stv_slice_end(void) {
    strview sv = stv_new("Hello");
    strview s  = stv_slice(sv, 2, stv_end);
    TEST_ASSERT_EQUAL_size_t(3, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("llo", s.data, 3);
}

void test_stv_slice_empty_result(void) {
    strview sv = stv_new("Hello");
    strview s  = stv_slice(sv, 2, 2);
    TEST_ASSERT_EQUAL_size_t(0, s.len);
    TEST_ASSERT_TRUE(stv_empty(s));
}

void test_stv_slice_out_of_range(void) {
    strview sv = stv_new("Hi");
    strview s1 = stv_slice(sv, 0, 10);
    TEST_ASSERT_TRUE(stv_empty(s1));

    strview s2 = stv_slice(sv, 3, 4);
    TEST_ASSERT_TRUE(stv_empty(s2));

    strview s3 = stv_slice(sv, 2, 1);
    TEST_ASSERT_TRUE(stv_empty(s3));
}

void test_stv_slice_null_view(void) {
    strview sv = stv_nullstv;
    strview s  = stv_slice(sv, 0, 5);
    TEST_ASSERT_TRUE(stv_empty(s));
}

/* ========================================================================== */
/*  trim functions                                                            */
/* ========================================================================== */

void test_stv_trim_both(void) {
    strview sv      = stv_new("  \t  hello \t ");
    strview trimmed = stv_trim(sv, stv_whitespace);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_stv_trim_start(void) {
    strview sv      = stv_new("///path//");
    strview charset = stv_new("/");
    strview trimmed = stv_trimStart(sv, charset);
    TEST_ASSERT_EQUAL_size_t(6, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("path//", trimmed.data, 6);
}

void test_stv_trim_end(void) {
    strview sv      = stv_new("hello---");
    strview charset = stv_new("-");
    strview trimmed = stv_trimEnd(sv, charset);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_stv_trim_no_op(void) {
    strview sv      = stv_new("abc");
    strview trimmed = stv_trim(sv, stv_nullstv);
    TEST_ASSERT_TRUE(stv_equal(sv, trimmed));
}

void test_stv_trim_all_chars_removed(void) {
    strview sv      = stv_new("xxxx");
    strview charset = stv_new("x");
    strview trimmed = stv_trim(sv, charset);
    TEST_ASSERT_EQUAL_size_t(0, trimmed.len);
}

void test_stv_trim_empty_view(void) {
    strview sv      = stv_nullstv;
    strview trimmed = stv_trim(sv, stv_whitespace);
    TEST_ASSERT_TRUE(stv_empty(trimmed));
}

/* ========================================================================== */
/*  Search functions                                                          */
/* ========================================================================== */

void test_stv_naive_search_found(void) {
    strview text = stv_new("hello world hello");
    strview pat  = stv_new("world");
    size_t  pos  = stv_naiveSearch(text, pat);
    TEST_ASSERT_EQUAL_size_t(6, pos);
}

void test_stv_naive_search_not_found(void) {
    strview text = stv_new("hello");
    strview pat  = stv_new("world");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_naiveSearch(text, pat));
}

void test_stv_naive_search_empty_pattern(void) {
    strview text = stv_new("abc");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(0, stv_naiveSearch(text, pat));
}

void test_stv_naive_search_longer_pat(void) {
    strview text = stv_new("ab");
    strview pat  = stv_new("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_naiveSearch(text, pat));
}

void test_stv_sunday_search_found(void) {
    strview text = stv_new("find the needle in haystack");
    strview pat  = stv_new("needle");
    size_t  pos  = stv_sundaySearch(text, pat);
    TEST_ASSERT_EQUAL_size_t(9, pos);
}

void test_stv_sunday_search_not_found(void) {
    strview text = stv_new("abcde");
    strview pat  = stv_new("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_sundaySearch(text, pat));
}

void test_stv_search_uses_sunday(void) {
    /* With pattern length > 4, should use Sunday algorithm */
    strview text = stv_new("aaaaabaaaaa");
    strview pat  = stv_new("aabaa");
    size_t  pos  = stv_search(text, pat);
    TEST_ASSERT_EQUAL_size_t(3, pos);
}

void test_stv_search_fallback_naive(void) {
    /* Pattern length <= 4 should use naive search */
    strview text = stv_new("abcdef");
    strview pat  = stv_new("cd");
    size_t  pos  = stv_search(text, pat);
    TEST_ASSERT_EQUAL_size_t(2, pos);
}

void test_stv_search_empty_pat(void) {
    strview text = stv_new("text");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(0, stv_search(text, pat));
}

/* ========================================================================== */
/*  Reverse search functions                                                  */
/* ========================================================================== */

void test_stv_rev_search_found(void) {
    strview text = stv_new("hello world hello");
    strview pat  = stv_new("hello");
    size_t  pos  = stv_rev_search(text, pat);
    TEST_ASSERT_EQUAL_size_t(12, pos);
}

void test_stv_rev_search_not_found(void) {
    strview text = stv_new("abc");
    strview pat  = stv_new("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_search(text, pat));
}

void test_stv_rev_search_empty_pattern(void) {
    strview text = stv_new("abc");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(3, stv_rev_search(text, pat));
}

void test_stv_rev_search_longer_pat(void) {
    strview text = stv_new("ab");
    strview pat  = stv_new("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_search(text, pat));
}

void test_stv_rev_search_single_char(void) {
    strview text = stv_new("aaaa");
    strview pat  = stv_new("a");
    TEST_ASSERT_EQUAL_size_t(3, stv_rev_search(text, pat));
}

void test_stv_rev_naive_search_found(void) {
    strview text = stv_new("ababa");
    strview pat  = stv_new("aba");
    TEST_ASSERT_EQUAL_size_t(2, stv_rev_naiveSearch(text, pat));
}

void test_stv_rev_naive_search_not_found(void) {
    strview text = stv_new("xyz");
    strview pat  = stv_new("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_naiveSearch(text, pat));
}

void test_stv_rev_naive_search_empty_pattern(void) {
    strview text = stv_new("abc");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(3, stv_rev_naiveSearch(text, pat));
}

void test_stv_rev_sunday_search_found(void) {
    strview text = stv_new("find the needle, then another needle here");
    strview pat  = stv_new("needle");
    size_t  pos  = stv_rev_sundaySearch(text, pat);
    TEST_ASSERT_EQUAL_size_t(30, pos);
}

void test_stv_rev_sunday_search_not_found(void) {
    strview text = stv_new("abcde");
    strview pat  = stv_new("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_sundaySearch(text, pat));
}

void test_stv_rev_sunday_search_empty_pattern(void) {
    strview text = stv_new("abc");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(3, stv_rev_sundaySearch(text, pat));
}

void test_stv_rev_search_empty_text(void) {
    strview text = stv_nullstv;
    strview pat  = stv_new("x");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_search(text, pat));
}

void test_stv_rev_naive_empty_text(void) {
    strview text = stv_nullstv;
    strview pat  = stv_new("x");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_naiveSearch(text, pat));
}

void test_stv_rev_sunday_empty_text(void) {
    strview text = stv_nullstv;
    strview pat  = stv_new("x");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_sundaySearch(text, pat));
}

/* ========================================================================== */
/*  First/Last character queries                                              */
/* ========================================================================== */

void test_stv_first_char_found(void) {
    strview sv = stv_new("hello");
    TEST_ASSERT_EQUAL_size_t(0, stv_firstChar(sv, 'h'));
    TEST_ASSERT_EQUAL_size_t(4, stv_firstChar(sv, 'o'));
}

void test_stv_first_char_not_found(void) {
    strview sv = stv_new("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChar(sv, 'x'));
}

void test_stv_first_not_char(void) {
    strview sv = stv_new("aaabc");
    TEST_ASSERT_EQUAL_size_t(3, stv_firstNotChar(sv, 'a'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstNotChar(stv_new("aaa"), 'a'));
}

void test_stv_last_char(void) {
    strview sv = stv_new("abracadabra");
    TEST_ASSERT_EQUAL_size_t(10, stv_lastChar(sv, 'a'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastChar(sv, 'z'));
}

void test_stv_last_not_char(void) {
    strview sv = stv_new("hello---");
    TEST_ASSERT_EQUAL_size_t(4, stv_lastNotChar(sv, '-'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastNotChar(stv_new("---"), '-'));
}

void test_stv_char_empty_view(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChar(sv, 'x'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstNotChar(sv, 'x'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastChar(sv, 'x'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastNotChar(sv, 'x'));
}

/* ========================================================================== */
/*  firstDiff / lastDiff                                                      */
/* ========================================================================== */

void test_stv_first_diff_same(void) {
    strview a = stv_new("abc");
    strview b = stv_new("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstDiff(a, b));
}

void test_stv_first_diff_at_start(void) {
    strview a = stv_new("abc");
    strview b = stv_new("xbc");
    TEST_ASSERT_EQUAL_size_t(0, stv_firstDiff(a, b));
}

void test_stv_first_diff_length_mismatch(void) {
    strview a = stv_new("abc");
    strview b = stv_new("ab");
    TEST_ASSERT_EQUAL_size_t(2, stv_firstDiff(a, b));
}

void test_stv_last_diff(void) {
    strview a = stv_new("hello");
    strview b = stv_new("hallo");
    TEST_ASSERT_EQUAL_size_t(1, stv_lastDiff(a, b));
}

void test_stv_last_diff_length_mismatch(void) {
    strview a = stv_new("abc");
    strview b = stv_new("abcd");
    TEST_ASSERT_EQUAL_size_t(3, stv_lastDiff(a, b));
}

void test_stv_diff_with_empty(void) {
    strview a = stv_new("abc");
    strview e = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(0, stv_firstDiff(a, e));
    TEST_ASSERT_EQUAL_size_t(2, stv_lastDiff(a, e));
}

void test_stv_diff_both_empty(void) {
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstDiff(stv_nullstv, stv_nullstv));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastDiff(stv_nullstv, stv_nullstv));
}

/* ========================================================================== */
/*  stv_compare                                                               */
/* ========================================================================== */

void test_stv_compare_equal(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_new("abc"), stv_new("abc")) == 0);
}

void test_stv_compare_less(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_new("abc"), stv_new("abd")) < 0);
}

void test_stv_compare_greater(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_new("abd"), stv_new("abc")) > 0);
}

void test_stv_compare_shorter(void) {
    /* "ab" < "abc" */
    TEST_ASSERT_TRUE(stv_compare(stv_new("ab"), stv_new("abc")) < 0);
}

/* ========================================================================== */
/*  startsWith / endsWith / contains                                          */
/* ========================================================================== */

void test_stv_starts_with_true(void) {
    strview text = stv_new("Hello World!");
    strview pat  = stv_new("Hello");
    TEST_ASSERT_TRUE(stv_startsWith(text, pat));
}

void test_stv_starts_with_false(void) {
    strview text = stv_new("Hello");
    strview pat  = stv_new("World");
    TEST_ASSERT_FALSE(stv_startsWith(text, pat));
}

void test_stv_starts_with_empty_pat(void) {
    TEST_ASSERT_TRUE(stv_startsWith(stv_new("abc"), stv_nullstv));
}

void test_stv_ends_with(void) {
    strview text = stv_new("document.txt");
    strview pat  = stv_new(".txt");
    TEST_ASSERT_TRUE(stv_endsWith(text, pat));
    TEST_ASSERT_FALSE(stv_endsWith(text, stv_new(".doc")));
}

void test_stv_ends_with_empty(void) {
    TEST_ASSERT_TRUE(stv_endsWith(stv_new("any"), stv_nullstv));
}

void test_stv_contains(void) {
    strview text = stv_new("the quick brown fox");
    TEST_ASSERT_TRUE(stv_contains(text, stv_new("quick")));
    TEST_ASSERT_FALSE(stv_contains(text, stv_new("slow")));
}

void test_stv_contains_empty_pat(void) {
    TEST_ASSERT_TRUE(stv_contains(stv_new("abc"), stv_nullstv));
}

/* ========================================================================== */
/*  stv_same / stv_equal / stv_empty                                          */
/* ========================================================================== */

void test_stv_same(void) {
    char* data = strcpy(malloc(sizeof("abc")), "abc");
    TEST_ASSERT_TRUE(stv_same(stv_new("abc"), stv_new("abc")));
    TEST_ASSERT_FALSE(stv_same(stv_new(data), stv_new("abc")));
}

void test_stv_equal(void) {
    const char* data = "example";
    strview     a    = stv_makestv(data, 5);
    strview     b    = stv_makestv(data, 6);
    strview     c    = stv_makestv(data + 1, 4);
    TEST_ASSERT_FALSE(stv_equal(a, b));
    TEST_ASSERT_FALSE(stv_equal(a, c));
}

void test_stv_empty(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_nullstv));
    TEST_ASSERT_TRUE(stv_empty(stv_new("")));
    TEST_ASSERT_FALSE(stv_empty(stv_new("a")));
}

/* ========================================================================== */
/*  stv_front / stv_back                                                      */
/* ========================================================================== */

void test_stv_front_back(void) {
    strview sv = stv_new("Hello");
    TEST_ASSERT_EQUAL_CHAR('H', stv_front(sv));
    TEST_ASSERT_EQUAL_CHAR('o', stv_back(sv));
}

void test_stv_front_back_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_CHAR('\0', stv_front(sv));
    TEST_ASSERT_EQUAL_CHAR('\0', stv_back(sv));
}

/* ========================================================================== */
/*  stv_swap                                                                  */
/* ========================================================================== */

void test_stv_swap(void) {
    strview a = stv_new("first");
    strview b = stv_new("second");
    stv_swap(&a, &b);
    TEST_ASSERT_EQUAL_STRING("second", a.data);
    TEST_ASSERT_EQUAL_size_t(6, a.len);
    TEST_ASSERT_EQUAL_STRING("first", b.data);
    TEST_ASSERT_EQUAL_size_t(5, b.len);
}

void test_stv_swap_null_pointers(void) {
    strview a = stv_new("a");
    stv_swap(&a, NULL); /* should do nothing */
    TEST_ASSERT_EQUAL_STRING("a", a.data);
}

/* ========================================================================== */
/*  stv_cstr / stv_rev_cstr                                                   */
/* ========================================================================== */

void test_stv_cstr_success(void) {
    char    buf[6];
    strview sv  = stv_new("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("Hello", buf);
}

void test_stv_cstr_too_small(void) {
    char    buf[3];
    strview sv  = stv_new("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf));
    TEST_ASSERT_NULL(ret);
}

void test_stv_rev_cstr(void) {
    char    buf[6];
    strview sv  = stv_new("Hello");
    char*   ret = stv_rev_cstr(sv, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("olleH", buf);
}

void test_stv_cstr_empty_view(void) {
    char    buf[2] = {0};
    strview sv     = stv_nullstv;
    char*   ret    = stv_cstr(sv, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

/* ========================================================================== */
/*  Main entry point                                                          */
/* ========================================================================== */

int main(void) {
    UNITY_BEGIN();

    /* stv_new / stv_create */
    RUN_TEST(test_stv_new_normal);
    RUN_TEST(test_stv_new_empty);
    RUN_TEST(test_stv_new_null);
    RUN_TEST(test_stv_create_stop_at_char);
    RUN_TEST(test_stv_create_maxlen_cut);
    RUN_TEST(test_stv_create_null);

    /* stv_slice */
    RUN_TEST(test_stv_slice_full_range);
    RUN_TEST(test_stv_slice_begin);
    RUN_TEST(test_stv_slice_mid);
    RUN_TEST(test_stv_slice_end);
    RUN_TEST(test_stv_slice_empty_result);
    RUN_TEST(test_stv_slice_out_of_range);
    RUN_TEST(test_stv_slice_null_view);

    /* trim functions */
    RUN_TEST(test_stv_trim_both);
    RUN_TEST(test_stv_trim_start);
    RUN_TEST(test_stv_trim_end);
    RUN_TEST(test_stv_trim_no_op);
    RUN_TEST(test_stv_trim_all_chars_removed);
    RUN_TEST(test_stv_trim_empty_view);

    /* search functions */
    RUN_TEST(test_stv_naive_search_found);
    RUN_TEST(test_stv_naive_search_not_found);
    RUN_TEST(test_stv_naive_search_empty_pattern);
    RUN_TEST(test_stv_naive_search_longer_pat);
    RUN_TEST(test_stv_sunday_search_found);
    RUN_TEST(test_stv_sunday_search_not_found);
    RUN_TEST(test_stv_search_uses_sunday);
    RUN_TEST(test_stv_search_fallback_naive);
    RUN_TEST(test_stv_search_empty_pat);

    /* reverse search functions */
    RUN_TEST(test_stv_rev_search_found);
    RUN_TEST(test_stv_rev_search_not_found);
    RUN_TEST(test_stv_rev_search_empty_pattern);
    RUN_TEST(test_stv_rev_search_longer_pat);
    RUN_TEST(test_stv_rev_search_single_char);
    RUN_TEST(test_stv_rev_naive_search_found);
    RUN_TEST(test_stv_rev_naive_search_not_found);
    RUN_TEST(test_stv_rev_naive_search_empty_pattern);
    RUN_TEST(test_stv_rev_sunday_search_found);
    RUN_TEST(test_stv_rev_sunday_search_not_found);
    RUN_TEST(test_stv_rev_sunday_search_empty_pattern);
    RUN_TEST(test_stv_rev_search_empty_text);
    RUN_TEST(test_stv_rev_naive_empty_text);
    RUN_TEST(test_stv_rev_sunday_empty_text);

    /* first/last character queries */
    RUN_TEST(test_stv_first_char_found);
    RUN_TEST(test_stv_first_char_not_found);
    RUN_TEST(test_stv_first_not_char);
    RUN_TEST(test_stv_last_char);
    RUN_TEST(test_stv_last_not_char);
    RUN_TEST(test_stv_char_empty_view);

    /* firstDiff / lastDiff */
    RUN_TEST(test_stv_first_diff_same);
    RUN_TEST(test_stv_first_diff_at_start);
    RUN_TEST(test_stv_first_diff_length_mismatch);
    RUN_TEST(test_stv_last_diff);
    RUN_TEST(test_stv_last_diff_length_mismatch);
    RUN_TEST(test_stv_diff_with_empty);
    RUN_TEST(test_stv_diff_both_empty);

    /* stv_compare */
    RUN_TEST(test_stv_compare_equal);
    RUN_TEST(test_stv_compare_less);
    RUN_TEST(test_stv_compare_greater);
    RUN_TEST(test_stv_compare_shorter);

    /* startsWith / endsWith / contains */
    RUN_TEST(test_stv_starts_with_true);
    RUN_TEST(test_stv_starts_with_false);
    RUN_TEST(test_stv_starts_with_empty_pat);
    RUN_TEST(test_stv_ends_with);
    RUN_TEST(test_stv_ends_with_empty);
    RUN_TEST(test_stv_contains);
    RUN_TEST(test_stv_contains_empty_pat);

    /* same / equal / empty */
    RUN_TEST(test_stv_same);
    RUN_TEST(test_stv_equal);
    RUN_TEST(test_stv_empty);

    /* front / back */
    RUN_TEST(test_stv_front_back);
    RUN_TEST(test_stv_front_back_empty);

    /* swap */
    RUN_TEST(test_stv_swap);
    RUN_TEST(test_stv_swap_null_pointers);

    /* cstr / rev_cstr */
    RUN_TEST(test_stv_cstr_success);
    RUN_TEST(test_stv_cstr_too_small);
    RUN_TEST(test_stv_rev_cstr);
    RUN_TEST(test_stv_cstr_empty_view);

    return UNITY_END();
}
