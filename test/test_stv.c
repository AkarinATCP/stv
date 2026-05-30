/**
 * @file test_stv.c
 * @brief Unity tests for the stv.h
 */

#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
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
/*  stv_removeStart / stv_removeEnd / stv_removePrefix / stv_removeSuffix     */
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
    strview trimmed = stv_trimChs(sv, stv_whitespace);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_stv_trim_start(void) {
    strview sv      = stv_literal("///path//");
    strview charset = stv_literal("/");
    strview trimmed = stv_trimStartChs(sv, charset);
    TEST_ASSERT_EQUAL_size_t(6, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("path//", trimmed.data, 6);
}

void test_stv_trim_end(void) {
    strview sv      = stv_literal("hello---");
    strview charset = stv_literal("-");
    strview trimmed = stv_trimEndChs(sv, charset);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_stv_trim_no_op(void) {
    strview sv      = stv_literal("abc");
    strview trimmed = stv_trimChs(sv, stv_nullstv);
    TEST_ASSERT_TRUE(stv_equal(sv, trimmed));
}

void test_stv_trim_all_chars_removed(void) {
    strview sv      = stv_literal("xxxx");
    strview charset = stv_literal("x");
    strview trimmed = stv_trimChs(sv, charset);
    TEST_ASSERT_EQUAL_size_t(0, trimmed.len);
}

void test_stv_trim_empty_view(void) {
    strview sv      = stv_nullstv;
    strview trimmed = stv_trimChs(sv, stv_whitespace);
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
/*  stv_splitLines / stv_splitWords                                           */
/* ========================================================================== */

void test_stv_splitLines_lf(void) {
    strview stv = stv_literal("line1\nline2");
    strview rem;
    strview line = stv_splitLines(stv, &rem);
    TEST_ASSERT_EQUAL_size_t(5, line.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("line1", line.data, 5);
    TEST_ASSERT_EQUAL_size_t(5, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("line2", rem.data, 5);
}

void test_stv_splitLines_crlf(void) {
    strview stv = stv_literal("hello\r\nworld");
    strview rem;
    strview line = stv_splitLines(stv, &rem);
    TEST_ASSERT_EQUAL_size_t(5, line.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", line.data, 5);
    TEST_ASSERT_EQUAL_size_t(5, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("world", rem.data, 5);
}

void test_stv_splitLines_cr_only(void) {
    strview stv = stv_literal("foo\rbar");
    strview rem;
    strview line = stv_splitLines(stv, &rem);
    TEST_ASSERT_EQUAL_size_t(3, line.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("foo", line.data, 3);
    TEST_ASSERT_EQUAL_size_t(3, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("bar", rem.data, 3);
}

void test_stv_splitLines_no_break(void) {
    strview stv = stv_literal("single");
    strview rem;
    strview line = stv_splitLines(stv, &rem);
    TEST_ASSERT_TRUE(stv_equal(stv, line));
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_splitLines_trailing_newline(void) {
    strview stv = stv_literal("end\n");
    strview rem;
    strview line = stv_splitLines(stv, &rem);
    TEST_ASSERT_EQUAL_size_t(3, line.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("end", line.data, 3);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_splitLines_empty(void) {
    strview stv = stv_nullstv;
    strview rem;
    strview line = stv_splitLines(stv, &rem);
    TEST_ASSERT_TRUE(stv_empty(line));
}

void test_stv_splitLines_null_remaining(void) {
    strview stv  = stv_literal("a\nb");
    strview line = stv_splitLines(stv, NULL);
    TEST_ASSERT_EQUAL_size_t(1, line.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("a", line.data, 1);
}

void test_stv_splitWords_normal(void) {
    strview stv = stv_literal("  hello world  ");
    strview rem;
    strview word = stv_splitWords(stv, &rem);
    TEST_ASSERT_EQUAL_size_t(5, word.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", word.data, 5);
    TEST_ASSERT_EQUAL_size_t(8, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(" world  ", rem.data, 8);
}

void test_stv_splitWords_single_word(void) {
    strview stv = stv_literal("test");
    strview rem;
    strview word = stv_splitWords(stv, &rem);
    TEST_ASSERT_EQUAL_size_t(4, word.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("test", word.data, 4);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_splitWords_only_whitespace(void) {
    strview stv = stv_literal("   \t ");
    strview rem;
    strview word = stv_splitWords(stv, &rem);
    TEST_ASSERT_TRUE(stv_empty(word));
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_splitWords_empty(void) {
    strview stv = stv_nullstv;
    strview rem;
    strview word = stv_splitWords(stv, &rem);
    TEST_ASSERT_TRUE(stv_empty(word));
    /* rem unmodified */
}

void test_stv_splitWords_null_remaining(void) {
    strview stv  = stv_literal(" word");
    strview word = stv_splitWords(stv, NULL);
    TEST_ASSERT_EQUAL_size_t(4, word.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("word", word.data, 4);
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
    strview text = stv_literal("aaaaabaaaaa");
    strview pat  = stv_literal("aabaa");
    size_t  pos  = stv_search(text, pat);
    TEST_ASSERT_EQUAL_size_t(3, pos);
}

void test_stv_search_fallback_naive(void) {
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
    TEST_ASSERT_EQUAL_size_t(0, stv_firstChar(sv, 'h', false));
    TEST_ASSERT_EQUAL_size_t(4, stv_firstChar(sv, 'o', false));
}

void test_stv_first_char_not_found(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChar(sv, 'x', false));
}

void test_stv_first_not_char(void) {
    strview sv = stv_literal("aaabc");
    TEST_ASSERT_EQUAL_size_t(3, stv_firstChar(sv, 'a', true));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChar(stv_literal("aaa"), 'a', true));
}

void test_stv_last_char(void) {
    strview sv = stv_literal("abracadabra");
    TEST_ASSERT_EQUAL_size_t(10, stv_lastChar(sv, 'a', false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastChar(sv, 'z', false));
}

void test_stv_last_not_char(void) {
    strview sv = stv_literal("hello---");
    TEST_ASSERT_EQUAL_size_t(4, stv_lastChar(sv, '-', true));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastChar(stv_literal("---"), '-', true));
}

void test_stv_char_empty_view(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChar(sv, 'x', false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstChar(sv, 'x', true));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastChar(sv, 'x', false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastChar(sv, 'x', true));
}

void test_stv_firstCharset_found(void) {
    strview sv = stv_literal("abc123");
    TEST_ASSERT_EQUAL_size_t(3, stv_firstCharset(sv, stv_literal("0123456789"), false));
    TEST_ASSERT_EQUAL_size_t(0, stv_firstCharset(sv, stv_literal("0123456789"), true));
}

void test_stv_firstCharset_not_found(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstCharset(sv, stv_literal("123"), false));
}

void test_stv_lastCharset(void) {
    strview sv = stv_literal("abc123def456");
    TEST_ASSERT_EQUAL_size_t(11, stv_lastCharset(sv, stv_literal("0123456789"), false));
    TEST_ASSERT_EQUAL_size_t(8, stv_lastCharset(sv, stv_literal("0123456789"), true));
}

void test_stv_firstCharClass_digit(void) {
    strview sv = stv_literal("abc123");
    TEST_ASSERT_EQUAL_size_t(3, stv_firstCharClass(sv, isdigit, false));
    TEST_ASSERT_EQUAL_size_t(0, stv_firstCharClass(sv, isdigit, true));
}

void test_stv_lastCharClass_digit(void) {
    strview sv = stv_literal("123abc456");
    TEST_ASSERT_EQUAL_size_t(8, stv_lastCharClass(sv, isdigit, false));
    TEST_ASSERT_EQUAL_size_t(5, stv_lastCharClass(sv, isdigit, true));
}

void test_stv_firstCharset_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstCharset(sv, stv_whitespace, false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastCharset(sv, stv_whitespace, false));
}

void test_stv_firstCharClass_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_firstCharClass(sv, isdigit, false));
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_lastCharClass(sv, isdigit, false));
}

/* ========================================================================== */
/*  Generic macro tests                                                       */
/* ========================================================================== */

#ifdef LIB_STV_GENERIC
void test_stv_trim_generic_charset(void) {
    strview sv      = stv_literal("  hello  ");
    strview trimmed = stv_trim(sv, stv_whitespace);
    TEST_ASSERT_EQUAL_size_t(5, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", trimmed.data, 5);
}

void test_stv_trim_generic_fn(void) {
    strview sv      = stv_literal("123abc456");
    strview trimmed = stv_trim(sv, isdigit);
    TEST_ASSERT_EQUAL_size_t(3, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abc", trimmed.data, 3);
}

void test_stv_trimStart_generic(void) {
    strview sv      = stv_literal("///path");
    strview trimmed = stv_trimStart(sv, stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(4, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("path", trimmed.data, 4);
}

void test_stv_trimEnd_generic(void) {
    strview sv      = stv_literal("path///");
    strview trimmed = stv_trimEnd(sv, stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(4, trimmed.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("path", trimmed.data, 4);
}

void test_stv_firstIndex_generic(void) {
    strview sv = stv_literal("hello123");
    TEST_ASSERT_EQUAL_size_t(5, stv_firstIndex(sv, stv_literal("0123456789"), false));
    TEST_ASSERT_EQUAL_size_t(0, stv_firstIndex(sv, 'h', false));
    TEST_ASSERT_EQUAL_size_t(5, stv_firstIndex(sv, isdigit, false));
}

void test_stv_lastIndex_generic(void) {
    strview sv = stv_literal("123hello456");
    TEST_ASSERT_EQUAL_size_t(10, stv_lastIndex(sv, stv_literal("456"), false));
    TEST_ASSERT_EQUAL_size_t(7, stv_lastIndex(sv, 'o', false));
    TEST_ASSERT_EQUAL_size_t(10, stv_lastIndex(sv, isdigit, false));
}

void test_stv_count_generic(void) {
    strview sv = stv_literal("a1b2c3");
    TEST_ASSERT_EQUAL_size_t(3, stv_count(sv, isdigit));
    TEST_ASSERT_EQUAL_size_t(1, stv_count(sv, '2'));
    TEST_ASSERT_EQUAL_size_t(1, stv_count(sv, stv_literal("1b")));
}

void test_stv_every_generic(void) {
    strview sv = stv_literal("12345");
    TEST_ASSERT_TRUE(stv_every(sv, isdigit));
    TEST_ASSERT_FALSE(stv_every(sv, '1'));
}

void test_stv_some_generic(void) {
    strview sv = stv_literal("abc1");
    TEST_ASSERT_TRUE(stv_some(sv, isdigit));
    TEST_ASSERT_FALSE(stv_some(sv, 'x'));
}
#endif /* ifdef  LIB_STV_GENERIC */

/* ========================================================================== */
/*  Hash Functions                                                            */
/* ========================================================================== */

void test_stv_hash_empty(void) {
    TEST_ASSERT_EQUAL_size_t(0, stv_hash(stv_nullstv));
    TEST_ASSERT_EQUAL_size_t(0, stv_hash_FNV1a(stv_nullstv));
}

void test_stv_hash_nonempty(void) {
    strview sv = stv_literal("hello");
    size_t  h  = stv_hash(sv);
    TEST_ASSERT_NOT_EQUAL(0, h);
    TEST_ASSERT_EQUAL_size_t(h, stv_hash_FNV1a(sv));
}

void test_stv_hash_consistent(void) {
    strview a = stv_literal("test");
    strview b = stv_literal("test");
    TEST_ASSERT_EQUAL_size_t(stv_hash(a), stv_hash(b));
}

void test_stv_hash_different(void) {
    /* not guaranteed but highly likely different */
    strview a = stv_literal("abc");
    strview b = stv_literal("xyz");
    TEST_ASSERT_TRUE(stv_hash(a) != stv_hash(b) || stv_equal(a, b));
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
/*  count / countChar / countSubstr                                           */
/* ========================================================================== */

void test_stv_count_digits(void) {
    strview sv = stv_literal("abc123def456");
    TEST_ASSERT_EQUAL_size_t(6, stv_countIf(sv, isdigit));
}

void test_stv_count_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_countIf(sv, isdigit));
}

void test_stv_count_null_handle(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_EQUAL_size_t(stv_npos, stv_countIf(sv, NULL));
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
    TEST_ASSERT_TRUE(stv_everyIf(sv, isdigit));
}

void test_stv_every_not_all_digit(void) {
    strview sv = stv_literal("123a");
    TEST_ASSERT_FALSE(stv_everyIf(sv, isdigit));
}

void test_stv_every_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_FALSE(stv_everyIf(sv, isdigit));
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
    TEST_ASSERT_TRUE(stv_someIf(sv, isdigit));
}

void test_stv_some_no_digit(void) {
    strview sv = stv_literal("abcdef");
    TEST_ASSERT_FALSE(stv_someIf(sv, isdigit));
}

void test_stv_some_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_FALSE(stv_someIf(sv, isdigit));
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
    char    buf1[] = "abc";
    char    buf2[] = "abc";
    strview a      = stv_makestv(buf1, 3);
    strview b      = stv_makestv(buf1, 3);
    strview c      = stv_makestv(buf2, 3);
    TEST_ASSERT_TRUE(stv_same(a, b));
    TEST_ASSERT_FALSE(stv_same(a, c));
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
/*  stv_front / stv_back / stv_at                                             */
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

void test_stv_at_valid(void) {
    strview sv = stv_literal("Hello");
    TEST_ASSERT_EQUAL_CHAR('H', stv_at(sv, 0));
    TEST_ASSERT_EQUAL_CHAR('o', stv_at(sv, 4));
}

void test_stv_at_out_of_bounds(void) {
    strview sv = stv_literal("Hi");
    TEST_ASSERT_EQUAL_CHAR('\0', stv_at(sv, 2));
    TEST_ASSERT_EQUAL_CHAR('\0', stv_at(sv, 100));
}

void test_stv_at_empty(void) {
    strview sv = stv_nullstv;
    TEST_ASSERT_EQUAL_CHAR('\0', stv_at(sv, 0));
}

/* ========================================================================== */
/*  stv_forEach                                                               */
/* ========================================================================== */

static int     foreach_call_count;
static char    foreach_last_char;
static size_t  foreach_last_index;
static strview foreach_last_view;

static void callback_foreach_test(char ch, size_t idx, strview ctx) {
    foreach_call_count++;
    foreach_last_char  = ch;
    foreach_last_index = idx;
    foreach_last_view  = ctx;
}

static void callback_foreach_empty(char ch, size_t idx, strview ctx) {
    TEST_FAIL_MESSAGE("forEach callback should not be called on empty view");
}

void test_stv_forEach_empty(void) {
    stv_forEach(stv_nullstv, callback_foreach_empty);
}

void test_stv_forEach_normal(void) {
    strview sv         = stv_literal("abc");
    foreach_call_count = 0;
    stv_forEach(sv, callback_foreach_test);

    TEST_ASSERT_EQUAL_INT(3, foreach_call_count);
    TEST_ASSERT_EQUAL_CHAR('c', foreach_last_char);
    TEST_ASSERT_EQUAL_size_t(2, foreach_last_index);
    TEST_ASSERT_TRUE(stv_equal(sv, foreach_last_view));
}

void test_stv_forEach_single_char(void) {
    strview sv         = stv_literal("X");
    foreach_call_count = 0;
    stv_forEach(sv, callback_foreach_test);

    TEST_ASSERT_EQUAL_INT(1, foreach_call_count);
    TEST_ASSERT_EQUAL_CHAR('X', foreach_last_char);
    TEST_ASSERT_EQUAL_size_t(0, foreach_last_index);
    TEST_ASSERT_TRUE(stv_equal(sv, foreach_last_view));
}

void test_stv_forEachRev_normal(void) {
    strview sv         = stv_literal("abc");
    foreach_call_count = 0;
    stv_forEachRev(sv, callback_foreach_test);

    TEST_ASSERT_EQUAL_INT(3, foreach_call_count);
    TEST_ASSERT_EQUAL_CHAR('a', foreach_last_char);
    TEST_ASSERT_EQUAL_size_t(0, foreach_last_index);
    TEST_ASSERT_TRUE(stv_equal(sv, foreach_last_view));
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
    stv_swap(&a, NULL);
    TEST_ASSERT_EQUAL_STRING("a", a.data);
}

/* ========================================================================== */
/*  stv_cstr / stv_opt_cstr (advanced options)                                */
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

void test_stv_opt_cstr_reverse(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_opt_cstr(sv, buf, sizeof(buf), stv_Reverse);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("olleH", buf);
}

void test_stv_opt_cstr_upper(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_opt_cstr(sv, buf, sizeof(buf), stv_ToUpper);
    TEST_ASSERT_EQUAL_STRING("HELLO", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_stv_opt_cstr_lower(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_opt_cstr(sv, buf, sizeof(buf), stv_ToLower);
    TEST_ASSERT_EQUAL_STRING("hello", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_stv_opt_cstr_reverse_upper(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_opt_cstr(sv, buf, sizeof(buf), stv_Reverse | stv_ToUpper);
    TEST_ASSERT_EQUAL_STRING("OLLEH", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_stv_opt_cstr_swapcase(void) {
    char    buf[6];
    strview sv  = stv_literal("Str%1");
    char*   ret = stv_opt_cstr(sv, buf, sizeof(buf), stv_ToUpper | stv_ToLower);
    TEST_ASSERT_EQUAL_STRING("sTR%1", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_stv_cstr_empty_view(void) {
    char    buf[2] = {0};
    strview sv     = stv_nullstv;
    char*   ret    = stv_cstr(sv, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

/* ========================================================================== */
/*  stv_opt_join                                                              */
/* ========================================================================== */

void test_stv_opt_join_empty_array(void) {
    char    buf[4] = {0};
    strview sep    = stv_literal(",");
    char*   ret    = stv_opt_join(NULL, 0, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_stv_opt_join_null_array_with_zero_len(void) {
    char  buf[1] = {0};
    char* ret    = stv_opt_join(NULL, 0, buf, sizeof(buf), stv_nullstv, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_stv_opt_join_null_array_with_nonzero_len(void) {
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_opt_join(NULL, 1, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_NULL(ret);
}

void test_stv_opt_join_single_element(void) {
    strview arr[] = {stv_literal("Hello")};
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_opt_join(arr, 1, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("Hello", buf);
}

void test_stv_opt_join_multiple_elements(void) {
    strview arr[] = {stv_literal("a"), stv_literal("b"), stv_literal("c")};
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_opt_join(arr, 3, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("a,b,c", buf);
}

void test_stv_opt_join_empty_separator(void) {
    strview arr[] = {stv_literal("1"), stv_literal("2"), stv_literal("3")};
    char    buf[10];
    strview sep = stv_nullstv;
    char*   ret = stv_opt_join(arr, 3, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("123", buf);
}

void test_stv_opt_join_with_empty_elements(void) {
    strview arr[] = {stv_literal("a"), stv_nullstv, stv_literal("c")};
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_opt_join(arr, 3, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("a,,c", buf);
}

void test_stv_opt_join_with_options(void) {
    strview arr[] = {stv_literal("Hello"), stv_literal("World")};
    char    buf[20];
    strview sep = stv_literal("-");
    char*   ret = stv_opt_join(arr, 2, buf, sizeof(buf), sep, stv_ToUpper);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("HELLO-WORLD", buf);
}

void test_stv_opt_join_reverse_elements(void) {
    strview arr[] = {stv_literal("abc"), stv_literal("123")};
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_opt_join(arr, 2, buf, sizeof(buf), sep, stv_Reverse);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("cba,321", buf);
}

void test_stv_opt_join_buffer_too_small(void) {
    strview arr[] = {stv_literal("long"), stv_literal("string")};
    char    buf[5]; /* too small */
    strview sep = stv_literal(",");
    char*   ret = stv_opt_join(arr, 2, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_NULL(ret);
}

void test_stv_opt_join_buffer_exact_size(void) {
    strview arr[] = {stv_literal("ab"), stv_literal("cd")};
    char    buf[6];
    strview sep = stv_literal(",");
    char*   ret = stv_opt_join(arr, 2, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("ab,cd", buf);
}

void test_stv_opt_join_null_mem(void) {
    strview arr[] = {stv_literal("x")};
    strview sep   = stv_literal(",");
    char*   ret   = stv_opt_join(arr, 1, NULL, 10, sep, stv_Default);
    TEST_ASSERT_NULL(ret);
}

void test_stv_opt_join_separator_with_non_ascii(void) {
    strview arr[] = {stv_literal("hello"), stv_literal("world")};
    char    buf[20];
    strview sep = stv_literal(" - ");
    char*   ret = stv_opt_join(arr, 2, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("hello - world", buf);
}

/* ========================================================================== */
/*  stv_ch2digit                                                              */
/* ========================================================================== */

void test_stv_ch2digit_digits(void) {
    TEST_ASSERT_EQUAL_INT(0, stv_ch2digit('0'));
    TEST_ASSERT_EQUAL_INT(9, stv_ch2digit('9'));
}

void test_stv_ch2digit_uppercase(void) {
    TEST_ASSERT_EQUAL_INT(10, stv_ch2digit('A'));
    TEST_ASSERT_EQUAL_INT(35, stv_ch2digit('Z'));
}

void test_stv_ch2digit_lowercase(void) {
    TEST_ASSERT_EQUAL_INT(10, stv_ch2digit('a'));
    TEST_ASSERT_EQUAL_INT(35, stv_ch2digit('z'));
}

void test_stv_ch2digit_invalid(void) {
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('/'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit(':'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('@'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('['));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('`'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('{'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit(' '));
}

/* ========================================================================== */
/*  stv_parseIntBase                                                          */
/* ========================================================================== */

void test_stv_parseIntBase_empty(void) {
    strview rem;
    int     base = stv_parseIntBase(stv_nullstv, &rem);
    TEST_ASSERT_EQUAL_INT(0, base);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseIntBase_no_prefix(void) {
    strview sv = stv_literal("123");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
    TEST_ASSERT_EQUAL_STRING("123", rem.data);
    TEST_ASSERT_EQUAL_size_t(3, rem.len);
}

void test_stv_parseIntBase_single_zero(void) {
    strview sv = stv_literal("0");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
    TEST_ASSERT_EQUAL_STRING("0", rem.data);
    TEST_ASSERT_EQUAL_size_t(1, rem.len);
}

void test_stv_parseIntBase_0b(void) {
    strview sv = stv_literal("0b101");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(2, base);
    TEST_ASSERT_EQUAL_STRING("101", rem.data);
    TEST_ASSERT_EQUAL_size_t(3, rem.len);
}

void test_stv_parseIntBase_0B(void) {
    strview sv = stv_literal("0B101");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(2, base);
    TEST_ASSERT_EQUAL_STRING("101", rem.data);
}

void test_stv_parseIntBase_0o(void) {
    strview sv = stv_literal("0o77");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(8, base);
    TEST_ASSERT_EQUAL_STRING("77", rem.data);
}

void test_stv_parseIntBase_0O(void) {
    strview sv = stv_literal("0O77");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(8, base);
    TEST_ASSERT_EQUAL_STRING("77", rem.data);
}

void test_stv_parseIntBase_0d(void) {
    strview sv = stv_literal("0d99");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
    TEST_ASSERT_EQUAL_STRING("99", rem.data);
}

void test_stv_parseIntBase_0D(void) {
    strview sv = stv_literal("0D99");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
    TEST_ASSERT_EQUAL_STRING("99", rem.data);
}

void test_stv_parseIntBase_0x(void) {
    strview sv = stv_literal("0xFF");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(16, base);
    TEST_ASSERT_EQUAL_STRING("FF", rem.data);
}

void test_stv_parseIntBase_0X(void) {
    strview sv = stv_literal("0XFF");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(16, base);
    TEST_ASSERT_EQUAL_STRING("FF", rem.data);
}

void test_stv_parseIntBase_0_but_no_known_prefix(void) {
    strview sv = stv_literal("077");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
    TEST_ASSERT_EQUAL_STRING("077", rem.data);
}

void test_stv_parseIntBase_0_followed_by_invalid(void) {
    strview sv = stv_literal("0?");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
    TEST_ASSERT_EQUAL_STRING("0?", rem.data);
}

/* ========================================================================== */
/*  stv_parseInum (signed)                                                    */
/* ========================================================================== */

void test_stv_parseInum_decimal_positive(void) {
    strview  sv = stv_literal("42");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(42, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_decimal_negative(void) {
    strview  sv = stv_literal("-42");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(-42, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_plus_sign(void) {
    strview  sv = stv_literal("+99");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(99, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_whitespace_skip(void) {
    strview  sv = stv_literal("   -123");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(-123, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_auto_base_bin(void) {
    strview  sv = stv_literal("0b1101");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_INT64(13, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_auto_base_oct(void) {
    strview  sv = stv_literal("0o77");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_INT64(63, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_auto_base_hex(void) {
    strview  sv = stv_literal("0xFF");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_INT64(255, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_auto_base_dec_no_prefix(void) {
    strview  sv = stv_literal("077");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_INT64(77, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_explicit_base_16(void) {
    strview  sv = stv_literal("0xFF");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 16, &rem);
    TEST_ASSERT_EQUAL_INT64(255, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_trailing_chars(void) {
    strview  sv = stv_literal("123abc");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(123, val);
    TEST_ASSERT_EQUAL_STRING("abc", rem.data);
    TEST_ASSERT_EQUAL_size_t(3, rem.len);
}

void test_stv_parseInum_empty_string(void) {
    strview  rem;
    intmax_t val = stv_parseInum(stv_nullstv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(0, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_only_whitespace(void) {
    strview  sv = stv_literal("  \t ");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(0, val);
    TEST_ASSERT_EQUAL_STRING("", rem.data);
    TEST_ASSERT_EQUAL_size_t(0, rem.len);
}

void test_stv_parseInum_only_sign(void) {
    strview  sv = stv_literal("-");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(0, val);
    TEST_ASSERT_EQUAL_STRING("", rem.data);
    TEST_ASSERT_EQUAL_size_t(0, rem.len);
}

void test_stv_parseInum_invalid_base(void) {
    strview  sv = stv_literal("123");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 1, &rem);
    TEST_ASSERT_EQUAL_INT64(0, val);
    TEST_ASSERT_EQUAL_STRING("123", rem.data);
}

void test_stv_parseInum_overflow_positive(void) {
    strview  sv = stv_literal("9223372036854775808");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(INTMAX_MAX, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_overflow_negative(void) {
    strview  sv = stv_literal("-9223372036854775809");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(INTMAX_MIN, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseInum_overflow_with_trailing(void) {
    strview  sv = stv_literal("99999999999999999999abc");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(INTMAX_MAX, val);
    TEST_ASSERT_EQUAL_STRING("abc", rem.data);
}

/* ========================================================================== */
/*  stv_parseUnum (unsigned)                                                  */
/* ========================================================================== */

void test_stv_parseUnum_normal(void) {
    strview   sv = stv_literal("12345");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_UINT64(12345, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseUnum_negative_wraps(void) {
    strview   sv = stv_literal("-40");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 10, &rem);
    /* -(uintmax_t)40 is UINTMAX_MAX - 39 */
    TEST_ASSERT_EQUAL_UINT64(UINTMAX_MAX - 39, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseUnum_hex(void) {
    strview   sv = stv_literal("0xDEAD");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_UINT64(0xDEAD, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseUnum_overflow(void) {
    /* UINT64_MAX + 1 would overflow */
    strview   sv = stv_literal("18446744073709551616");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_UINT64(UINTMAX_MAX, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseUnum_overflow_negative(void) {
    strview   sv = stv_literal("-99999999999999999999");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_UINT64(UINTMAX_MAX, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_parseUnum_empty(void) {
    strview   rem;
    uintmax_t val = stv_parseUnum(stv_nullstv, 10, &rem);
    TEST_ASSERT_EQUAL_UINT64(0, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

/* ========================================================================== */
/*  stv_LIST / stv_PFARG / stv_PFFMT                                          */
/* ========================================================================== */

void test_stv_LIST_macro(void) {
    strview sv1 = stv_literal("a"), sv2 = stv_literal("b"), sv3 = stv_literal("c");
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_opt_join(stv_LIST(sv1, sv2, sv3), buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("a,b,c", buf);
}

void test_stv_printf_macro(void) {
    strview sv = stv_literal("test");
    char    buf[20];
    sprintf(buf, "[" stv_PFFMT "]", stv_PFARG(sv));
    TEST_ASSERT_EQUAL_STRING("[test]", buf);
}

void test_stv_printf_macro_empty(void) {
    strview sv = stv_nullstv;
    char    buf[20];
    sprintf(buf, "[" stv_PFFMT "]", stv_PFARG(sv));
    TEST_ASSERT_EQUAL_STRING("[]", buf);
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

    /* splitLines */
    RUN_TEST(test_stv_splitLines_lf);
    RUN_TEST(test_stv_splitLines_crlf);
    RUN_TEST(test_stv_splitLines_cr_only);
    RUN_TEST(test_stv_splitLines_no_break);
    RUN_TEST(test_stv_splitLines_trailing_newline);
    RUN_TEST(test_stv_splitLines_empty);
    RUN_TEST(test_stv_splitLines_null_remaining);

    /* splitWords */
    RUN_TEST(test_stv_splitWords_normal);
    RUN_TEST(test_stv_splitWords_single_word);
    RUN_TEST(test_stv_splitWords_only_whitespace);
    RUN_TEST(test_stv_splitWords_empty);
    RUN_TEST(test_stv_splitWords_null_remaining);

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
    RUN_TEST(test_stv_firstCharset_found);
    RUN_TEST(test_stv_firstCharset_not_found);
    RUN_TEST(test_stv_lastCharset);
    RUN_TEST(test_stv_firstCharClass_digit);
    RUN_TEST(test_stv_lastCharClass_digit);
    RUN_TEST(test_stv_firstCharset_empty);
    RUN_TEST(test_stv_firstCharClass_empty);

#ifdef LIB_STV_GENERIC
    /* generic macros */
    RUN_TEST(test_stv_trim_generic_charset);
    RUN_TEST(test_stv_trim_generic_fn);
    RUN_TEST(test_stv_trimStart_generic);
    RUN_TEST(test_stv_trimEnd_generic);
    RUN_TEST(test_stv_firstIndex_generic);
    RUN_TEST(test_stv_lastIndex_generic);
    RUN_TEST(test_stv_count_generic);
    RUN_TEST(test_stv_every_generic);
    RUN_TEST(test_stv_some_generic);
#endif /* ifdef LIB_STV_GENERIC */

    /* hash functions */
    RUN_TEST(test_stv_hash_empty);
    RUN_TEST(test_stv_hash_nonempty);
    RUN_TEST(test_stv_hash_consistent);
    RUN_TEST(test_stv_hash_different);

    /* firstDiff / lastDiff */
    RUN_TEST(test_stv_first_diff_same);
    RUN_TEST(test_stv_first_diff_at_start);
    RUN_TEST(test_stv_first_diff_length_mismatch);
    RUN_TEST(test_stv_last_diff);
    RUN_TEST(test_stv_last_diff_length_mismatch);
    RUN_TEST(test_stv_diff_with_empty);
    RUN_TEST(test_stv_diff_both_empty);

    /* count / countChar / countSubstr */
    RUN_TEST(test_stv_count_digits);
    RUN_TEST(test_stv_count_empty);
    RUN_TEST(test_stv_count_null_handle);
    RUN_TEST(test_stv_countChar_normal);
    RUN_TEST(test_stv_countChar_empty);
    RUN_TEST(test_stv_countSubstr_normal);
    RUN_TEST(test_stv_countSubstr_empty);

    /* every / everyChar / some / someChar */
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
    RUN_TEST(test_stv_at_valid);
    RUN_TEST(test_stv_at_out_of_bounds);
    RUN_TEST(test_stv_at_empty);

    /* stv_forEach */
    RUN_TEST(test_stv_forEach_empty);
    RUN_TEST(test_stv_forEach_normal);
    RUN_TEST(test_stv_forEach_single_char);
    RUN_TEST(test_stv_forEachRev_normal);

    /* swap */
    RUN_TEST(test_stv_swap);
    RUN_TEST(test_stv_swap_null_pointers);

    /* cstr / opt_cstr */
    RUN_TEST(test_stv_cstr_success);
    RUN_TEST(test_stv_cstr_too_small);
    RUN_TEST(test_stv_opt_cstr_reverse);
    RUN_TEST(test_stv_opt_cstr_upper);
    RUN_TEST(test_stv_opt_cstr_lower);
    RUN_TEST(test_stv_opt_cstr_reverse_upper);
    RUN_TEST(test_stv_opt_cstr_swapcase);
    RUN_TEST(test_stv_cstr_empty_view);

    /* stv_opt_join */
    RUN_TEST(test_stv_opt_join_empty_array);
    RUN_TEST(test_stv_opt_join_null_array_with_zero_len);
    RUN_TEST(test_stv_opt_join_null_array_with_nonzero_len);
    RUN_TEST(test_stv_opt_join_single_element);
    RUN_TEST(test_stv_opt_join_multiple_elements);
    RUN_TEST(test_stv_opt_join_empty_separator);
    RUN_TEST(test_stv_opt_join_with_empty_elements);
    RUN_TEST(test_stv_opt_join_with_options);
    RUN_TEST(test_stv_opt_join_reverse_elements);
    RUN_TEST(test_stv_opt_join_buffer_too_small);
    RUN_TEST(test_stv_opt_join_buffer_exact_size);
    RUN_TEST(test_stv_opt_join_null_mem);
    RUN_TEST(test_stv_opt_join_separator_with_non_ascii);

    /* stv_ch2digit */
    RUN_TEST(test_stv_ch2digit_digits);
    RUN_TEST(test_stv_ch2digit_uppercase);
    RUN_TEST(test_stv_ch2digit_lowercase);
    RUN_TEST(test_stv_ch2digit_invalid);

    /* stv_parseIntBase */
    RUN_TEST(test_stv_parseIntBase_empty);
    RUN_TEST(test_stv_parseIntBase_no_prefix);
    RUN_TEST(test_stv_parseIntBase_single_zero);
    RUN_TEST(test_stv_parseIntBase_0b);
    RUN_TEST(test_stv_parseIntBase_0B);
    RUN_TEST(test_stv_parseIntBase_0o);
    RUN_TEST(test_stv_parseIntBase_0O);
    RUN_TEST(test_stv_parseIntBase_0d);
    RUN_TEST(test_stv_parseIntBase_0D);
    RUN_TEST(test_stv_parseIntBase_0x);
    RUN_TEST(test_stv_parseIntBase_0X);
    RUN_TEST(test_stv_parseIntBase_0_but_no_known_prefix);
    RUN_TEST(test_stv_parseIntBase_0_followed_by_invalid);

    /* stv_parseInum */
    RUN_TEST(test_stv_parseInum_decimal_positive);
    RUN_TEST(test_stv_parseInum_decimal_negative);
    RUN_TEST(test_stv_parseInum_plus_sign);
    RUN_TEST(test_stv_parseInum_whitespace_skip);
    RUN_TEST(test_stv_parseInum_auto_base_bin);
    RUN_TEST(test_stv_parseInum_auto_base_oct);
    RUN_TEST(test_stv_parseInum_auto_base_hex);
    RUN_TEST(test_stv_parseInum_auto_base_dec_no_prefix);
    RUN_TEST(test_stv_parseInum_explicit_base_16);
    RUN_TEST(test_stv_parseInum_trailing_chars);
    RUN_TEST(test_stv_parseInum_empty_string);
    RUN_TEST(test_stv_parseInum_only_whitespace);
    RUN_TEST(test_stv_parseInum_only_sign);
    RUN_TEST(test_stv_parseInum_invalid_base);
    RUN_TEST(test_stv_parseInum_overflow_positive);
    RUN_TEST(test_stv_parseInum_overflow_negative);
    RUN_TEST(test_stv_parseInum_overflow_with_trailing);

    /* stv_parseUnum */
    RUN_TEST(test_stv_parseUnum_normal);
    RUN_TEST(test_stv_parseUnum_negative_wraps);
    RUN_TEST(test_stv_parseUnum_hex);
    RUN_TEST(test_stv_parseUnum_overflow);
    RUN_TEST(test_stv_parseUnum_overflow_negative);
    RUN_TEST(test_stv_parseUnum_empty);

    /* macros */
    RUN_TEST(test_stv_LIST_macro);
    RUN_TEST(test_stv_printf_macro);
    RUN_TEST(test_stv_printf_macro_empty);

    return UNITY_END();
}
