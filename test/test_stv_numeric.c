/**
 * @file test_stv_numeric.c
 * @brief test parse int num
 */

#include "stv.h"
#include "unity/unity.h"
#include <inttypes.h>

/* ========== stv_ch2digit ========== */
void test_ch2digit_digits(void) {
    TEST_ASSERT_EQUAL_INT(0, stv_ch2digit('0'));
    TEST_ASSERT_EQUAL_INT(9, stv_ch2digit('9'));
}

void test_ch2digit_uppercase(void) {
    TEST_ASSERT_EQUAL_INT(10, stv_ch2digit('A'));
    TEST_ASSERT_EQUAL_INT(35, stv_ch2digit('Z'));
}

void test_ch2digit_lowercase(void) {
    TEST_ASSERT_EQUAL_INT(10, stv_ch2digit('a'));
    TEST_ASSERT_EQUAL_INT(35, stv_ch2digit('z'));
}

void test_ch2digit_invalid(void) {
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('/'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit(':'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('@'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('['));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('`'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit('{'));
    TEST_ASSERT_EQUAL_INT(-1, stv_ch2digit(' '));
}

/* ========== stv_parseIntBase ========== */
void test_parseIntBase_empty(void) {
    strview rem;
    int     base = stv_parseIntBase(stv_nullstv, &rem);
    TEST_ASSERT_EQUAL_INT(0, base);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseIntBase_no_prefix(void) {
    strview sv = stv_literal("123");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
    TEST_ASSERT_EQUAL_STRING("123", rem.data);
    TEST_ASSERT_EQUAL_size_t(3, rem.len);
}

void test_parseIntBase_0b(void) {
    strview sv = stv_literal("0b101");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(2, base);
    TEST_ASSERT_EQUAL_STRING("101", rem.data);
}

void test_parseIntBase_0B(void) {
    strview sv = stv_literal("0B101");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(2, base);
}

void test_parseIntBase_0o(void) {
    strview sv = stv_literal("0o77");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(8, base);
}

void test_parseIntBase_0d(void) {
    strview sv = stv_literal("0d99");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
}

void test_parseIntBase_0x(void) {
    strview sv = stv_literal("0xFF");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(16, base);
}

void test_parseIntBase_leading_zero_no_prefix(void) {
    strview sv = stv_literal("077");
    strview rem;
    int     base = stv_parseIntBase(sv, &rem);
    TEST_ASSERT_EQUAL_INT(10, base);
    TEST_ASSERT_EQUAL_STRING("077", rem.data);
}

/* ========== stv_parseInum ========== */
void test_parseInum_decimal_positive(void) {
    strview  sv = stv_literal("42");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(42, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseInum_decimal_negative(void) {
    strview  sv = stv_literal("-42");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(-42, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseInum_with_whitespace(void) {
    strview  sv = stv_literal("  -123");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(-123, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseInum_auto_base_hex(void) {
    strview  sv = stv_literal("0xFF");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_INT64(255, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseInum_auto_base_binary(void) {
    strview  sv = stv_literal("0b101");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_INT64(5, val);
}

void test_parseInum_auto_base_octal(void) {
    strview  sv = stv_literal("0o77");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_INT64(63, val);
}

void test_parseInum_auto_base_decimal(void) {
    strview  sv = stv_literal("0d99");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_INT64(99, val);
}

void test_parseInum_explicit_base_hex(void) {
    strview  sv = stv_literal("0x1a");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 16, &rem);
    TEST_ASSERT_EQUAL_INT64(26, val);
}

void test_parseInum_explicit_base_hex_negative(void) {
    strview  sv = stv_literal("-0xFF");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 16, &rem);
    TEST_ASSERT_EQUAL_INT64(-255, val);
}

void test_parseInum_overflow_positive(void) {
    strview  sv = stv_literal("9223372036854775808");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(INTMAX_MAX, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseInum_overflow_negative(void) {
    strview  sv = stv_literal("-9223372036854775809");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(INTMAX_MIN, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseInum_invalid_base(void) {
    strview  sv = stv_literal("123");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 1, &rem);
    TEST_ASSERT_EQUAL_INT64(0, val);
    TEST_ASSERT_EQUAL_STRING("123", rem.data);
}

void test_parseInum_trailing_chars(void) {
    strview  sv = stv_literal("42abc");
    strview  rem;
    intmax_t val = stv_parseInum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_INT64(42, val);
    TEST_ASSERT_EQUAL_STRING("abc", rem.data);
}

/* ========== stv_parseUnum ========== */
void test_parseUnum_normal(void) {
    strview   sv = stv_literal("12345");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_UINT64(12345, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseUnum_negative_wraps(void) {
    strview   sv = stv_literal("-40");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_UINT64(UINTMAX_MAX - 39, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseUnum_overflow(void) {
    strview   sv = stv_literal("18446744073709551616"); /* > UINTMAX_MAX */
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_UINT64(UINTMAX_MAX, val);
    TEST_ASSERT_TRUE(stv_empty(rem));
}

void test_parseUnum_auto_base_hex(void) {
    strview   sv = stv_literal("0xFF");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 0, &rem);
    TEST_ASSERT_EQUAL_UINT64(255, val);
}

void test_parseUnum_trailing_chars(void) {
    strview   sv = stv_literal("42abc");
    strview   rem;
    uintmax_t val = stv_parseUnum(sv, 10, &rem);
    TEST_ASSERT_EQUAL_UINT64(42, val);
    TEST_ASSERT_EQUAL_STRING("abc", rem.data);
}

void run_numeric_tests(void) {
    RUN_TEST(test_ch2digit_digits);
    RUN_TEST(test_ch2digit_uppercase);
    RUN_TEST(test_ch2digit_lowercase);
    RUN_TEST(test_ch2digit_invalid);
    RUN_TEST(test_parseIntBase_empty);
    RUN_TEST(test_parseIntBase_no_prefix);
    RUN_TEST(test_parseIntBase_0b);
    RUN_TEST(test_parseIntBase_0B);
    RUN_TEST(test_parseIntBase_0o);
    RUN_TEST(test_parseIntBase_0d);
    RUN_TEST(test_parseIntBase_0x);
    RUN_TEST(test_parseIntBase_leading_zero_no_prefix);
    RUN_TEST(test_parseInum_decimal_positive);
    RUN_TEST(test_parseInum_decimal_negative);
    RUN_TEST(test_parseInum_with_whitespace);
    RUN_TEST(test_parseInum_auto_base_hex);
    RUN_TEST(test_parseInum_auto_base_binary);
    RUN_TEST(test_parseInum_auto_base_octal);
    RUN_TEST(test_parseInum_auto_base_decimal);
    RUN_TEST(test_parseInum_explicit_base_hex);
    RUN_TEST(test_parseInum_explicit_base_hex_negative);
    RUN_TEST(test_parseInum_overflow_positive);
    RUN_TEST(test_parseInum_overflow_negative);
    RUN_TEST(test_parseInum_invalid_base);
    RUN_TEST(test_parseInum_trailing_chars);
    RUN_TEST(test_parseUnum_normal);
    RUN_TEST(test_parseUnum_negative_wraps);
    RUN_TEST(test_parseUnum_overflow);
    RUN_TEST(test_parseUnum_auto_base_hex);
    RUN_TEST(test_parseUnum_trailing_chars);
}
