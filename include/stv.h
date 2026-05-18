/*
 * MIT License
 * Copyright (c) 2026 Akarin <akarin@icatcp.work>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file stv.h
 * @brief Lightweight string view library
 *
 * Provides operations on read-only string views (strview), supporting C99 and C++11 or later compilers.
 * All functions allocate no memory and do not modify the original string.
 * By default, only declarations are included; define the LIB_STV_IMPL macro before one inclusion to compile the
 * implementation. Alternatively, define LIB_STV_STATIC_INLINE_IMPL to compile the implementation as static inline.
 */

#ifndef LIB_STV_H
#define LIB_STV_H

#ifdef LIB_STV_STATIC_INLINE_IMPL
    #define LIB_STV_FN static inline
    #ifndef LIB_STV_IMPL
        #define LIB_STV_IMPL
    #endif
#else
    #define LIB_STV_FN
#endif // !LIB_STV_STATIC_INLINE_IMPL

#ifdef __cplusplus
extern "C" {
#endif // !__cplusplus

#if defined(__cplusplus) && __cplusplus >= 201103L
    #include <limits.h>
    #include <stddef.h>
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #include <limits.h>
    #include <stddef.h>
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <limits.h>
    #include <stdbool.h>
    #include <stddef.h>
    #ifndef nullptr
        #define nullptr NULL
    #endif
#else
    #error "stv.h requires a C99 (or C++11) compiler!"
#endif

/**
 * @brief String view structure
 *
 * Represents a read-only string fragment, consisting of a pointer to the first character and a length.
 * Not null-terminated; does not own the data.
 */
typedef struct stv_strview_t strview;
struct stv_strview_t {
    const char* data; ///< Pointer to the string data (may be NULL)
    size_t      len;  ///< Length of the string in bytes
};

/**
 * @brief Create a string view from a C-style string (up to the null terminator)
 *
 * @param c_str Pointer to a null-terminated C string, may be NULL
 * @return A new strview; returns an empty view if c_str is NULL
 */
LIB_STV_FN strview stv_new(const char* c_str);

/**
 * @brief Create a string view from a character sequence, stopping at a specified terminator or maximum length
 *
 * @param c_str Source string pointer, may be NULL
 * @param endchar Terminator character (stop before this character; the character itself is not included)
 * @param maxlen Maximum number of bytes to scan
 * @return A new strview; returns an empty view if c_str is NULL
 */
LIB_STV_FN strview stv_create(const char* str, unsigned char endchar, size_t maxlen);

/**
 * @brief Extract a substring view
 *
 * @param stv Source string view
 * @param begin_pos Start position (inclusive)
 * @param end_pos End position (exclusive); use stv_end to indicate up to the end
 * @return The substring view [begin_pos, end_pos) if the range is valid, otherwise an empty view
 */
LIB_STV_FN strview stv_slice(strview stv, size_t begin_pos, size_t end_pos);

/**
 * @brief Remove any characters belonging to a given charset from the beginning and end of the view
 *
 * @param stv Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with leading and trailing characters removed; if charset is empty, the original view is returned
 * unchanged
 */
LIB_STV_FN strview stv_trim(strview stv, strview charset);

/**
 * @brief Remove any characters belonging to a given charset from the beginning of the view
 *
 * @param stv Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with leading characters removed; if charset is empty, the original view is returned unchanged
 */
LIB_STV_FN strview stv_trimStart(strview stv, strview charset);

/**
 * @brief Remove any characters belonging to a given charset from the end of the view
 *
 * @param stv Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with trailing characters removed; if charset is empty, the original view is returned unchanged
 */
LIB_STV_FN strview stv_trimEnd(strview stv, strview charset);

/**
 * @brief Search for a substring within a text view (automatic algorithm selection)
 *
 * Uses naive search when the pattern length is <= 4, otherwise uses Sunday search.
 *
 * @param stv_text The text view to search in
 * @param stv_pat The pattern view to search for
 * @return Start position of the first match; returns 0 if stv_pat is empty, or stv_npos if not found
 */
LIB_STV_FN size_t stv_search(strview stv_text, strview stv_pat);

/**
 * @brief Naive string search (suitable for short patterns)
 *
 * @param stv_text The text view to search in
 * @param stv_pat The pattern view to search for
 * @return Start position of the first match; returns 0 if stv_pat is empty, or stv_npos if not found
 */
LIB_STV_FN size_t stv_naiveSearch(strview stv_text, strview stv_pat);

/**
 * @brief Sunday string search (suitable for longer patterns)
 *
 * @param stv_text The text view to search in
 * @param stv_pat The pattern view to search for
 * @return Start position of the first match; returns 0 if stv_pat is empty, or stv_npos if not found
 */
LIB_STV_FN size_t stv_sundaySearch(strview stv_text, strview stv_pat);

/**
 * @brief Find the first occurrence of a character in the view
 *
 * @param stv String view
 * @param ch Character to locate
 * @return Index of the first occurrence, or stv_npos if not found
 */
LIB_STV_FN size_t stv_firstChar(strview stv, const char ch);

/**
 * @brief Find the first position where the character differs from ch
 *
 * @param stv String view
 * @param ch Character to skip
 * @return Index of the first character not equal to ch, or stv_npos if all characters match or the view is empty
 */
LIB_STV_FN size_t stv_firstNotChar(strview stv, const char ch);

/**
 * @brief Find the last occurrence of a character in the view
 *
 * @param stv String view
 * @param ch Character to locate
 * @return Index of the last occurrence, or stv_npos if not found
 */
LIB_STV_FN size_t stv_lastChar(strview stv, const char ch);

/**
 * @brief Find the last position where the character differs from ch
 *
 * @param stv String view
 * @param ch Character to skip
 * @return Index of the last character not equal to ch, or stv_npos if all characters match or the view is empty
 */
LIB_STV_FN size_t stv_lastNotChar(strview stv, const char ch);

/**
 * @brief Find the first position where two views differ (from left to right)
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return Index of the first differing character, or stv_npos if the views are identical
 */
LIB_STV_FN size_t stv_firstDiff(strview stv_left, strview stv_right);

/**
 * @brief Find the last position where two views differ (from right to left)
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return Index of the last differing character, or stv_npos if the views are identical
 */
LIB_STV_FN size_t stv_lastDiff(strview stv_left, strview stv_right);

/**
 * @brief Compare two string views lexicographically
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return Negative if left < right, 0 if equal, positive if left > right
 */
LIB_STV_FN int stv_compare(strview stv_left, strview stv_right);

/**
 * @brief Check if a view starts with a given prefix
 *
 * @param stv_text The text view to examine
 * @param stv_pat The prefix pattern (empty pattern always returns true)
 * @return true if stv_text starts with stv_pat, false otherwise
 */
LIB_STV_FN bool stv_startsWith(strview stv_text, strview stv_pat);

/**
 * @brief Check if a view ends with a given suffix
 *
 * @param stv_text The text view to examine
 * @param stv_pat The suffix pattern (empty pattern always returns true)
 * @return true if stv_text ends with stv_pat, false otherwise
 */
LIB_STV_FN bool stv_endsWith(strview stv_text, strview stv_pat);

/**
 * @brief Check if a view contains a given substring
 *
 * @param stv_text The text view
 * @param stv_pat The pattern to search for (empty pattern is considered as contained)
 * @return true if stv_pat appears in stv_text, false otherwise
 */
LIB_STV_FN bool stv_contains(strview stv_text, strview stv_pat);

/**
 * @brief Check if two views have identical content (byte-by-byte comparison)
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return true if both length and every character match, false otherwise
 */
LIB_STV_FN bool stv_equal(strview stv_left, strview stv_right);

/**
 * @brief Check if two views reference exactly the same underlying data (same pointer and length)
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return true if both the data pointer and the length are equal, false otherwise
 */
LIB_STV_FN bool stv_same(strview stv_left, strview stv_right);

/**
 * @brief Check if a view is empty
 *
 * @param stv String view
 * @return true if data is NULL or length is 0, false otherwise
 */
LIB_STV_FN bool stv_empty(strview stv);

/**
 * @brief Get the first character of the view
 *
 * @param stv String view
 * @return The first character if the view is non-empty, otherwise '\0'
 */
LIB_STV_FN char stv_front(strview stv);

/**
 * @brief Get the last character of the view
 *
 * @param stv String view
 * @return The last character if the view is non-empty, otherwise '\0'
 */
LIB_STV_FN char stv_back(strview stv);

/**
 * @brief Swap the contents of two string views
 *
 * If either pointer is NULL, the function does nothing.
 *
 * @param stv_left Pointer to the first view
 * @param stv_right Pointer to the second view
 */
LIB_STV_FN void stv_swap(strview* stv_left, strview* stv_right);

/**
 * @brief Copy the string view into a C-style string buffer
 *
 * @param stv Source string view
 * @param mem Destination buffer
 * @param size Buffer size in bytes
 * @return mem if the buffer is large enough to hold the view content and the null terminator, otherwise NULL
 */
LIB_STV_FN char* stv_cstr(strview stv, char* mem, size_t size);

/**
 * @brief Copy the reversed content of the string view into a C-style string buffer
 *
 * @param stv Source string view
 * @param mem Destination buffer
 * @param size Buffer size in bytes
 * @return mem if the buffer is large enough to hold the reversed content and the null terminator, otherwise NULL
 */
LIB_STV_FN char* stv_rev_cstr(strview stv, char* mem, size_t size);

/**
 * @brief Convenience macro: construct a strview from a given data pointer and length
 *
 * @param data_v Pointer to the data
 * @param len_v Length
 */
#ifdef __cplusplus
    #define stv_makestv(data_v, len_v) (strview{/* .data= */ data_v, /* .len= */ len_v})
#else
    #define stv_makestv(data_v, len_v) ((strview){.data = data_v, .len = len_v})
#endif

/** @brief Predefined empty string view (data = nullptr, len = 0) */
#define stv_nullstv stv_makestv(nullptr, 0)

/** @brief Predefined whitespace character set view (space, carriage return, newline, tab, vertical tab, form feed) */
#define stv_whitespace stv_makestv(" \r\n\t\v\f", 6)

/** @brief Sentinel value indicating "not found" */
#define stv_npos ((size_t)-1)

/** @brief Represents the start position (index 0) */
#define stv_begin (0)

/** @brief Represents the end position (equivalent to stv_npos) */
#define stv_end (stv_npos)

/**
 * @brief Helper macro for printf-style formatting of a string view
 *
 * If the view length exceeds INT_MAX, the length parameter is capped at INT_MAX.
 * Example usage: printf("data: " stv_PFFMT "\n", stv_PFARG(myview));
 *
 * @param stv The string view to output
 */
#define stv_PFARG(stv)                                                                                                 \
    (int)(stv_empty(stv) ? 0 : (stv).len > INT_MAX ? INT_MAX : (stv).len), (stv_empty(stv) ? "" : (stv).data)

/**
 * @brief printf format string helper macro, used together with stv_PFARG
 *
 * @see stv_PFARG
 */
#define stv_PFFMT "%.*s"

/******************************* IMPLEMENTATION *******************************/

#ifdef LIB_STV_IMPL

LIB_STV_FN strview stv_new(const char* c_str) {
    return stv_create(c_str, '\0', stv_npos);
}

LIB_STV_FN strview stv_create(const char* str, unsigned char endchar, size_t maxlen) {
    size_t len = 0;
    if (str == nullptr) {
        return stv_nullstv;
    }
    while (len < maxlen && (unsigned char)str[len] != endchar) {
        len++;
    }
    return stv_makestv(str, len);
}

LIB_STV_FN strview stv_slice(strview stv, size_t begin_pos, size_t end_pos) {
    if (end_pos == stv_npos) {
        end_pos = stv.len;
    }
    if (stv.data && stv.len >= end_pos && end_pos >= begin_pos) {
        return stv_makestv(stv.data + begin_pos, end_pos - begin_pos);
    }
    return stv_nullstv;
}

LIB_STV_FN strview stv_trim(strview stv, strview charset) {
    return stv_trimStart(stv_trimEnd(stv, charset), charset);
}

LIB_STV_FN strview stv_trimStart(strview stv, strview charset) {
    if (stv_empty(stv) || stv_empty(charset)) {
        return stv;
    }
    bool chs[UCHAR_MAX + 1] = {0};
    for (size_t idx = 0; idx < charset.len; idx++) {
        chs[(unsigned char)charset.data[idx]] = true;
    }
    const char* end_pos = stv.data + stv.len;
    const char* ch      = stv.data;
    size_t      tc      = 0;
    while (ch < end_pos) {
        if (chs[(unsigned char)*ch]) {
            ch++, tc++;
        } else {
            break;
        }
    }
    return stv_makestv(stv.data + tc, stv.len - tc);
}

LIB_STV_FN strview stv_trimEnd(strview stv, strview charset) {
    if (stv_empty(stv) || stv_empty(charset)) {
        return stv;
    }
    bool chs[UCHAR_MAX + 1] = {0};
    for (size_t idx = 0; idx < charset.len; idx++) {
        chs[(unsigned char)charset.data[idx]] = true;
    }
    const char* start_pos = stv.data;
    const char* ch        = stv.data + stv.len;
    size_t      tc        = 0;
    while (ch > start_pos) {
        ch--;
        if (chs[(unsigned char)*ch]) {
            tc++;
        } else {
            break;
        }
    }
    return stv_makestv(stv.data, stv.len - tc);
}

LIB_STV_FN size_t stv_search(strview stv_text, strview stv_pat) {
    if (stv_pat.len > 4) {
        return stv_sundaySearch(stv_text, stv_pat);
    } else {
        return stv_naiveSearch(stv_text, stv_pat);
    }
}

LIB_STV_FN size_t stv_naiveSearch(strview stv_text, strview stv_pat) {
    if (stv_empty(stv_pat)) {
        return 0;
    }
    if (stv_empty(stv_text) || stv_text.len < stv_pat.len) {
        return stv_npos;
    }

    for (size_t pos = 0; pos <= (stv_text.len - stv_pat.len); pos++) {
        size_t idx;
        for (idx = 0; idx < stv_pat.len; idx++) {
            if (stv_text.data[pos + idx] != stv_pat.data[idx]) {
                break;
            }
        }
        if (idx == stv_pat.len) {
            return pos;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_sundaySearch(strview stv_text, strview stv_pat) {
    if (stv_empty(stv_pat)) {
        return 0;
    }
    if (stv_empty(stv_text) || stv_text.len < stv_pat.len) {
        return stv_npos;
    }

    size_t shift[UCHAR_MAX + 1];
    for (size_t i = 0; i < (UCHAR_MAX + 1); i++) {
        shift[i] = stv_pat.len + 1;
    }
    for (size_t i = 0; i < stv_pat.len; i++) {
        shift[(unsigned char)stv_pat.data[i]] = stv_pat.len - i;
    }
    size_t pos = 0;
    while (pos <= (stv_text.len - stv_pat.len)) {
        size_t idx;
        for (idx = 0; idx < stv_pat.len; idx++) {
            if (stv_text.data[pos + idx] != stv_pat.data[idx]) {
                if (pos + stv_pat.len >= stv_text.len) {
                    return stv_npos;
                }
                const unsigned char next_char = (unsigned char)stv_text.data[pos + stv_pat.len];
                pos += shift[next_char];
                break;
            }
        }
        if (idx == stv_pat.len) {
            return pos;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_firstChar(strview stv, const char ch) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    const char*  curr = stv.data;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        if (*curr == ch) {
            return idx;
        }
        curr++;
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_firstNotChar(strview stv, const char ch) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    const char*  curr = stv.data;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        if (*curr != ch) {
            return idx;
        }
        curr++;
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_lastChar(strview stv, const char ch) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    const char*  curr = stv.data + stv.len;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        curr--;
        if (*curr == ch) {
            return len - idx - 1;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_lastNotChar(strview stv, const char ch) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    const char*  curr = stv.data + stv.len;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        curr--;
        if (*curr != ch) {
            return len - idx - 1;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_firstDiff(strview stv_left, strview stv_right) {
    if (stv_same(stv_left, stv_right)) {
        return stv_npos;
    }
    if (stv_empty(stv_left) && stv_empty(stv_right)) {
        return stv_npos;
    }
    if (stv_empty(stv_left) || stv_empty(stv_right)) {
        return 0;
    }
    const size_t min_len   = stv_left.len > stv_right.len ? stv_right.len : stv_left.len;
    const char*  left_ptr  = stv_left.data;
    const char*  right_ptr = stv_right.data;
    for (size_t idx = 0; idx < min_len; idx++) {
        if (*left_ptr != *right_ptr) {
            return idx;
        }
        left_ptr++, right_ptr++;
    }
    return (stv_left.len == stv_right.len) ? stv_npos : min_len;
}

LIB_STV_FN size_t stv_lastDiff(strview stv_left, strview stv_right) {
    if (stv_same(stv_left, stv_right)) {
        return stv_npos;
    }
    if (stv_empty(stv_left) && stv_empty(stv_right)) {
        return stv_npos;
    }
    if (stv_empty(stv_left) || stv_empty(stv_right)) {
        return (stv_empty(stv_left) ? stv_right.len : stv_left.len) - 1;
    }
    const size_t min_len   = stv_left.len < stv_right.len ? stv_left.len : stv_right.len;
    const size_t max_len   = stv_left.len > stv_right.len ? stv_left.len : stv_right.len;
    const char*  left_ptr  = stv_left.data + stv_left.len;
    const char*  right_ptr = stv_right.data + stv_right.len;
    for (size_t idx = 0; idx < min_len; idx++) {
        left_ptr--, right_ptr--;
        if (*left_ptr != *right_ptr) {
            return max_len - idx - 1;
        }
    }
    return (stv_left.len == stv_right.len) ? stv_npos : max_len - 1;
}

LIB_STV_FN bool stv_startsWith(strview stv_text, strview stv_pat) {
    if (stv_empty(stv_pat)) {
        return true;
    }
    if (stv_empty(stv_text) || stv_text.len < stv_pat.len) {
        return false;
    }
    const char* text_ptr = stv_text.data;
    const char* pat_ptr  = stv_pat.data;
    const char* end_ptr  = pat_ptr + stv_pat.len;
    while (pat_ptr < end_ptr) {
        if (*text_ptr != *pat_ptr) {
            return false;
        }
        text_ptr++, pat_ptr++;
    }
    return true;
}

LIB_STV_FN bool stv_endsWith(strview stv_text, strview stv_pat) {
    if (stv_empty(stv_pat)) {
        return true;
    }
    if (stv_empty(stv_text) || stv_text.len < stv_pat.len) {
        return false;
    }
    const char* text_ptr  = stv_text.data + stv_text.len;
    const char* pat_ptr   = stv_pat.data + stv_pat.len;
    const char* begin_ptr = stv_pat.data;
    while (pat_ptr > begin_ptr) {
        text_ptr--, pat_ptr--;
        if (*text_ptr != *pat_ptr) {
            return false;
        }
    }
    return true;
}

LIB_STV_FN int stv_compare(strview stv_left, strview stv_right) {
    const size_t        pos = stv_firstDiff(stv_left, stv_right);
    const unsigned char c1  = (pos < stv_left.len) ? stv_left.data[pos] : '\0';
    const unsigned char c2  = (pos < stv_right.len) ? stv_right.data[pos] : '\0';
    return c1 - c2;
}

LIB_STV_FN bool stv_contains(strview stv_text, strview stv_pat) {
    return stv_empty(stv_pat) || stv_search(stv_text, stv_pat) != stv_npos;
}

LIB_STV_FN bool stv_equal(strview stv_left, strview stv_right) {
    return stv_firstDiff(stv_left, stv_right) == stv_npos;
}

LIB_STV_FN bool stv_same(strview stv_left, strview stv_right) {
    return (stv_left.data == stv_right.data) && (stv_left.len == stv_right.len);
}

LIB_STV_FN bool stv_empty(strview stv) {
    return stv.data == nullptr || stv.len == 0;
}

LIB_STV_FN char stv_front(strview stv) {
    return (stv_empty(stv) ? '\0' : stv.data[0]);
}

LIB_STV_FN char stv_back(strview stv) {
    return (stv_empty(stv) ? '\0' : stv.data[stv.len - 1]);
}

LIB_STV_FN void stv_swap(strview* stv_left, strview* stv_right) {
    if (stv_left && stv_right) {
        const strview tmp = *stv_left;
        *stv_left         = *stv_right;
        *stv_right        = tmp;
    }
}

LIB_STV_FN char* stv_cstr(strview stv, char* mem, size_t size) {
    if (mem == nullptr || size <= stv.len) {
        return nullptr;
    }
    const size_t endidx = stv_empty(stv) ? 0 : stv.len;
    if (endidx > 0) {
        for (size_t idx = 0; idx < endidx; idx++) {
            mem[idx] = stv.data[idx];
        }
    }
    mem[endidx] = '\0';
    return mem;
}

LIB_STV_FN char* stv_rev_cstr(strview stv, char* mem, size_t size) {
    if (mem == nullptr || size <= stv.len) {
        return nullptr;
    }
    const size_t endidx = stv_empty(stv) ? 0 : stv.len;
    if (endidx > 0) {
        for (size_t idx = 0, ridx = endidx - 1; idx < endidx; idx++, ridx--) {
            mem[idx] = stv.data[ridx];
        }
    }
    mem[endidx] = '\0';
    return mem;
}

#endif // !LIB_STV_IMPL

#ifdef __cplusplus
}
#endif // !__cplusplus

#endif // !LIB_STV_H
