/**
 * @file test_stv_cstr.c
 * @brief test to C string and join
 */

#include "stv.h"
#include "unity/unity.h"

/* ========== stv_cstr ========== */
void test_cstr_success(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("Hello", buf);
}

void test_cstr_too_small(void) {
    char    buf[3];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), stv_Default);
    TEST_ASSERT_NULL(ret);
}

void test_cstr_empty_view(void) {
    char    buf[2] = {0};
    strview sv     = stv_nullstv;
    char*   ret    = stv_cstr(sv, buf, sizeof(buf), stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_cstr_null_mem(void) {
    char* ret = stv_cstr(stv_literal("abc"), NULL, 10, stv_Default);
    TEST_ASSERT_NULL(ret);
}

void test_cstr_zero_size(void) {
    char    buf[1];
    strview sv  = stv_literal("abc");
    char*   ret = stv_cstr(sv, buf, 0, stv_Default);
    TEST_ASSERT_NULL(ret);
}

/* ========== stv_cstr 选项 ========== */
void test_cstr_reverse(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), stv_Reverse);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("olleH", buf);
}

void test_cstr_upper(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), stv_ToUpper);
    TEST_ASSERT_EQUAL_STRING("HELLO", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_lower(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), stv_ToLower);
    TEST_ASSERT_EQUAL_STRING("hello", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_reverse_upper(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), (stv_cstrOptions)(stv_Reverse | stv_ToUpper));
    TEST_ASSERT_EQUAL_STRING("OLLEH", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_swapCase(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), (stv_cstrOptions)(stv_ToUpper | stv_ToLower));
    TEST_ASSERT_EQUAL_STRING("hELLO", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_truncate(void) {
    char    buf[4];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), stv_Truncate);
    TEST_ASSERT_EQUAL_STRING("Hel", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_truncate_exact(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), stv_Truncate);
    TEST_ASSERT_EQUAL_STRING("Hello", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_truncate_zero_len(void) {
    char    buf[1] = {0};
    strview sv     = stv_literal("abc");
    char*   ret    = stv_cstr(sv, buf, sizeof(buf), stv_Truncate);
    TEST_ASSERT_EQUAL_STRING("", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_view_reverse(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), stv_ViewReverse);
    TEST_ASSERT_EQUAL_STRING("olleH", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_view_reverse_truncate(void) {
    char    buf[4];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), (stv_cstrOptions)(stv_ViewReverse | stv_Truncate));
    /* ViewReverse 选择最后的 size-1 个源字节：从 'l','l','o' 倒序读取 -> 'o','l','l'，然后截断为 3 字节 */
    TEST_ASSERT_EQUAL_STRING("oll", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_cstr_reverse_and_view_reverse(void) {
    char    buf[6];
    strview sv  = stv_literal("Hello");
    char*   ret = stv_cstr(sv, buf, sizeof(buf), (stv_cstrOptions)(stv_Reverse | stv_ViewReverse));
    /* 两者相互抵消，结果与原始相同 */
    TEST_ASSERT_EQUAL_STRING("Hello", buf);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
}

/* ========== stv_join ========== */
void test_join_LIST_macro(void) {
    strview sv1 = stv_literal("a"), sv2 = stv_literal("b"), sv3 = stv_literal("c");
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_join(stv_LIST(sv1, sv2, sv3), buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("a,b,c", buf);
}

void test_join_empty_array(void) {
    char    buf[4] = {0};
    strview sep    = stv_literal(",");
    char*   ret    = stv_join(NULL, 0, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_join_null_array_with_zero_len(void) {
    char  buf[1] = {0};
    char* ret    = stv_join(NULL, 0, buf, sizeof(buf), stv_nullstv, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_join_null_array_with_nonzero_len(void) {
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_join(NULL, 1, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_NULL(ret);
}

void test_join_single_element(void) {
    strview arr[] = {stv_literal("Hello")};
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_join(arr, 1, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_PTR(buf, ret);
    TEST_ASSERT_EQUAL_STRING("Hello", buf);
}

void test_join_multiple_elements(void) {
    strview arr[] = {stv_literal("a"), stv_literal("b"), stv_literal("c")};
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_join(arr, 3, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_STRING("a,b,c", buf);
}

void test_join_empty_separator(void) {
    strview arr[] = {stv_literal("1"), stv_literal("2"), stv_literal("3")};
    char    buf[10];
    char*   ret = stv_join(arr, 3, buf, sizeof(buf), stv_nullstv, stv_Default);
    TEST_ASSERT_EQUAL_STRING("123", buf);
}

void test_join_with_empty_elements(void) {
    strview arr[] = {stv_literal("a"), stv_nullstv, stv_literal("c")};
    char    buf[10];
    strview sep = stv_literal(",");
    char*   ret = stv_join(arr, 3, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_STRING("a,,c", buf);
}

void test_join_with_options(void) {
    strview arr[] = {stv_literal("Hello"), stv_literal("World")};
    char    buf[20];
    strview sep = stv_literal("-");
    char*   ret = stv_join(arr, 2, buf, sizeof(buf), sep, stv_ToUpper);
    TEST_ASSERT_EQUAL_STRING("HELLO-WORLD", buf);
}

void test_join_reverse(void) {
    strview arr[] = {stv_literal("abc"), stv_literal("123")};
    char    buf[10];
    strview sep = stv_literal(",");
    /* stv_Reverse 反转整个连接结果 */
    char* ret = stv_join(arr, 2, buf, sizeof(buf), sep, stv_Reverse);
    TEST_ASSERT_EQUAL_STRING("321,cba", buf);
}

void test_join_view_reverse(void) {
    strview arr[] = {stv_literal("abc"), stv_literal("123")};
    char    buf[10];
    strview sep = stv_literal(",");
    /* stv_ViewReverse 反转每个元素 */
    char* ret = stv_join(arr, 2, buf, sizeof(buf), sep, stv_ViewReverse);
    TEST_ASSERT_EQUAL_STRING("cba,321", buf);
}

void test_join_reverse_and_view_reverse(void) {
    strview arr[] = {stv_literal("abc"), stv_literal("123")};
    char    buf[10];
    strview sep = stv_literal(",");
    /* 先每个元素反转，再整体反转 */
    char* ret = stv_join(arr, 2, buf, sizeof(buf), sep, (stv_cstrOptions)(stv_Reverse | stv_ViewReverse));
    TEST_ASSERT_EQUAL_STRING("123,abc", buf);
}

void test_join_join_reverse(void) {
    strview arr[] = {stv_literal("a"), stv_literal("b"), stv_literal("c")};
    char    buf[10];
    strview sep = stv_literal(",");
    /* stv_JoinReverse 反转元素顺序 */
    char* ret = stv_join(arr, 3, buf, sizeof(buf), sep, stv_JoinReverse);
    TEST_ASSERT_EQUAL_STRING("c,b,a", buf);
}

void test_join_buffer_too_small(void) {
    strview arr[] = {stv_literal("long"), stv_literal("string")};
    char    buf[5];
    strview sep = stv_literal(",");
    char*   ret = stv_join(arr, 2, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_NULL(ret);
}

void test_join_buffer_exact_size(void) {
    strview arr[] = {stv_literal("ab"), stv_literal("cd")};
    char    buf[6];
    strview sep = stv_literal(",");
    char*   ret = stv_join(arr, 2, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_STRING("ab,cd", buf);
}

void test_join_null_mem(void) {
    strview arr[] = {stv_literal("x")};
    strview sep   = stv_literal(",");
    char*   ret   = stv_join(arr, 1, NULL, 10, sep, stv_Default);
    TEST_ASSERT_NULL(ret);
}

void test_join_separator_with_spaces(void) {
    strview arr[] = {stv_literal("hello"), stv_literal("world")};
    char    buf[20];
    strview sep = stv_literal(" - ");
    char*   ret = stv_join(arr, 2, buf, sizeof(buf), sep, stv_Default);
    TEST_ASSERT_EQUAL_STRING("hello - world", buf);
}

void test_join_truncate(void) {
    strview arr[] = {stv_literal("abc"), stv_literal("defgh")};
    char    buf[6];
    strview sep = stv_literal(",");
    char*   ret = stv_join(arr, 2, buf, sizeof(buf), sep, stv_Truncate);
    TEST_ASSERT_EQUAL_STRING("abc,d", buf);
}

void test_join_zero_size(void) {
    strview arr[] = {stv_literal("x")};
    char    buf[1];
    strview sep = stv_literal(",");
    char*   ret = stv_join(arr, 1, buf, 0, sep, stv_Default);
    TEST_ASSERT_NULL(ret);
}

void run_cstr_tests(void) {
    RUN_TEST(test_cstr_success);
    RUN_TEST(test_cstr_too_small);
    RUN_TEST(test_cstr_empty_view);
    RUN_TEST(test_cstr_null_mem);
    RUN_TEST(test_cstr_zero_size);
    RUN_TEST(test_cstr_reverse);
    RUN_TEST(test_cstr_upper);
    RUN_TEST(test_cstr_lower);
    RUN_TEST(test_cstr_reverse_upper);
    RUN_TEST(test_cstr_swapCase);
    RUN_TEST(test_cstr_truncate);
    RUN_TEST(test_cstr_truncate_exact);
    RUN_TEST(test_cstr_truncate_zero_len);
    RUN_TEST(test_cstr_view_reverse);
    RUN_TEST(test_cstr_view_reverse_truncate);
    RUN_TEST(test_cstr_reverse_and_view_reverse);
    RUN_TEST(test_join_LIST_macro);
    RUN_TEST(test_join_empty_array);
    RUN_TEST(test_join_null_array_with_zero_len);
    RUN_TEST(test_join_null_array_with_nonzero_len);
    RUN_TEST(test_join_single_element);
    RUN_TEST(test_join_multiple_elements);
    RUN_TEST(test_join_empty_separator);
    RUN_TEST(test_join_with_empty_elements);
    RUN_TEST(test_join_with_options);
    RUN_TEST(test_join_reverse);
    RUN_TEST(test_join_view_reverse);
    RUN_TEST(test_join_reverse_and_view_reverse);
    RUN_TEST(test_join_join_reverse);
    RUN_TEST(test_join_buffer_too_small);
    RUN_TEST(test_join_buffer_exact_size);
    RUN_TEST(test_join_null_mem);
    RUN_TEST(test_join_separator_with_spaces);
    RUN_TEST(test_join_truncate);
    RUN_TEST(test_join_zero_size);
}
