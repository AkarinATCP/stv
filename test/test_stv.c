/**
 * @file test_stv.c
 * @brief Unity tests for the stv.h
 */

#include <ctype.h>
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
    strview sv = stv_literal("Hello");
    strview s  = stv_slice(sv, stv_begin, stv_end);
    TEST_ASSERT_TRUE(stv_same(sv, s));
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
/*  stv_removeStart / stv_removeEnd / stv_removePrefix / stv_removeSuffix    */
/* ========================================================================== */

void test_stv_removeStart_normal(void) {
    strview sv = stv_literal("Hello World");
    strview s  = stv_removeStart(sv, 6);
    TEST_ASSERT_EQUAL_size_t(5, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("World", s.data, 5);
}

void test_stv_removeStart_zero(void) {
    strview sv = stv_literal("Hello");
    strview s  = stv_removeStart(sv, 0);
    TEST_ASSERT_TRUE(stv_equal(sv, s));
}

void test_stv_removeStart_exact_length(void) {
    strview sv = stv_literal("abc");
    strview s  = stv_removeStart(sv, 3);
    TEST_ASSERT_TRUE(stv_empty(s));
}

void test_stv_removeStart_exceeds_length(void) {
    strview sv = stv_literal("abc");
    strview s  = stv_removeStart(sv, 10);
    TEST_ASSERT_TRUE(stv_empty(s));
}

void test_stv_removeStart_empty_view(void) {
    strview sv = stv_nullstv;
    strview s  = stv_removeStart(sv, 5);
    TEST_ASSERT_TRUE(stv_empty(s));
}

void test_stv_removeEnd_normal(void) {
    strview sv = stv_literal("Hello World");
    strview s  = stv_removeEnd(sv, 6);
    TEST_ASSERT_EQUAL_size_t(5, s.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("Hello", s.data, 5);
}

void test_stv_removeEnd_zero(void) {
    strview sv = stv_literal("Hello");
    strview s  = stv_removeEnd(sv, 0);
    TEST_ASSERT_TRUE(stv_equal(sv, s));
}

void test_stv_removeEnd_exact_length(void) {
    strview sv = stv_literal("abc");
    strview s  = stv_removeEnd(sv, 3);
    TEST_ASSERT_TRUE(stv_empty(s));
}

void test_stv_removeEnd_exceeds_length(void) {
    strview sv = stv_literal("abc");
    strview s  = stv_removeEnd(sv, 5);
    TEST_ASSERT_TRUE(stv_empty(s));
}

void test_stv_removeEnd_empty_view(void) {
    strview sv = stv_nullstv;
    strview s  = stv_removeEnd(sv, 2);
    TEST_ASSERT_TRUE(stv_empty(s));
}

void test_stv_removePrefix_match(void) {
    strview sv     = stv_literal("http://example.com");
    strview prefix = stv_literal("http://");
    strview result = stv_removePrefix(sv, prefix);
    TEST_ASSERT_EQUAL_size_t(11, result.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("example.com", result.data, 11);
}

void test_stv_removePrefix_no_match(void) {
    strview sv     = stv_literal("ftp://example.com");
    strview prefix = stv_literal("http://");
    strview result = stv_removePrefix(sv, prefix);
    TEST_ASSERT_TRUE(stv_equal(sv, result));
}

void test_stv_removePrefix_empty_prefix(void) {
    strview sv     = stv_literal("hello");
    strview prefix = stv_nullstv;
    strview result = stv_removePrefix(sv, prefix);
    TEST_ASSERT_TRUE(stv_equal(sv, result));
}

void test_stv_removePrefix_empty_view(void) {
    strview sv     = stv_nullstv;
    strview prefix = stv_literal("a");
    strview result = stv_removePrefix(sv, prefix);
    TEST_ASSERT_TRUE(stv_empty(result));
}

void test_stv_removeSuffix_match(void) {
    strview sv     = stv_literal("document.txt");
    strview suffix = stv_literal(".txt");
    strview result = stv_removeSuffix(sv, suffix);
    TEST_ASSERT_EQUAL_size_t(8, result.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("document", result.data, 8);
}

void test_stv_removeSuffix_no_match(void) {
    strview sv     = stv_literal("document.md");
    strview suffix = stv_literal(".txt");
    strview result = stv_removeSuffix(sv, suffix);
    TEST_ASSERT_TRUE(stv_equal(sv, result));
}

void test_stv_removeSuffix_empty_suffix(void) {
    strview sv     = stv_literal("hello");
    strview suffix = stv_nullstv;
    strview result = stv_removeSuffix(sv, suffix);
    TEST_ASSERT_TRUE(stv_equal(sv, result));
}

void test_stv_removeSuffix_empty_view(void) {
    strview sv     = stv_nullstv;
    strview suffix = stv_literal("a");
    strview result = stv_removeSuffix(sv, suffix);
    TEST_ASSERT_TRUE(stv_empty(result));
}

/* ========================================================================== */
/*  trim functions                                                            */
/* ========================================================================== */

void test_stv_trim_both(void) {
    strview sv      = stv_literal("  \t  hello \t ");
    strview trimmed = stv_trim(sv, stv_whitespace);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_stv_trim_start(void) {
    strview sv      = stv_literal("///path//");
    strview charset = stv_literal("/");
    strview trimmed = stv_trimStart(sv, charset);
    TEST_ASSERT_EQUAL_size_t(6, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("path//", trimmed.data, 6);
}

void test_stv_trim_end(void) {
    strview sv      = stv_literal("hello---");
    strview charset = stv_literal("-");
    strview trimmed = stv_trimEnd(sv, charset);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_stv_trim_no_op(void) {
    strview sv      = stv_literal("abc");
    strview trimmed = stv_trim(sv, stv_nullstv);
    TEST_ASSERT_TRUE(stv_equal(sv, trimmed));
}

void test_stv_trim_all_chars_removed(void) {
    strview sv      = stv_literal("xxxx");
    strview charset = stv_literal("x");
    strview trimmed = stv_trim(sv, charset);
    TEST_ASSERT_EQUAL_size_t(0, trimmed.len);
}

void test_stv_trim_empty_view(void) {
    strview sv      = stv_nullstv;
    strview trimmed = stv_trim(sv, stv_whitespace);
    TEST_ASSERT_TRUE(stv_empty(trimmed));
}

void test_stv_trimIf_both(void) {
    strview sv      = stv_literal("  \t  hello \t ");
    strview trimmed = stv_trimIf(sv, isspace);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_stv_trimIf_null_handle(void) {
    strview sv      = stv_literal("abc");
    strview trimmed = stv_trimIf(sv, NULL);
    TEST_ASSERT_TRUE(stv_equal(sv, trimmed));
}

void test_stv_trimIf_all_removed(void) {
    strview sv      = stv_literal(" \t\n\r");
    strview trimmed = stv_trimIf(sv, isspace);
    TEST_ASSERT_EQUAL_size_t(0, trimmed.len);
}

void test_stv_trimIf_empty_view(void) {
    strview sv      = stv_nullstv;
    strview trimmed = stv_trimIf(sv, isspace);
    TEST_ASSERT_TRUE(stv_empty(trimmed));
}

void test_stv_trimStartIf(void) {
    strview sv      = stv_literal("   text");
    strview trimmed = stv_trimStartIf(sv, isspace);
    TEST_ASSERT_EQUAL_size_t(4, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("text", trimmed.data, 4);
}

void test_stv_trimStartIf_null_handle(void) {
    strview sv      = stv_literal("abc");
    strview trimmed = stv_trimStartIf(sv, NULL);
    TEST_ASSERT_TRUE(stv_equal(sv, trimmed));
}

void test_stv_trimStartIf_empty_view(void) {
    strview sv      = stv_nullstv;
    strview trimmed = stv_trimStartIf(sv, isspace);
    TEST_ASSERT_TRUE(stv_empty(trimmed));
}

void test_stv_trimEndIf(void) {
    strview sv      = stv_literal("text   ");
    strview trimmed = stv_trimEndIf(sv, isspace);
    TEST_ASSERT_EQUAL_size_t(4, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("text", trimmed.data, 4);
}

void test_stv_trimEndIf_null_handle(void) {
    strview sv      = stv_literal("abc");
    strview trimmed = stv_trimEndIf(sv, NULL);
    TEST_ASSERT_TRUE(stv_equal(sv, trimmed));
}

void test_stv_trimEndIf_empty_view(void) {
    strview sv      = stv_nullstv;
    strview trimmed = stv_trimEndIf(sv, isspace);
    TEST_ASSERT_TRUE(stv_empty(trimmed));
}

/* ========================================================================== */
/*  stv_split / stv_beforeDelim / stv_afterDelim                              */
/* ========================================================================== */

void test_stv_split_normal(void) {
    strview stv = stv_literal("hello world");
    strview sep = stv_literal(" ");
    strview rem;
    strview first = stv_split(stv, sep, &rem);
    TEST_ASSERT_EQUAL_size_t(5, first.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", first.data, 5);
    TEST_ASSERT_EQUAL_size_t(5, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("world", rem.data, 5);
}

void test_stv_split_not_found(void) {
    strview stv = stv_literal("hello");
    strview sep = stv_literal("/");
    strview rem;
    strview first = stv_split(stv, sep, &rem);
    TEST_ASSERT_TRUE(stv_equal(stv, first));
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_split_empty_sep(void) {
    strview stv = stv_literal("ab");
    strview sep = stv_nullstv;
    strview rem;
    strview first = stv_split(stv, sep, &rem);
    TEST_ASSERT_EQUAL_size_t(1, first.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("a", first.data, 1);
    TEST_ASSERT_EQUAL_size_t(1, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("b", rem.data, 1);
}

void test_stv_split_empty_sep_single_char(void) {
    strview stv = stv_literal("a");
    strview sep = stv_nullstv;
    strview rem;
    strview first = stv_split(stv, sep, &rem);
    TEST_ASSERT_EQUAL_size_t(1, first.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("a", first.data, 1);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_split_null_remaining(void) {
    strview stv   = stv_literal("a,b");
    strview sep   = stv_literal(",");
    strview first = stv_split(stv, sep, NULL);
    TEST_ASSERT_EQUAL_size_t(1, first.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("a", first.data, 1);
}

void test_stv_split_sep_at_end(void) {
    strview stv = stv_literal("hello,");
    strview sep = stv_literal(",");
    strview rem;
    strview first = stv_split(stv, sep, &rem);
    TEST_ASSERT_EQUAL_size_t(5, first.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", first.data, 5);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_split_sep_at_start(void) {
    strview stv = stv_literal(",world");
    strview sep = stv_literal(",");
    strview rem;
    strview first = stv_split(stv, sep, &rem);
    TEST_ASSERT_EQUAL_size_t(0, first.len);
    TEST_ASSERT_TRUE(stv_empty(first));
    TEST_ASSERT_EQUAL_size_t(5, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("world", rem.data, 5);
}

void test_stv_split_empty_view(void) {
    strview stv   = stv_nullstv;
    strview sep   = stv_literal(",");
    strview first = stv_split(stv, sep, NULL);
    TEST_ASSERT_TRUE(stv_empty(first));
}

void test_stv_before_delim_found(void) {
    strview stv    = stv_literal("key=value");
    strview delim  = stv_literal("=");
    strview before = stv_beforeDelim(stv, delim);
    TEST_ASSERT_EQUAL_size_t(3, before.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("key", before.data, 3);
}

void test_stv_before_delim_not_found(void) {
    strview stv    = stv_literal("hello");
    strview delim  = stv_literal(":");
    strview before = stv_beforeDelim(stv, delim);
    TEST_ASSERT_TRUE(stv_equal(stv, before));
}

void test_stv_before_delim_empty_view(void) {
    strview stv    = stv_nullstv;
    strview delim  = stv_literal(":");
    strview before = stv_beforeDelim(stv, delim);
    TEST_ASSERT_TRUE(stv_empty(before));
}

void test_stv_before_delim_empty_delim(void) {
    strview stv    = stv_literal("abc");
    strview delim  = stv_nullstv;
    strview before = stv_beforeDelim(stv, delim);
    TEST_ASSERT_TRUE(stv_empty(before));
}

void test_stv_before_delim_delim_at_start(void) {
    strview stv    = stv_literal("!abc");
    strview delim  = stv_literal("!");
    strview before = stv_beforeDelim(stv, delim);
    TEST_ASSERT_EQUAL_size_t(0, before.len);
    TEST_ASSERT_TRUE(stv_empty(before));
}

void test_stv_after_delim_found(void) {
    strview stv   = stv_literal("path/to/file");
    strview delim = stv_literal("/");
    strview after = stv_afterDelim(stv, delim);
    TEST_ASSERT_EQUAL_size_t(7, after.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("to/file", after.data, 7);
}

void test_stv_after_delim_not_found(void) {
    strview stv   = stv_literal("no_slash");
    strview delim = stv_literal("/");
    strview after = stv_afterDelim(stv, delim);
    TEST_ASSERT_TRUE(stv_empty(after));
}

void test_stv_after_delim_empty_view(void) {
    strview stv   = stv_nullstv;
    strview delim = stv_literal("/");
    strview after = stv_afterDelim(stv, delim);
    TEST_ASSERT_TRUE(stv_empty(after));
}

void test_stv_after_delim_empty_delim(void) {
    strview stv   = stv_literal("any");
    strview delim = stv_nullstv;
    strview after = stv_afterDelim(stv, delim);
    TEST_ASSERT_TRUE(stv_equal(stv, after));
}

void test_stv_after_delim_delim_at_end(void) {
    strview stv   = stv_literal("abc/");
    strview delim = stv_literal("/");
    strview after = stv_afterDelim(stv, delim);
    TEST_ASSERT_TRUE(stv_empty(after));
}

/* ========================================================================== */
/*  Search functions                                                          */
/* ========================================================================== */

void test_stv_naive_search_found(void) {
    strview text = stv_literal("hello world hello");
    strview pat  = stv_literal("world");
    size_t  pos  = stv_naiveSearch(text, pat);
    TEST_ASSERT_EQUAL_size_t(6, pos);
}

void test_stv_naive_search_not_found(void) {
    strview text = stv_literal("hello");
    strview pat  = stv_literal("world");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_naiveSearch(text, pat));
}

void test_stv_naive_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(0, stv_naiveSearch(text, pat));
}

void test_stv_naive_search_longer_pat(void) {
    strview text = stv_literal("ab");
    strview pat  = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_naiveSearch(text, pat));
}

void test_stv_sunday_search_found(void) {
    strview text = stv_literal("find the needle in haystack");
    strview pat  = stv_literal("needle");
    size_t  pos  = stv_sundaySearch(text, pat);
    TEST_ASSERT_EQUAL_size_t(9, pos);
}

void test_stv_sunday_search_not_found(void) {
    strview text = stv_literal("abcde");
    strview pat  = stv_literal("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_sundaySearch(text, pat));
}

void test_stv_search_uses_sunday(void) {
    /* With pattern length > 4, should use Sunday algorithm */
    strview text = stv_literal("aaaaabaaaaa");
    strview pat  = stv_literal("aabaa");
    size_t  pos  = stv_search(text, pat);
    TEST_ASSERT_EQUAL_size_t(3, pos);
}

void test_stv_search_fallback_naive(void) {
    /* Pattern length <= 4 should use naive search */
    strview text = stv_literal("abcdef");
    strview pat  = stv_literal("cd");
    size_t  pos  = stv_search(text, pat);
    TEST_ASSERT_EQUAL_size_t(2, pos);
}

void test_stv_search_empty_pat(void) {
    strview text = stv_literal("text");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(0, stv_search(text, pat));
}

/* ========================================================================== */
/*  Reverse search functions                                                  */
/* ========================================================================== */

void test_stv_rev_search_found(void) {
    strview text = stv_literal("hello world hello");
    strview pat  = stv_literal("hello");
    size_t  pos  = stv_rev_search(text, pat);
    TEST_ASSERT_EQUAL_size_t(12, pos);
}

void test_stv_rev_search_not_found(void) {
    strview text = stv_literal("abc");
    strview pat  = stv_literal("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_search(text, pat));
}

void test_stv_rev_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(3, stv_rev_search(text, pat));
}

void test_stv_rev_search_longer_pat(void) {
    strview text = stv_literal("ab");
    strview pat  = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_search(text, pat));
}

void test_stv_rev_search_single_char(void) {
    strview text = stv_literal("aaaa");
    strview pat  = stv_literal("a");
    TEST_ASSERT_EQUAL_size_t(3, stv_rev_search(text, pat));
}

void test_stv_rev_naive_search_found(void) {
    strview text = stv_literal("ababa");
    strview pat  = stv_literal("aba");
    TEST_ASSERT_EQUAL_size_t(2, stv_rev_naiveSearch(text, pat));
}

void test_stv_rev_naive_search_not_found(void) {
    strview text = stv_literal("xyz");
    strview pat  = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_naiveSearch(text, pat));
}

void test_stv_rev_naive_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(3, stv_rev_naiveSearch(text, pat));
}

void test_stv_rev_sunday_search_found(void) {
    strview text = stv_literal("find the needle, then another needle here");
    strview pat  = stv_literal("needle");
    size_t  pos  = stv_rev_sundaySearch(text, pat);
    TEST_ASSERT_EQUAL_size_t(30, pos);
}

void test_stv_rev_sunday_search_not_found(void) {
    strview text = stv_literal("abcde");
    strview pat  = stv_literal("xyz");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_sundaySearch(text, pat));
}

void test_stv_rev_sunday_search_empty_pattern(void) {
    strview text = stv_literal("abc");
    strview pat  = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(3, stv_rev_sundaySearch(text, pat));
}

void test_stv_rev_search_empty_text(void) {
    strview text = stv_nullstv;
    strview pat  = stv_literal("x");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_search(text, pat));
}

void test_stv_rev_naive_empty_text(void) {
    strview text = stv_nullstv;
    strview pat  = stv_literal("x");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_naiveSearch(text, pat));
}

void test_stv_rev_sunday_empty_text(void) {
    strview text = stv_nullstv;
    strview pat  = stv_literal("x");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_rev_sundaySearch(text, pat));
}

/* ========================================================================== */
/*  First/Last character queries                                              */
/* ========================================================================== */

void test_stv_first_char_found(void) {
    strview sv = stv_literal("hello");
    TEST_ASSERT_EQUAL_size_t(0, stv_firstChar(sv, 'h'));
    TEST_ASSERT_EQUAL_size_t(4, stv_firstChar(sv, 'o'));
}

void test_stv_first_char_not_found(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChar(sv, 'x'));
}

void test_stv_first_not_char(void) {
    strview sv = stv_literal("aaabc");
    TEST_ASSERT_EQUAL_size_t(3, stv_firstNotChar(sv, 'a'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstNotChar(stv_literal("aaa"), 'a'));
}

void test_stv_last_char(void) {
    strview sv = stv_literal("abracadabra");
    TEST_ASSERT_EQUAL_size_t(10, stv_lastChar(sv, 'a'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastChar(sv, 'z'));
}

void test_stv_last_not_char(void) {
    strview sv = stv_literal("hello---");
    TEST_ASSERT_EQUAL_size_t(4, stv_lastNotChar(sv, '-'));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastNotChar(stv_literal("---"), '-'));
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
    strview a = stv_literal("abc");
    strview b = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstDiff(a, b));
}

void test_stv_first_diff_at_start(void) {
    strview a = stv_literal("abc");
    strview b = stv_literal("xbc");
    TEST_ASSERT_EQUAL_size_t(0, stv_firstDiff(a, b));
}

void test_stv_first_diff_length_mismatch(void) {
    strview a = stv_literal("abc");
    strview b = stv_literal("ab");
    TEST_ASSERT_EQUAL_size_t(2, stv_firstDiff(a, b));
}

void test_stv_last_diff(void) {
    strview a = stv_literal("hello");
    strview b = stv_literal("hallo");
    TEST_ASSERT_EQUAL_size_t(1, stv_lastDiff(a, b));
}

void test_stv_last_diff_length_mismatch(void) {
    strview a = stv_literal("abc");
    strview b = stv_literal("abcd");
    TEST_ASSERT_EQUAL_size_t(3, stv_lastDiff(a, b));
}

void test_stv_diff_with_empty(void) {
    strview a = stv_literal("abc");
    strview e = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(0, stv_firstDiff(a, e));
    TEST_ASSERT_EQUAL_size_t(2, stv_lastDiff(a, e));
}

void test_stv_diff_both_empty(void) {
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstDiff(stv_nullstv, stv_nullstv));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastDiff(stv_nullstv, stv_nullstv));
}

/* ========================================================================== */
/*  count / countChar /  countSubstr                                          */
/* ========================================================================== */

void test_stv_count_digits(void) {
    strview sv = stv_literal("abc123def456");
    TEST_ASSERT_EQUAL_size_t(6, stv_count(sv, isdigit));
}

void test_stv_count_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_count(sv, isdigit));
}

void test_stv_count_null_handle(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_count(sv, NULL));
}

void test_stv_countChar_normal(void) {
    strview sv = stv_literal("hello world");
    TEST_ASSERT_EQUAL_size_t(3, stv_countChar(sv, 'l'));
}

void test_stv_countChar_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_countChar(sv, 'a'));
}

void test_stv_countSubstr_normal(void) {
    strview sv  = stv_literal("abcabcdeabcabed");
    strview sub = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(3, stv_countSubstr(sv, sub));
}

void test_stv_countSubstr_empty(void) {
    strview sv  = stv_literal("abcabcdeabcabed");
    strview sub = stv_literal("");
    TEST_ASSERT_EQUAL_size_t(sv.len, stv_countSubstr(sv, sub));
}

/* ========================================================================== */
/*  every / everyChar / some / someChar                                       */
/* ========================================================================== */

void test_stv_every_digit(void) {
    strview sv = stv_literal("12345");
    TEST_ASSERT_TRUE(stv_every(sv, isdigit));
}

void test_stv_every_not_all_digit(void) {
    strview sv = stv_literal("123a");
    TEST_ASSERT_FALSE(stv_every(sv, isdigit));
}

void test_stv_every_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_FALSE(stv_every(sv, isdigit));
}

void test_stv_everyChar_true(void) {
    strview sv = stv_literal("aaaa");
    TEST_ASSERT_TRUE(stv_everyChar(sv, 'a'));
}

void test_stv_everyChar_false(void) {
    strview sv = stv_literal("aaab");
    TEST_ASSERT_FALSE(stv_everyChar(sv, 'a'));
}

void test_stv_everyChar_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_FALSE(stv_everyChar(sv, 'x'));
}

void test_stv_some_digit(void) {
    strview sv = stv_literal("abc1xyz");
    TEST_ASSERT_TRUE(stv_some(sv, isdigit));
}

void test_stv_some_no_digit(void) {
    strview sv = stv_literal("abcdef");
    TEST_ASSERT_FALSE(stv_some(sv, isdigit));
}

void test_stv_some_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_FALSE(stv_some(sv, isdigit));
}

void test_stv_someChar_found(void) {
    strview sv = stv_literal("hello");
    TEST_ASSERT_TRUE(stv_someChar(sv, 'e'));
}

void test_stv_someChar_not_found(void) {
    strview sv = stv_literal("hello");
    TEST_ASSERT_FALSE(stv_someChar(sv, 'x'));
}

void test_stv_someChar_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_FALSE(stv_someChar(sv, 'a'));
}

/* ========================================================================== */
/*  stv_compare                                                               */
/* ========================================================================== */

void test_stv_compare_equal(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_literal("abc"), stv_literal("abc")) == 0);
}

void test_stv_compare_less(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_literal("abc"), stv_literal("abd")) < 0);
}

void test_stv_compare_greater(void) {
    TEST_ASSERT_TRUE(stv_compare(stv_literal("abd"), stv_literal("abc")) > 0);
}

void test_stv_compare_shorter(void) {
    /* "ab" < "abc" */
    TEST_ASSERT_TRUE(stv_compare(stv_literal("ab"), stv_literal("abc")) < 0);
}

/* ========================================================================== */
/*  startsWith / endsWith / contains                                          */
/* ========================================================================== */

void test_stv_starts_with_true(void) {
    strview text = stv_literal("Hello World!");
    strview pat  = stv_literal("Hello");
    TEST_ASSERT_TRUE(stv_startsWith(text, pat));
}

void test_stv_starts_with_false(void) {
    strview text = stv_literal("Hello");
    strview pat  = stv_literal("World");
    TEST_ASSERT_FALSE(stv_startsWith(text, pat));
}

void test_stv_starts_with_empty_pat(void) {
    TEST_ASSERT_TRUE(stv_startsWith(stv_literal("abc"), stv_nullstv));
}

void test_stv_ends_with(void) {
    strview text = stv_literal("document.txt");
    strview pat  = stv_literal(".txt");
    TEST_ASSERT_TRUE(stv_endsWith(text, pat));
    TEST_ASSERT_FALSE(stv_endsWith(text, stv_literal(".doc")));
}

void test_stv_ends_with_empty(void) {
    TEST_ASSERT_TRUE(stv_endsWith(stv_literal("any"), stv_nullstv));
}

void test_stv_contains(void) {
    strview text = stv_literal("the quick brown fox");
    TEST_ASSERT_TRUE(stv_contains(text, stv_literal("quick")));
    TEST_ASSERT_FALSE(stv_contains(text, stv_literal("slow")));
}

void test_stv_contains_empty_pat(void) {
    TEST_ASSERT_TRUE(stv_contains(stv_literal("abc"), stv_nullstv));
}

/* ========================================================================== */
/*  stv_same / stv_equal / stv_empty                                          */
/* ========================================================================== */

void test_stv_same(void) {
    char* data = strcpy(malloc(sizeof("abc")), "abc");
    TEST_ASSERT_TRUE(stv_same(stv_literal("abc"), stv_literal("abc")));
    TEST_ASSERT_FALSE(stv_same(stv_literal(data), stv_literal("abc")));
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
    TEST_ASSERT_TRUE(stv_empty(stv_literal("")));
    TEST_ASSERT_FALSE(stv_empty(stv_literal("a")));
}

/* ========================================================================== */
/*  stv_front / stv_back                                                      */
/* ========================================================================== */

void test_stv_front_back(void) {
    strview sv = stv_literal("Hello");
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
    strview a = stv_literal("first");
    strview b = stv_literal("second");
    stv_swap(&a, &b);
    TEST_ASSERT_EQUAL_STRING("second", a.data);
    TEST_ASSERT_EQUAL_size_t(6, a.len);
    TEST_ASSERT_EQUAL_STRING("first", b.data);
    TEST_ASSERT_EQUAL_size_t(5, b.len);
}

void test_stv_swap_null_pointers(void) {
    strview a = stv_literal("a");
    stv_swap(&a, NULL); /* should do nothing */
    TEST_ASSERT_EQUAL_STRING("a", a.data);
}

/* ========================================================================== */
/*  stv_cstr / stv_rev_cstr                                                   */
/* ========================================================================== */

void test_stv_cstr_success(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("Hello", buf);
}

void test_stv_cstr_too_small(void) {
    char    buf[3];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf));
    TEST_ASSERT_NULL(ret);
}

void test_stv_rev_cstr(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
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

    /* stv_removeStart / stv_removeEnd / stv_removePrefix / stv_removeSuffix */
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

    /* trim functions */
    RUN_TEST(test_stv_trim_both);
    RUN_TEST(test_stv_trim_start);
    RUN_TEST(test_stv_trim_end);
    RUN_TEST(test_stv_trim_no_op);
    RUN_TEST(test_stv_trim_all_chars_removed);
    RUN_TEST(test_stv_trim_empty_view);
    RUN_TEST(test_stv_trimIf_both);
    RUN_TEST(test_stv_trimIf_null_handle);
    RUN_TEST(test_stv_trimIf_all_removed);
    RUN_TEST(test_stv_trimIf_empty_view);
    RUN_TEST(test_stv_trimStartIf);
    RUN_TEST(test_stv_trimStartIf_null_handle);
    RUN_TEST(test_stv_trimStartIf_empty_view);
    RUN_TEST(test_stv_trimEndIf);
    RUN_TEST(test_stv_trimEndIf_null_handle);
    RUN_TEST(test_stv_trimEndIf_empty_view);

    /* split / beforeDelim / afterDelim */
    RUN_TEST(test_stv_split_normal);
    RUN_TEST(test_stv_split_not_found);
    RUN_TEST(test_stv_split_empty_sep);
    RUN_TEST(test_stv_split_empty_sep_single_char);
    RUN_TEST(test_stv_split_null_remaining);
    RUN_TEST(test_stv_split_sep_at_end);
    RUN_TEST(test_stv_split_sep_at_start);
    RUN_TEST(test_stv_split_empty_view);
    RUN_TEST(test_stv_before_delim_found);
    RUN_TEST(test_stv_before_delim_not_found);
    RUN_TEST(test_stv_before_delim_empty_view);
    RUN_TEST(test_stv_before_delim_empty_delim);
    RUN_TEST(test_stv_before_delim_delim_at_start);
    RUN_TEST(test_stv_after_delim_found);
    RUN_TEST(test_stv_after_delim_not_found);
    RUN_TEST(test_stv_after_delim_empty_view);
    RUN_TEST(test_stv_after_delim_empty_delim);
    RUN_TEST(test_stv_after_delim_delim_at_end);

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

    /* count / every / some */
    RUN_TEST(test_stv_count_digits);
    RUN_TEST(test_stv_count_empty);
    RUN_TEST(test_stv_count_null_handle);
    RUN_TEST(test_stv_countChar_normal);
    RUN_TEST(test_stv_countChar_empty);
    RUN_TEST(test_stv_every_digit);
    RUN_TEST(test_stv_every_not_all_digit);
    RUN_TEST(test_stv_every_empty);
    RUN_TEST(test_stv_everyChar_true);
    RUN_TEST(test_stv_everyChar_false);
    RUN_TEST(test_stv_everyChar_empty);
    RUN_TEST(test_stv_some_digit);
    RUN_TEST(test_stv_some_no_digit);
    RUN_TEST(test_stv_some_empty);
    RUN_TEST(test_stv_someChar_found);
    RUN_TEST(test_stv_someChar_not_found);
    RUN_TEST(test_stv_someChar_empty);

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
