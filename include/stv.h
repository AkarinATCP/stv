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
    #include <stdint.h>
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #include <limits.h>
    #include <stddef.h>
    #include <stdint.h>
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <limits.h>
    #include <stdbool.h>
    #include <stddef.h>
    #include <stdint.h>
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
 * @brief Character classification function pointer type
 *
 * Accepts a character (as int) and returns non-zero if the character belongs to the class.
 * Typical examples include isalnum(), islower(), etc.
 */
typedef int (*stv_charClassFn)(int);

/**
 * @brief Output options for stv_opt_cstr (bitwise combinable)
 *
 * Each option can be combined using bitwise OR. They control character case
 * and reversal during string copy.
 *
 * - stv_Default (0x00): No transformation (plain copy).
 * - stv_ToUpper (0x01): Convert characters to uppercase (using ASCII mask).
 * - stv_ToLower (0x02): Convert characters to lowercase (using ASCII mask).
 * - stv_Reverse (0x04): Reverse the order of characters in the output.
 *
 * @note When both stv_ToUpper and stv_ToLower are set, the result is swapCase (Abc -> aBC).
 */
typedef enum {
    stv_Default = 0, // 0b00000000
    stv_ToUpper = 1, // 0b00000001
    stv_ToLower = 2, // 0b00000010
    stv_Reverse = 4, // 0b00000100
} stv_cstrOptions;

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
 * @param str Source string pointer, may be NULL
 * @param endchar Terminator character (stop before this character; the character itself is not included)
 * @param maxlen Maximum number of bytes to scan
 * @return A new strview; returns an empty view if str is NULL
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
 * @brief Remove a given number of bytes from the beginning of the view
 *
 * Returns a new view that starts at @p len bytes from the beginning of @p stv.
 * If @p len is greater than or equal to the view length, an empty view is returned.
 *
 * @param stv The source string view
 * @param len Number of bytes to remove from the start
 * @return A new view with the prefix removed; an empty view if @p len >= stv.len
 */
LIB_STV_FN strview stv_removeStart(strview stv, size_t len);

/**
 * @brief Remove a given number of bytes from the end of the view
 *
 * Returns a new view that ends @p len bytes before the end of @p stv.
 * If @p len is greater than or equal to the view length, an empty view is returned.
 *
 * @param stv The source string view
 * @param len Number of bytes to remove from the end
 * @return A new view with the suffix removed; an empty view if @p len >= stv.len
 */
LIB_STV_FN strview stv_removeEnd(strview stv, size_t len);

/**
 * @brief Remove the prefix from the view if it starts with the given pattern
 *
 * Checks if @p stv starts with @p prefix; if so, returns the remainder of the view
 * after the prefix. If the prefix does not match, the original view is returned unchanged.
 *
 * @param stv    The source string view
 * @param prefix The prefix to remove (may be empty; an empty prefix matches and returns stv unchanged)
 * @return A view without the prefix, or the original view if it does not start with prefix
 */
LIB_STV_FN strview stv_removePrefix(strview stv, strview prefix);

/**
 * @brief Remove the suffix from the view if it ends with the given pattern
 *
 * Checks if @p stv ends with @p suffix; if so, returns the view without the suffix.
 * If the suffix does not match, the original view is returned unchanged.
 *
 * @param stv    The source string view
 * @param suffix The suffix to remove (may be empty; an empty suffix matches and returns stv unchanged)
 * @return A view without the suffix, or the original view if it does not end with suffix
 */
LIB_STV_FN strview stv_removeSuffix(strview stv, strview suffix);

/**
 * @brief Split a string view at the first occurrence of a separator
 *
 * Searches for the separator @p sep in @p stv. If found, returns the part before it and stores the remainder
 * (the part after the separator) in @p *remaining (if @p remaining is not NULL). If the separator is not found,
 * returns the entire @p stv and sets @p *remaining to an empty view.
 *
 * If @p sep is an empty view, the split occurs by single characters: the returned view contains the first
 * character, and @p *remaining receives the rest of the view (which may be empty if @p stv had length 1).
 *
 * When @p stv is empty, the function returns an empty view and does not modify @p *remaining.
 *
 * @param stv       The string view to split
 * @param sep       The separator view (may be empty)
 * @param remaining Optional pointer to a strview that receives the remainder after the separator;
 *                  can be NULL if the remainder is not needed
 * @return The part before the separator, or the entire view if the separator is not found
 */
LIB_STV_FN strview stv_split(strview stv, strview sep, strview* remaining);

/**
 * @brief Split the view at the first line break, supporting CR, LF, and CRLF
 *
 * Scans the view for the first occurrence of a carriage return ('\r') or newline ('\n').
 * Handles Windows‑style CRLF as a single line break. The returned line does not include
 * the line break characters. If no line break is found, the entire view is returned and
 * @p *remaining is set to an empty view. If the view is empty, it is returned unchanged
 * and @p *remaining is not modified.
 *
 * @param stv       The string view to split
 * @param remaining Optional pointer to a strview that receives the remainder after the line break
 *                  (can be NULL)
 * @return The current line (without trailing line break), or the entire view if no break found
 */
LIB_STV_FN strview stv_splitLines(strview stv, strview* remaining);

/**
 * @brief Split the view at the first word (sequence of non‑whitespace characters)
 *
 * Skips leading whitespace (as defined by stv_whitespace), then extracts the next contiguous
 * sequence of non‑whitespace characters as the current word. The remainder (starting from
 * the first character after the word) is stored in @p *remaining (if not NULL). If the view
 * contains no word (i.e., is empty or consists entirely of whitespace), an empty view is
 * returned and @p *remaining is set to an empty view. When @p stv is empty, @p *remaining
 * is not modified.
 *
 * @param stv       The string view to split
 * @param remaining Optional pointer to a strview that receives the remainder after the word
 *                  (can be NULL)
 * @return The current word (non‑whitespace sequence), or an empty view if no word is found
 */
LIB_STV_FN strview stv_splitWords(strview stv, strview* remaining);

/**
 * @brief Return the portion of the view before the first occurrence of a delimiter
 *
 * Locates the first occurrence of @p delim in @p stv. If found, returns the substring from the beginning up to,
 * but not including, the delimiter. If the delimiter is not found, the entire @p stv is returned.
 *
 * If @p delim is empty, the result is always an empty view. If @p stv is empty, an empty view is returned.
 *
 * @param stv   The string view to examine
 * @param delim The delimiter view (may be empty)
 * @return The part before the delimiter; empty view if @p delim is empty
 */
LIB_STV_FN strview stv_beforeDelim(strview stv, strview delim);

/**
 * @brief Return the portion of the view after the first occurrence of a delimiter
 *
 * Locates the first occurrence of @p delim in @p stv. If found, returns the substring that follows the
 * delimiter to the end of the view. If the delimiter is not found, an empty view is returned.
 *
 * If @p delim is empty, the entire @p stv is returned unchanged (the function behaves as if no split occurred).
 * If @p stv is empty, an empty view is returned.
 *
 * @param stv   The string view to examine
 * @param delim The delimiter view (may be empty)
 * @return The part after the delimiter; entire view if @p delim is empty, empty view if delimiter not found
 */
LIB_STV_FN strview stv_afterDelim(strview stv, strview delim);

/**
 * @brief Trim characters from both ends of a string view (generic)
 *
 * Dispatches to stv_trimChs() or stv_trimIf() based on the type of @p target:
 * - If @p target is a strview, it is treated as a character set.
 * - If @p target is a stv_charClassFn, it is treated as a character classification function.
 *
 * @param stv    Source string view
 * @param target A strview charset or a stv_charClassFn function pointer
 * @return A new view with leading and trailing matching characters removed
 */
#define stv_trim(stv, target) _Generic((target), strview: stv_trimChs, stv_charClassFn: stv_trimIf)((stv), (target))

/**
 * @brief Remove any characters belonging to a given charset from the beginning and end of the view
 *
 * @param stv Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with leading and trailing characters removed; if charset is empty, the original view is returned
 * unchanged
 */
LIB_STV_FN strview stv_trimChs(strview stv, strview charset);

/**
 * @brief Remove any characters satisfying a classification function from the beginning and end of the view
 *
 * @param stv Source string view
 * @param handle Character classification function (e.g., isspace). If NULL, no trimming is performed.
 * @return A new view with leading and trailing matching characters removed
 */
LIB_STV_FN strview stv_trimIf(strview stv, stv_charClassFn handle);

/**
 * @brief Trim characters from the beginning of a string view (generic)
 *
 * Dispatches to stv_trimStartChs() or stv_trimStartIf() based on the type of @p target:
 * - If @p target is a strview, it is treated as a character set.
 * - If @p target is a stv_charClassFn, it is treated as a character classification function.
 *
 * @param stv    Source string view
 * @param target A strview charset or a stv_charClassFn function pointer
 * @return A new view with leading matching characters removed
 */
#define stv_trimStart(stv, target)                                                                                     \
    _Generic((target), strview: stv_trimStartChs, stv_charClassFn: stv_trimStartIf)((stv), (target))

/**
 * @brief Remove any characters belonging to a given charset from the beginning of the view
 *
 * @param stv Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with leading characters removed; if charset is empty, the original view is returned unchanged
 */
LIB_STV_FN strview stv_trimStartChs(strview stv, strview charset);

/**
 * @brief Remove any characters satisfying a classification function from the beginning of the view
 *
 * @param stv Source string view
 * @param handle Character classification function (e.g., isspace). If NULL, no trimming is performed.
 * @return A new view with leading matching characters removed
 */
LIB_STV_FN strview stv_trimStartIf(strview stv, stv_charClassFn handle);

/**
 * @brief Trim characters from the end of a string view (generic)
 *
 * Dispatches to stv_trimEndChs() or stv_trimEndIf() based on the type of @p target:
 * - If @p target is a strview, it is treated as a character set.
 * - If @p target is a stv_charClassFn, it is treated as a character classification function.
 *
 * @param stv    Source string view
 * @param target A strview charset or a stv_charClassFn function pointer
 * @return A new view with trailing matching characters removed
 */
#define stv_trimEnd(stv, target)                                                                                       \
    _Generic((target), strview: stv_trimEndChs, stv_charClassFn: stv_trimEndIf)((stv), (target))

/**
 * @brief Remove any characters belonging to a given charset from the end of the view
 *
 * @param stv Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with trailing characters removed; if charset is empty, the original view is returned unchanged
 */
LIB_STV_FN strview stv_trimEndChs(strview stv, strview charset);

/**
 * @brief Remove any characters satisfying a classification function from the end of the view
 *
 * @param stv Source string view
 * @param handle Character classification function (e.g., isspace). If NULL, no trimming is performed.
 * @return A new view with trailing matching characters removed
 */
LIB_STV_FN strview stv_trimEndIf(strview stv, stv_charClassFn handle);

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
 * @brief Search for the last occurrence of a substring within a text view (automatic algorithm selection)
 *
 * Uses naive search when the pattern length is <= 4, otherwise uses Sunday search.
 * The search proceeds from right to left.
 *
 * @param stv_text The text view to search in
 * @param stv_pat The pattern view to search for
 * @return Start position of the last match; returns stv_text.len if stv_pat is empty, or stv_npos if not found
 */
LIB_STV_FN size_t stv_rev_search(strview stv_text, strview stv_pat);

/**
 * @brief Naive last-occurrence string search (suitable for short patterns)
 *
 * Searches from the end of the text towards the beginning.
 *
 * @param stv_text The text view to search in
 * @param stv_pat The pattern view to search for
 * @return Start position of the last match; returns stv_text.len if stv_pat is empty, or stv_npos if not found
 */
LIB_STV_FN size_t stv_rev_naiveSearch(strview stv_text, strview stv_pat);

/**
 * @brief Sunday last-occurrence string search (suitable for longer patterns)
 *
 * Adapts the Sunday algorithm to search from right to left.
 *
 * @param stv_text The text view to search in
 * @param stv_pat The pattern view to search for
 * @return Start position of the last match; returns stv_text.len if stv_pat is empty, or stv_npos if not found
 */
LIB_STV_FN size_t stv_rev_sundaySearch(strview stv_text, strview stv_pat);

/**
 * @brief Find the first occurrence of a character, charset member, or character class (generic)
 *
 * Dispatches based on the type of @p target:
 * - char:          calls stv_firstChar()
 * - strview:       calls stv_firstCharset()
 * - stv_charClassFn: calls stv_firstCharClass()
 *
 * @param stv    The string view to search
 * @param target A char, strview charset, or character classification function
 * @param invert If true, search for the first character NOT matching; otherwise search for the first match
 * @return Index of the first match, or stv_npos if not found / empty view
 */
#define stv_firstIndex(stv, target, invert)                                                                            \
    _Generic((target), int: stv_firstChar, strview: stv_firstCharset, stv_charClassFn: stv_firstCharClass)(            \
        (stv), (target), (invert))

/**
 * @brief Find the first occurrence of a specific character
 *
 * @param stv    The string view to search
 * @param ch     The character to locate
 * @param invert If true, search for the first character NOT equal to ch
 * @return Index of the first match, or stv_npos if not found or view is empty
 */
LIB_STV_FN size_t stv_firstChar(strview stv, const char ch, bool invert);

/**
 * @brief Find the first occurrence of any character from a charset
 *
 * @param stv     The string view to search
 * @param charset Character set view (empty charset always yields stv_npos)
 * @param invert  If true, search for the first character NOT in the charset
 * @return Index of the first match, or stv_npos if not found or view is empty
 */
LIB_STV_FN size_t stv_firstCharset(strview stv, strview charset, bool invert);

/**
 * @brief Find the first character that satisfies a classification function
 *
 * @param stv    The string view to search
 * @param handle Character classification function (e.g., isdigit). Must not be NULL.
 * @param invert If true, search for the first character that does NOT satisfy handle
 * @return Index of the first match, or stv_npos if not found or view is empty
 */
LIB_STV_FN size_t stv_firstCharClass(strview stv, stv_charClassFn handle, bool invert);

/**
 * @brief Find the last occurrence of a character, charset member, or character class (generic)
 *
 * Dispatches based on the type of @p target:
 * - char:          calls stv_lastChar()
 * - strview:       calls stv_lastCharset()
 * - stv_charClassFn: calls stv_lastCharClass()
 *
 * @param stv    The string view to search
 * @param target A char, strview charset, or character classification function
 * @param invert If true, search for the last character NOT matching; otherwise search for the last match
 * @return Index of the last match, or stv_npos if not found / empty view
 */
#define stv_lastIndex(stv, target, invert)                                                                             \
    _Generic((target), int: stv_lastChar, strview: stv_lastCharset, stv_charClassFn: stv_lastCharClass)(               \
        (stv), (target), (invert))

/**
 * @brief Find the last occurrence of a specific character
 *
 * @param stv    The string view to search
 * @param ch     The character to locate
 * @param invert If true, search for the last character NOT equal to ch
 * @return Index of the last match, or stv_npos if not found or view is empty
 */
LIB_STV_FN size_t stv_lastChar(strview stv, const char ch, bool invert);

/**
 * @brief Find the last occurrence of any character from a charset
 *
 * @param stv     The string view to search
 * @param charset Character set view (empty charset always yields stv_npos)
 * @param invert  If true, search for the last character NOT in the charset
 * @return Index of the last match, or stv_npos if not found or view is empty
 */
LIB_STV_FN size_t stv_lastCharset(strview stv, strview charset, bool invert);

/**
 * @brief Find the last character that satisfies a classification function
 *
 * @param stv    The string view to search
 * @param handle Character classification function (e.g., isdigit). Must not be NULL.
 * @param invert If true, search for the last character that does NOT satisfy handle
 * @return Index of the last match, or stv_npos if not found or view is empty
 */
LIB_STV_FN size_t stv_lastCharClass(strview stv, stv_charClassFn handle, bool invert);

/**
 * @brief Find the first position where two views differ (from left to right)
 *
 * Compares byte by byte from the beginning. If the views are identical (same pointer and length,
 * or same content and length), returns stv_npos. If one view is a prefix of the other, returns
 * the length of the shorter view. Otherwise returns the index of the first differing byte.
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return Index of first difference, or stv_npos if the views are identical
 */
LIB_STV_FN size_t stv_firstDiff(strview stv_left, strview stv_right);

/**
 * @brief Find the last position where two views differ (from right to left)
 *
 * Compares byte by byte from the end. If the views are identical, returns stv_npos.
 * If one view is a suffix of the other, returns the index of the first differing byte from the right
 * (relative to the longer view). For example, "hello" and "ello" differ at index 4 of the longer view.
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return Index of last difference, or stv_npos if the views are identical
 */
LIB_STV_FN size_t stv_lastDiff(strview stv_left, strview stv_right);

/**
 * @brief Count occurrences based on the type of target (generic)
 *
 * Dispatches to:
 * - stv_countChar()  if target is a char
 * - stv_countSubstr() if target is a strview
 * - stv_countIf()    if target is a stv_charClassFn
 *
 * @param stv    The string view to examine
 * @param target A char, a substring to count, or a character classification function
 * @return Number of occurrences (see individual functions for details on empty/stv_npos)
 */
#define stv_count(stv, target)                                                                                         \
    _Generic((target), int: stv_countChar, strview: stv_countSubstr, stv_charClassFn: stv_countIf)((stv), (target))

/**
 * @brief Count the number of characters that satisfy a classification function
 *
 * @param stv String view to examine
 * @param handle Character classification function (e.g., isdigit). If NULL, the function returns stv_npos.
 * @return Number of matching characters, or stv_npos if the view is empty or handle is NULL
 */
LIB_STV_FN size_t stv_countIf(strview stv, stv_charClassFn handle);

/**
 * @brief Count the occurrences of a specific character
 *
 * @param stv String view to examine
 * @param ch Character to count
 * @return Number of occurrences of ch, or stv_npos if the view is empty
 */
LIB_STV_FN size_t stv_countChar(strview stv, const char ch);

/**
 * @brief Count non-overlapping occurrences of a substring in the view
 *
 * Searches for @p sub repeatedly in @p stv from left to right, moving past each match.
 * The matches are non-overlapping: after a match, the search continues from the character
 * following the matched substring.
 *
 * @param stv The string view to search in
 * @param sub The substring to count (may be empty)
 * @return Number of non-overlapping occurrences, stv.len if @p sub is empty, or stv_npos if @p stv is empty
 */
LIB_STV_FN size_t stv_countSubstr(strview stv, strview sub);

/**
 * @brief Check if every character satisfies a condition (generic)
 *
 * Dispatches to stv_everyChar() or stv_everyIf() based on the type of @p target.
 *
 * @param stv    The string view to examine
 * @param target A char to compare, or a stv_charClassFn classification function
 * @return true if all characters satisfy the condition, false otherwise (empty view returns false)
 */
#define stv_every(stv, target) _Generic((target), int: stv_everyChar, stv_charClassFn: stv_everyIf)((stv), (target))

/**
 * @brief Check if every character in the view satisfies a classification function
 *
 * An empty view or a NULL handle will cause the function to return false.
 *
 * @param stv String view to examine
 * @param handle Character classification function (must not be NULL for a meaningful result)
 * @return true if the view and handle is non-empty and all characters match the class; false otherwise
 */
LIB_STV_FN bool stv_everyIf(strview stv, stv_charClassFn handle);

/**
 * @brief Check if every character in the view equals a given character
 *
 * An empty view always returns false.
 *
 * @param stv String view to examine
 * @param ch Character to compare against
 * @return true if the view is non-empty and all characters equal ch; false otherwise
 */
LIB_STV_FN bool stv_everyChar(strview stv, const char ch);

/**
 * @brief Check if at least one character satisfies a condition (generic)
 *
 * Dispatches to stv_someChar() or stv_someIf() based on the type of @p target.
 *
 * @param stv    The string view to examine
 * @param target A char to search for, or a stv_charClassFn classification function
 * @return true if at least one character satisfies the condition, false otherwise (empty view returns false)
 */
#define stv_some(stv, target) _Generic((target), int: stv_someChar, stv_charClassFn: stv_someIf)((stv), (target))

/**
 * @brief Check if at least one character in the view satisfies a classification function
 *
 * @param stv String view to examine
 * @param handle Character classification function (must not be NULL)
 * @return true if the view and handle is non-empty and at least one character matches the class; false otherwise
 */
LIB_STV_FN bool stv_someIf(strview stv, stv_charClassFn handle);

/**
 * @brief Check if at least one occurrence of a given character exists in the view
 *
 * @param stv String view to examine
 * @param ch Character to search for
 * @return true if the view is non-empty and contains ch; false otherwise
 */
LIB_STV_FN bool stv_someChar(strview stv, const char ch);

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
 * @param stv_sub The pattern to search for (empty pattern is considered as contained)
 * @return true if stv_pat appears in stv_text, false otherwise
 */
LIB_STV_FN bool stv_contains(strview stv_text, strview stv_sub);

/**
 * @brief Check if two views have identical content (byte-by-byte comparison)
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return true if contents are equal, false otherwise
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
 * @brief Compare two string views lexicographically
 *
 * @param stv_left Left view
 * @param stv_right Right view
 * @return Negative if left < right, 0 if equal, positive if left > right
 */
LIB_STV_FN int stv_compare(strview stv_left, strview stv_right);

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
 * @brief Access a character at the specified index
 *
 * Returns the character at position @p idx. If the view is empty or the index is out of bounds,
 * the null character '\0' is returned.
 *
 * @param stv String view
 * @param idx Zero-based index
 * @return The character at idx, or '\0' if out of range
 */
LIB_STV_FN char stv_at(strview stv, size_t idx);

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
 * @brief Compute a hash value for the string view
 *
 * Default using stv_hash_FNV1a.
 *
 * @param stv The string view to hash
 * @return The computed hash value (0 if the view is empty)
 */
LIB_STV_FN size_t stv_hash(strview stv);

/**
 * @brief Compute an FNV-1a hash for the string view
 *
 * Uses the FNV-1a hash algorithm, selecting the 16-bit, 32-bit or 64-bit:
 * - 64‑bit when SIZE_MAX == UINT64_MAX
 * - 32‑bit when SIZE_MAX == UINT32_MAX
 * - 16‑bit when SIZE_MAX == UINT16_MAX
 *
 * Other platform's size_t width and empty view returns 0.
 *
 * @param stv The string view to hash
 * @return The computed FNV-1a hash (0 if the view is empty)
 */
LIB_STV_FN size_t stv_hash_FNV1a(strview stv);

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
 * @brief Copy the string view into a C string buffer with optional transformations
 *
 * @param stv Source string view
 * @param mem Destination buffer
 * @param size Buffer size in bytes
 * @param opts Bitwise combination of stv_cstrOptions to apply
 * @return mem if the buffer is large enough to hold the view content and the null terminator, otherwise NULL
 */
LIB_STV_FN char* stv_opt_cstr(strview stv, char* mem, size_t size, stv_cstrOptions opts);

/**
 * @brief Convert a character to its numeric digit value (0-35)
 *
 * @param ch Input character
 * @return Digit value 0-35 for '0'-'9', 'A'-'Z', 'a'-'z'; -1 if invalid
 */
LIB_STV_FN int stv_ch2digit(char ch);

/**
 * @brief Detect numeric base from the prefix of a string view
 *
 * Supports 0b/0B for binary, 0o/0O for octal, 0d/0D for decimal, 0x/0X for hexadecimal.
 * A lone leading '0' without a recognised prefix is treated as decimal and is consumed as part of the number.
 * If no prefix is found, the base defaults to 10 and the view is unchanged.
 *
 * @param stv Input view to examine
 * @param remaining Optional output pointer to receive the portion after the prefix (if any).
 *                  If NULL, the remainder is discarded.
 * @return The detected base (2, 8, 10, or 16). Returns 0 if the view is empty.
 */
LIB_STV_FN int stv_parseIntBase(strview stv, strview* remaining);

/**
 * @brief Parse a signed integer from a string view
 *
 * Skips leading whitespace, handles an optional '+'/'-' sign, and then parses digits
 * in the specified base. If base is 0, it is auto-detected via stv_parseIntBase().
 * Overflows are clamped to INTMAX_MAX / INTMAX_MIN and the remaining view is updated.
 *
 * @param stv Input view
 * @param base Numeric base (2-36), or 0 for auto-detection
 * @param remaining Optional output pointer to receive the portion after the parsed number.
 *                  If NULL, the remainder is discarded.
 * @return The parsed value; 0 if no digits found or base invalid; clamped on overflow.
 */
LIB_STV_FN intmax_t stv_parseInum(strview stv, int base, strview* remaining);

/**
 * @brief Parse an unsigned integer from a string view
 *
 * Similar to stv_parseInum() but returns uintmax_t. Negative values are converted
 * via modulo arithmetic (e.g., "-40" yields UINTMAX_MAX - 39). Overflows are clamped
 * to UINTMAX_MAX.
 *
 * @param stv Input view
 * @param base Numeric base (2-36), or 0 for auto-detection
 * @param remaining Optional output pointer for the remainder
 * @return The parsed value; 0 if no digits or invalid base; clamped on overflow.
 */
LIB_STV_FN uintmax_t stv_parseUnum(strview stv, int base, strview* remaining);

/**
 * @brief Convenience macro: construct a strview from a given data pointer and length
 *
 * @param data_v Pointer to the data
 * @param len_v Length
 */
#ifdef __cplusplus
    #define stv_makestv(data_v, len_v) (strview{/* .data= */ (data_v), /* .len= */ (len_v)})
#else
    #define stv_makestv(data_v, len_v) ((strview){.data = (data_v), .len = (len_v)})
#endif

/** @brief Convenience macro: construct a strview from literal string */
#define stv_literal(str) stv_makestv((str), sizeof(str) - 1)

/** @brief Predefined empty string view (data = nullptr, len = 0) */
#define stv_nullstv stv_makestv(nullptr, 0)

/** @brief Predefined whitespace character set view (space, carriage return, newline, tab, vertical tab, form feed) */
#define stv_whitespace stv_literal(" \r\n\t\v\f")

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

LIB_STV_FN strview stv_removeStart(strview stv, size_t len) {
    return stv_slice(stv, len, stv_end);
}

LIB_STV_FN strview stv_removeEnd(strview stv, size_t len) {
    return stv_slice(stv, stv_begin, stv.len - len);
}

LIB_STV_FN strview stv_removePrefix(strview stv, strview prefix) {
    if (stv_startsWith(stv, prefix)) {
        return stv_slice(stv, prefix.len, stv_end);
    }
    return stv;
}

LIB_STV_FN strview stv_removeSuffix(strview stv, strview suffix) {
    if (stv_endsWith(stv, suffix)) {
        return stv_slice(stv, stv_begin, stv.len - suffix.len);
    }
    return stv;
}

LIB_STV_FN strview stv_split(strview stv, strview sep, strview* remaining) {
    if (stv_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return stv;
    }

    const bool   empty_sep = stv_empty(sep);
    const size_t idx       = empty_sep ? 1 : stv_search(stv, sep);
    const size_t len       = empty_sep ? 0 : sep.len;
    const bool   split_end = (idx == stv_npos);

    if (remaining) {
        *remaining = split_end ? stv_nullstv : stv_slice(stv, idx + len, stv_end);
    }
    return split_end ? stv : stv_slice(stv, stv_begin, idx);
}

LIB_STV_FN strview stv_splitLines(strview stv, strview* remaining) {
    if (stv_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return stv;
    }

    const size_t idx = stv_firstCharset(stv, stv_literal("\r\n"), false);
    if (idx == stv_npos) {
        if (remaining) {
            *remaining = stv_nullstv;
        }
        return stv;
    }

    const bool   is_CRLF   = (stv.data[idx] == '\r' && (idx + 1) < stv.len && stv.data[idx + 1] == '\n');
    const size_t nl_start  = idx + (is_CRLF ? 2 : 1);
    const bool   split_end = (nl_start >= stv.len);

    if (remaining) {
        *remaining = split_end ? stv_nullstv : stv_slice(stv, nl_start, stv_end);
    }
    return stv_slice(stv, stv_begin, idx);
}

LIB_STV_FN strview stv_splitWords(strview stv, strview* remaining) {
    if (stv_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return stv;
    }

    const size_t word_start = stv_firstCharset(stv, stv_whitespace, true);
    if (word_start == stv_npos) {
        if (remaining) {
            *remaining = stv_nullstv;
        }
        return stv_nullstv;
    }

    const strview tail      = stv_slice(stv, word_start, stv_end);
    const size_t  idx       = stv_firstCharset(tail, stv_whitespace, false);
    const size_t  word_end  = word_start + ((idx == stv_npos) ? tail.len : idx);
    const bool    split_end = (word_end >= stv.len);

    if (remaining) {
        *remaining = split_end ? stv_nullstv : stv_makestv(stv.data + word_end, stv.len - word_end);
    }
    return stv_slice(stv, word_start, word_end);
}

LIB_STV_FN strview stv_beforeDelim(strview stv, strview delim) {
    if (stv_empty(stv)) {
        return stv;
    }
    if (stv_empty(delim)) {
        return stv_nullstv;
    }
    const size_t pos = stv_search(stv, delim);
    return stv_slice(stv, stv_begin, pos);
}

LIB_STV_FN strview stv_afterDelim(strview stv, strview delim) {
    if (stv_empty(stv) || stv_empty(delim)) {
        return stv;
    }
    const size_t pos = stv_search(stv, delim);
    if (pos == stv_npos) {
        return stv_nullstv;
    }
    return stv_slice(stv, pos + delim.len, stv_end);
}

LIB_STV_FN strview stv_trimChs(strview stv, strview charset) {
    return stv_trimStartChs(stv_trimEndChs(stv, charset), charset);
}

LIB_STV_FN strview stv_trimStartChs(strview stv, strview charset) {
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

LIB_STV_FN strview stv_trimEndChs(strview stv, strview charset) {
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

LIB_STV_FN strview stv_trimIf(strview stv, stv_charClassFn handle) {
    return stv_trimStartIf(stv_trimEndIf(stv, handle), handle);
}

LIB_STV_FN strview stv_trimStartIf(strview stv, stv_charClassFn handle) {
    if (stv_empty(stv) || handle == nullptr) {
        return stv;
    }
    const char* end_pos = stv.data + stv.len;
    const char* ch      = stv.data;
    size_t      tc      = 0;
    while (ch < end_pos) {
        if (handle(*ch)) {
            ch++, tc++;
        } else {
            break;
        }
    }
    return stv_makestv(stv.data + tc, stv.len - tc);
}

LIB_STV_FN strview stv_trimEndIf(strview stv, stv_charClassFn handle) {
    if (stv_empty(stv) || handle == nullptr) {
        return stv;
    }
    const char* start_pos = stv.data;
    const char* ch        = stv.data + stv.len;
    size_t      tc        = 0;
    while (ch > start_pos) {
        ch--;
        if (handle(*ch)) {
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

    const size_t end_idx = (stv_text.len - stv_pat.len);
    for (size_t idx = 0; idx <= end_idx; idx++) {
        size_t i;
        for (i = 0; i < stv_pat.len; i++) {
            if (stv_text.data[idx + i] != stv_pat.data[i]) {
                break;
            }
        }
        if (i == stv_pat.len) {
            return idx;
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

    size_t txl = stv_text.len, pal = stv_pat.len;
    size_t shift[UCHAR_MAX + 1];
    for (size_t i = 0; i <= UCHAR_MAX; i++) {
        shift[i] = pal + 1;
    }
    for (size_t i = 0; i < pal; i++) {
        shift[(unsigned char)stv_pat.data[i]] = pal - i;
    }

    size_t idx = 0;
    while (true) {
        size_t i;
        for (i = 0; i < pal; i++) {
            if (stv_text.data[idx + i] != stv_pat.data[i]) {
                if (idx + pal >= txl) {
                    return stv_npos;
                }
                const unsigned char next_char = stv_text.data[idx + pal];
                const size_t        skip      = shift[next_char];
                idx += skip;
                break;
            }
        }
        if (i == stv_pat.len) {
            return idx;
        }
    }
}

LIB_STV_FN size_t stv_rev_search(strview stv_text, strview stv_pat) {
    if (stv_pat.len > 4) {
        return stv_rev_sundaySearch(stv_text, stv_pat);
    } else {
        return stv_rev_naiveSearch(stv_text, stv_pat);
    }
}

LIB_STV_FN size_t stv_rev_naiveSearch(strview stv_text, strview stv_pat) {
    if (stv_empty(stv_pat)) {
        return stv_text.len;
    }
    if (stv_empty(stv_text) || stv_text.len < stv_pat.len) {
        return stv_npos;
    }

    const size_t begin_idx = (stv_text.len - stv_pat.len + 1);
    for (size_t idx = begin_idx; idx > 0; idx--) {
        size_t i;
        for (i = 0; i < stv_pat.len; i++) {
            if (stv_text.data[idx - 1 + i] != stv_pat.data[i]) {
                break;
            }
        }
        if (i == stv_pat.len) {
            return idx - 1;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_rev_sundaySearch(strview stv_text, strview stv_pat) {
    if (stv_empty(stv_pat)) {
        return stv_text.len;
    }
    if (stv_empty(stv_text) || stv_text.len < stv_pat.len) {
        return stv_npos;
    }

    size_t txl = stv_text.len, pal = stv_pat.len;
    size_t shift[UCHAR_MAX + 1];
    for (size_t i = 0; i <= UCHAR_MAX; i++) {
        shift[i] = pal + 1;
    }
    for (size_t i = pal; i > 0; i--) {
        unsigned char c = (unsigned char)stv_pat.data[i - 1];
        shift[c]        = i;
    }

    size_t idx = txl - pal;
    while (true) {
        size_t i;
        for (i = 0; i < pal; i++) {
            if (stv_text.data[idx + i] != stv_pat.data[i]) {
                if (idx == 0) {
                    return stv_npos;
                }
                const unsigned char prev_char = stv_text.data[idx - 1];
                const size_t        skip      = shift[prev_char];
                if (idx < skip) {
                    return stv_npos;
                }
                idx -= skip;
                break;
            }
        }
        if (i == pal) {
            return idx;
        }
    }
}

LIB_STV_FN size_t stv_firstChar(strview stv, const char ch, bool invert) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    const char*  curr = stv.data;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        if ((*curr == ch) != invert) {
            return idx;
        }
        curr++;
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_lastChar(strview stv, const char ch, bool invert) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    const char*  curr = stv.data + stv.len;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        curr--;
        if ((*curr == ch) != invert) {
            return len - idx - 1;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_firstCharset(strview stv, strview charset, bool invert) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    bool chs[UCHAR_MAX + 1] = {0};
    for (size_t idx = 0; idx < charset.len; idx++) {
        chs[(unsigned char)charset.data[idx]] = true;
    }

    const char*  curr = stv.data;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        if (chs[(unsigned char)*curr] != invert) {
            return idx;
        }
        curr++;
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_lastCharset(strview stv, strview charset, bool invert) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    bool chs[UCHAR_MAX + 1] = {0};
    for (size_t idx = 0; idx < charset.len; idx++) {
        chs[(unsigned char)charset.data[idx]] = true;
    }

    const char*  curr = stv.data + stv.len;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        curr--;
        if (chs[(unsigned char)*curr] != invert) {
            return len - idx - 1;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_firstCharClass(strview stv, stv_charClassFn handle, bool invert) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    const char*  curr = stv.data;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        const bool ret = handle(*curr);
        if (ret != invert) {
            return idx;
        }
        curr++;
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_lastCharClass(strview stv, stv_charClassFn handle, bool invert) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    const char*  curr = stv.data + stv.len;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        curr--;
        const bool ret = handle(*curr);
        if (ret != invert) {
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

LIB_STV_FN size_t stv_countIf(strview stv, stv_charClassFn handle) {
    if (stv_empty(stv) || handle == nullptr) {
        return stv_npos;
    }

    size_t      sum     = 0;
    const char* pos     = stv.data;
    const char* end_pos = stv.data + stv.len;
    while (pos < end_pos) {
        if (handle(*pos)) {
            sum++;
        }
        pos++;
    }
    return sum;
}

LIB_STV_FN size_t stv_countChar(strview stv, const char ch) {
    if (stv_empty(stv)) {
        return stv_npos;
    }

    size_t      sum     = 0;
    const char* pos     = stv.data;
    const char* end_pos = stv.data + stv.len;
    while (pos < end_pos) {
        if (*pos == ch) {
            sum++;
        }
        pos++;
    }
    return sum;
}

LIB_STV_FN size_t stv_countSubstr(strview stv, strview sub) {
    if (stv_empty(stv)) {
        return stv_npos;
    }
    if (stv_empty(sub)) {
        return stv.len;
    }
    size_t sum = 0, len = sub.len;
    while (true) {
        const size_t pos = stv_search(stv, sub);
        if (pos == stv_npos) {
            break;
        }
        stv = stv_slice(stv, pos + len, stv_end);
        sum++;
    }
    return sum;
}

LIB_STV_FN bool stv_everyIf(strview stv, stv_charClassFn handle) {
    const size_t sum = stv_countIf(stv, handle);
    return sum != stv_npos && sum == stv.len;
}

LIB_STV_FN bool stv_everyChar(strview stv, const char ch) {
    const size_t sum = stv_countChar(stv, ch);
    return sum != stv_npos && sum == stv.len;
}

LIB_STV_FN bool stv_someIf(strview stv, stv_charClassFn handle) {
    const size_t sum = stv_countIf(stv, handle);
    return sum != stv_npos && sum > 0;
}

LIB_STV_FN bool stv_someChar(strview stv, const char ch) {
    const size_t sum = stv_countChar(stv, ch);
    return sum != stv_npos && sum > 0;
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

LIB_STV_FN bool stv_contains(strview stv_text, strview stv_sub) {
    return stv_empty(stv_sub) || stv_search(stv_text, stv_sub) != stv_npos;
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

LIB_STV_FN int stv_compare(strview stv_left, strview stv_right) {
    const size_t        pos = stv_firstDiff(stv_left, stv_right);
    const unsigned char c1  = (pos < stv_left.len) ? stv_left.data[pos] : '\0';
    const unsigned char c2  = (pos < stv_right.len) ? stv_right.data[pos] : '\0';
    return c1 - c2;
}

LIB_STV_FN char stv_front(strview stv) {
    return (stv_empty(stv) ? '\0' : stv.data[0]);
}

LIB_STV_FN char stv_back(strview stv) {
    return (stv_empty(stv) ? '\0' : stv.data[stv.len - 1]);
}

LIB_STV_FN char stv_at(strview stv, size_t idx) {
    return ((stv_empty(stv) || idx >= stv.len) ? '\0' : stv.data[idx]);
}

LIB_STV_FN void stv_swap(strview* stv_left, strview* stv_right) {
    if (stv_left && stv_right) {
        const strview tmp = *stv_left;
        *stv_left         = *stv_right;
        *stv_right        = tmp;
    }
}

LIB_STV_FN size_t stv_hash(strview stv) {
    return stv_hash_FNV1a(stv);
}

LIB_STV_FN size_t stv_hash_FNV1a(strview stv) {
    if (stv_empty(stv)) {
        return 0;
    }

    size_t fnv_offset_basis, fnv_prime;
    switch (SIZE_MAX) {
    case UINT64_MAX:
        fnv_offset_basis = 0xcbf29ce484222325ULL, fnv_prime = 0x00000100000001b3ULL;
        break;
    case UINT32_MAX:
        fnv_offset_basis = 0x811c9dc5UL, fnv_prime = 0x01000193UL;
        break;
    case UINT16_MAX:
        fnv_offset_basis = 0x811cU, fnv_prime = 0x0101U;
        break;
    default:
        return 0;
    }

    size_t hash = fnv_offset_basis;
    for (size_t i = 0; i < stv.len; i++) {
        hash ^= (unsigned char)stv.data[i];
        hash *= fnv_prime;
    }
    return hash;
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

LIB_STV_FN char* stv_opt_cstr(strview stv, char* mem, size_t size, stv_cstrOptions opts) {
    const bool UPPER   = (opts & stv_ToUpper);
    const bool LOWER   = (opts & stv_ToLower);
    const bool REVERSE = (opts & stv_Reverse);

    if (mem == nullptr || size <= stv.len) {
        return nullptr;
    }
    const size_t endidx = stv_empty(stv) ? 0 : stv.len;
    if (endidx > 0) {
        for (size_t idx = 0, ridx = endidx - 1; idx < endidx; idx++, ridx--) {
            char ch = stv.data[(REVERSE ? ridx : idx)];
            if (UPPER && ch >= 'a' && ch <= 'z') {
                ch = ch & ~32;
            } else if (LOWER && ch >= 'A' && ch <= 'Z') {
                ch = ch | 32;
            }
            mem[idx] = ch;
        }
    }
    mem[endidx] = '\0';
    return mem;
}

LIB_STV_FN intmax_t stv_parseInum(strview stv, int base, strview* remaining) {
    stv = stv_trimStart(stv, stv_whitespace);
    if (stv_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return 0;
    }

    const char neg_ch   = *(stv.data);
    const bool negative = (neg_ch == '-');
    if (neg_ch == '-' || neg_ch == '+') {
        stv = stv_makestv(stv.data + 1, stv.len - 1);
    }

    bool auto_base = (base == 0);
    base           = auto_base ? stv_parseIntBase(stv, &stv) : base;
    if (base < 2 || base > 36) {
        if (remaining) {
            *remaining = stv;
        }
        return 0;
    }

    if (!auto_base) {
        const char* up = (base == 2) ? "0B" : (base == 8) ? "0O" : (base == 10) ? "0D" : (base == 16) ? "0X" : nullptr;
        const char* lw = (base == 2) ? "0b" : (base == 8) ? "0o" : (base == 10) ? "0d" : (base == 16) ? "0x" : nullptr;
        if (up) {
            if (stv_startsWith(stv, stv_makestv(up, 2)) || stv_startsWith(stv, stv_makestv(lw, 2))) {
                stv = stv_makestv(stv.data + 2, stv.len - 2);
            }
        }
    }

    const char*     pos      = stv.data;
    const char*     end_pos  = stv.data + stv.len;
    const uintmax_t limit    = negative ? (uintmax_t)INTMAX_MAX + 1 : (uintmax_t)INTMAX_MAX;
    uintmax_t       acc      = 0;
    bool            overflow = false;

    while (pos < end_pos) {
        int d = stv_ch2digit(*pos);
        if (d < 0 || d >= base) {
            break;
        }

        if (!overflow) {
            if (acc > limit / base || (acc == limit / base && d > (int)(limit % base))) {
                overflow = true;
            } else {
                acc = acc * base + d;
            }
        }
        pos++;
    }

    if (remaining) {
        *remaining = stv_makestv(pos, end_pos - pos);
    }

    if (overflow) {
        return negative ? INTMAX_MIN : INTMAX_MAX;
    }

    return negative ? -(intmax_t)acc : (intmax_t)acc;
}

LIB_STV_FN uintmax_t stv_parseUnum(strview stv, int base, strview* remaining) {
    stv = stv_trimStart(stv, stv_whitespace);
    if (stv_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return 0;
    }

    const char neg_ch   = *(stv.data);
    const bool negative = (neg_ch == '-');
    if (neg_ch == '-' || neg_ch == '+') {
        stv = stv_makestv(stv.data + 1, stv.len - 1);
    }

    bool auto_base = (base == 0);
    base           = auto_base ? stv_parseIntBase(stv, &stv) : base;
    if (base < 2 || base > 36) {
        if (remaining) {
            *remaining = stv;
        }
        return 0;
    }

    if (!auto_base) {
        const char* up = (base == 2) ? "0B" : (base == 8) ? "0O" : (base == 10) ? "0D" : (base == 16) ? "0X" : nullptr;
        const char* lw = (base == 2) ? "0b" : (base == 8) ? "0o" : (base == 10) ? "0d" : (base == 16) ? "0x" : nullptr;
        if (up) {
            if (stv_startsWith(stv, stv_makestv(up, 2)) || stv_startsWith(stv, stv_makestv(lw, 2))) {
                stv = stv_makestv(stv.data + 2, stv.len - 2);
            }
        }
    }

    const char*     pos      = stv.data;
    const char*     end_pos  = stv.data + stv.len;
    const uintmax_t limit    = UINTMAX_MAX;
    uintmax_t       acc      = 0;
    bool            overflow = false;

    while (pos < end_pos) {
        int d = stv_ch2digit(*pos);
        if (d < 0 || d >= base) {
            break;
        }

        if (!overflow) {
            if (acc > limit / base || (acc == limit / base && d > (int)(limit % base))) {
                overflow = true;
            } else {
                acc = acc * base + d;
            }
        }
        pos++;
    }

    if (remaining) {
        *remaining = stv_makestv(pos, end_pos - pos);
    }

    if (overflow) {
        return UINTMAX_MAX;
    }

    return negative ? -(uintmax_t)acc : acc;
}

LIB_STV_FN int stv_ch2digit(char ch) {
    return (ch >= '0' && ch <= '9') ? (ch - '0')
         : (ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 10)
         : (ch >= 'a' && ch <= 'z') ? (ch - 'a' + 10)
                                    : -1;
}

LIB_STV_FN int stv_parseIntBase(strview stv, strview* remaining) {
    if (stv_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return 0;
    }

    int        base      = 10;
    size_t     num_start = 2;
    const char zero_ch   = stv.data[0];
    if (zero_ch == '0' && stv.len >= 2) {
        const char base_ch = (stv.data[1] | 32);
        if (base_ch == 'b') {
            base = 2;
        } else if (base_ch == 'o') {
            base = 8;
        } else if (base_ch == 'd') {
            base = 10;
        } else if (base_ch == 'x') {
            base = 16;
        } else {
            num_start = stv_begin;
        }
    } else {
        num_start = stv_begin;
    }

    if (remaining) {
        *remaining = stv_slice(stv, num_start, stv_end);
    }
    return base;
}

#endif // !LIB_STV_IMPL

#ifdef __cplusplus
}
#endif // !__cplusplus

#endif // !LIB_STV_H
