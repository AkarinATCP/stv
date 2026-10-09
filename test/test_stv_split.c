/**
 * @file test_stv_split.c
 * @brief test Splitting
 */

#include "stv.h"
#include "unity/unity.h"

/* ========== stv_split ========== */
void test_stv_split_normal(void) {
    strview stv = stv_literal("hello world");
    strview sep = stv_literal(" ");
    strview rem;
    strview first = stv_split(stv, sep, &rem, false);
    TEST_ASSERT_EQUAL_size_t(5, first.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", first.data, 5);
    TEST_ASSERT_EQUAL_size_t(5, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("world", rem.data, 5);
}

void test_stv_split_nocase(void) {
    strview stv = stv_literal("hello WORLD");
    strview sep = stv_literal("world");
    strview rem;
    strview first = stv_split(stv, sep, &rem, true);
    TEST_ASSERT_EQUAL_size_t(6, first.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello ", first.data, 6);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_split_not_found(void) {
    strview stv = stv_literal("hello");
    strview rem;
    strview first = stv_split(stv, stv_literal("/"), &rem, false);
    TEST_ASSERT_TRUE(stv_equal(stv, first));
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_split_empty_sep(void) {
    strview stv = stv_literal("ab");
    strview rem;
    strview first = stv_split(stv, stv_nullstv, &rem, false);
    TEST_ASSERT_EQUAL_size_t(1, first.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("a", first.data, 1);
    TEST_ASSERT_EQUAL_size_t(1, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("b", rem.data, 1);
}

void test_stv_split_empty_sep_single_char(void) {
    strview stv = stv_literal("a");
    strview rem;
    strview first = stv_split(stv, stv_nullstv, &rem, false);
    TEST_ASSERT_EQUAL_size_t(1, first.len);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_split_null_remaining(void) {
    strview first = stv_split(stv_literal("a,b"), stv_literal(","), NULL, false);
    TEST_ASSERT_EQUAL_size_t(1, first.len);
}

void test_stv_split_sep_at_end(void) {
    strview stv = stv_literal("hello,");
    strview rem;
    strview first = stv_split(stv, stv_literal(","), &rem, false);
    TEST_ASSERT_EQUAL_size_t(5, first.len);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_stv_split_sep_at_start(void) {
    strview stv = stv_literal(",world");
    strview rem;
    strview first = stv_split(stv, stv_literal(","), &rem, false);
    TEST_ASSERT_EQUAL_size_t(0, first.len);
    TEST_ASSERT_EQUAL_size_t(5, rem.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("world", rem.data, 5);
}

void test_stv_split_empty_view(void) {
    strview first = stv_split(stv_nullstv, stv_literal(","), NULL, false);
    TEST_ASSERT_TRUE(stv_empty(first));
}

/* ========== stv_splitLines ========== */
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
    strview rem;
    TEST_ASSERT_TRUE(stv_empty(stv_splitLines(stv_nullstv, &rem)));
}

void test_stv_splitLines_null_remaining(void) {
    strview line = stv_splitLines(stv_literal("a\nb"), NULL);
    TEST_ASSERT_EQUAL_size_t(1, line.len);
}

/* ========== stv_splitWords ========== */
void test_stv_splitWords_normal(void) {
    strview stv = stv_literal("  hello world  ");
    strview rem;
    strview word = stv_splitWords(stv, &rem);
    TEST_ASSERT_EQUAL_size_t(5, word.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hello", word.data, 5);
    TEST_ASSERT_EQUAL_size_t(8, rem.len); /* " world  " */
}

void test_stv_splitWords_single_word(void) {
    strview stv = stv_literal("test");
    strview rem;
    strview word = stv_splitWords(stv, &rem);
    TEST_ASSERT_EQUAL_size_t(4, word.len);
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
    strview rem;
    TEST_ASSERT_TRUE(stv_empty(stv_splitWords(stv_nullstv, &rem)));
}

void test_stv_splitWords_null_remaining(void) {
    strview word = stv_splitWords(stv_literal(" word"), NULL);
    TEST_ASSERT_EQUAL_size_t(4, word.len);
}

/* ========== before / after 系列 ========== */
void test_stv_before_delim_found(void) {
    strview before = stv_beforeFirstDelim(stv_literal("key=value"), stv_literal("="));
    TEST_ASSERT_EQUAL_size_t(3, before.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("key", before.data, 3);
}

void test_stv_before_delim_not_found(void) {
    strview stv = stv_literal("hello");
    TEST_ASSERT_TRUE(stv_equal(stv, stv_beforeFirstDelim(stv, stv_literal(":"))));
}

void test_stv_before_delim_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_beforeFirstDelim(stv_nullstv, stv_literal(":"))));
}

void test_stv_before_delim_empty_delim(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_beforeFirstDelim(stv_literal("abc"), stv_nullstv)));
}

void test_stv_before_delim_delim_at_start(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_beforeFirstDelim(stv_literal("!abc"), stv_literal("!"))));
}

void test_stv_after_delim_found(void) {
    strview after = stv_afterFirstDelim(stv_literal("path/to/file"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(7, after.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("to/file", after.data, 7);
}

void test_stv_after_delim_not_found(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_afterFirstDelim(stv_literal("no_slash"), stv_literal("/"))));
}

void test_stv_after_delim_empty_delim(void) {
    strview sv = stv_literal("any");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_afterFirstDelim(sv, stv_nullstv)));
}

void test_stv_after_delim_delim_at_end(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_afterFirstDelim(stv_literal("abc/"), stv_literal("/"))));
}

void test_stv_beforeLastDelim_found(void) {
    strview before = stv_beforeLastDelim(stv_literal("path/to/file.txt"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(7, before.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("path/to", before.data, 7);
}

void test_stv_beforeLastDelim_not_found(void) {
    strview sv = stv_literal("no_delimiter");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_beforeLastDelim(sv, stv_literal("/"))));
}

void test_stv_beforeLastDelim_at_start(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_beforeLastDelim(stv_literal("/abc"), stv_literal("/"))));
}

void test_stv_beforeLastDelim_at_end(void) {
    strview before = stv_beforeLastDelim(stv_literal("abc/"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(3, before.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abc", before.data, 3);
}

void test_stv_beforeLastDelim_empty_delim(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_beforeLastDelim(stv_literal("abc"), stv_nullstv)));
}

void test_stv_beforeLastDelim_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_beforeLastDelim(stv_nullstv, stv_literal("/"))));
}

void test_stv_beforeLastDelim_multiple(void) {
    strview before = stv_beforeLastDelim(stv_literal("a/b/c"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(3, before.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("a/b", before.data, 3);
}

void test_stv_afterLastDelim_found(void) {
    strview after = stv_afterLastDelim(stv_literal("path/to/file.txt"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(8, after.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("file.txt", after.data, 8);
}

void test_stv_afterLastDelim_not_found(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_afterLastDelim(stv_literal("no_delim"), stv_literal("/"))));
}

void test_stv_afterLastDelim_at_end(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_afterLastDelim(stv_literal("abc/"), stv_literal("/"))));
}

void test_stv_afterLastDelim_at_start(void) {
    strview after = stv_afterLastDelim(stv_literal("/abc"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(3, after.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abc", after.data, 3);
}

void test_stv_afterLastDelim_empty_delim(void) {
    strview sv = stv_literal("abc");
    TEST_ASSERT_TRUE(stv_equal(sv, stv_afterLastDelim(sv, stv_nullstv)));
}

void test_stv_afterLastDelim_empty_view(void) {
    TEST_ASSERT_TRUE(stv_empty(stv_afterLastDelim(stv_nullstv, stv_literal("/"))));
}

void test_stv_afterLastDelim_multiple(void) {
    strview after = stv_afterLastDelim(stv_literal("a/b/c"), stv_literal("/"));
    TEST_ASSERT_EQUAL_size_t(1, after.len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("c", after.data, 1);
}

void run_split_tests(void) {
    RUN_TEST(test_stv_split_normal);
    RUN_TEST(test_stv_split_nocase);
    RUN_TEST(test_stv_split_not_found);
    RUN_TEST(test_stv_split_empty_sep);
    RUN_TEST(test_stv_split_empty_sep_single_char);
    RUN_TEST(test_stv_split_null_remaining);
    RUN_TEST(test_stv_split_sep_at_end);
    RUN_TEST(test_stv_split_sep_at_start);
    RUN_TEST(test_stv_split_empty_view);
    RUN_TEST(test_stv_splitLines_lf);
    RUN_TEST(test_stv_splitLines_crlf);
    RUN_TEST(test_stv_splitLines_cr_only);
    RUN_TEST(test_stv_splitLines_no_break);
    RUN_TEST(test_stv_splitLines_trailing_newline);
    RUN_TEST(test_stv_splitLines_empty);
    RUN_TEST(test_stv_splitLines_null_remaining);
    RUN_TEST(test_stv_splitWords_normal);
    RUN_TEST(test_stv_splitWords_single_word);
    RUN_TEST(test_stv_splitWords_only_whitespace);
    RUN_TEST(test_stv_splitWords_empty);
    RUN_TEST(test_stv_splitWords_null_remaining);
    RUN_TEST(test_stv_before_delim_found);
    RUN_TEST(test_stv_before_delim_not_found);
    RUN_TEST(test_stv_before_delim_empty_view);
    RUN_TEST(test_stv_before_delim_empty_delim);
    RUN_TEST(test_stv_before_delim_delim_at_start);
    RUN_TEST(test_stv_after_delim_found);
    RUN_TEST(test_stv_after_delim_not_found);
    RUN_TEST(test_stv_after_delim_empty_delim);
    RUN_TEST(test_stv_after_delim_delim_at_end);
    RUN_TEST(test_stv_beforeLastDelim_found);
    RUN_TEST(test_stv_beforeLastDelim_not_found);
    RUN_TEST(test_stv_beforeLastDelim_at_start);
    RUN_TEST(test_stv_beforeLastDelim_at_end);
    RUN_TEST(test_stv_beforeLastDelim_empty_delim);
    RUN_TEST(test_stv_beforeLastDelim_empty_view);
    RUN_TEST(test_stv_beforeLastDelim_multiple);
    RUN_TEST(test_stv_afterLastDelim_found);
    RUN_TEST(test_stv_afterLastDelim_not_found);
    RUN_TEST(test_stv_afterLastDelim_at_end);
    RUN_TEST(test_stv_afterLastDelim_at_start);
    RUN_TEST(test_stv_afterLastDelim_empty_delim);
    RUN_TEST(test_stv_afterLastDelim_empty_view);
    RUN_TEST(test_stv_afterLastDelim_multiple);
}
