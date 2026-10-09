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
 * Provides operations on read‑only string views (`strview`), supporting C99 and C++11 or later compilers.
 * All functions allocate no memory and do not modify the original string.
 * By default, only declarations are included; define the `LIB_STV_IMPL` macro before one inclusion to compile the
 * implementation. Alternatively, define `LIB_STV_STATIC_INLINE_IMPL` to compile the implementation as `static inline`.
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
#endif // LIB_STV_STATIC_INLINE_IMPL

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define LIB_STV_GENERIC
#endif

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
 * Represents a read‑only string fragment, consisting of a pointer to the first character and a length.
 * It is not null‑terminated and does not own the underlying data.
 */
typedef struct {
    const char* data; ///< Pointer to the string data (may be NULL)
    size_t      len;  ///< Length of the string in bytes
} strview;

/**
 * @brief Character classification function pointer type
 *
 * Accepts a character (as `int`) and returns non‑zero if the character belongs to the class.
 * Typical examples include `isalnum()`, `islower()`, etc.
 */
typedef int (*stv_charClassFn)(int);

/**
 * @brief Callback type for stv_forEach() and stv_forEachRev()
 *
 * @param ch  Current character
 * @param idx Zero‑based index of the character (descending in stv_forEachRev)
 * @param ctx User‑provided context pointer, passed through unchanged from the
 *            calling function
 */
typedef void (*stv_forEachFn)(char ch, size_t idx, void* ctx);

/**
 * @brief Output options for stv_cstr() and stv_join() (bitwise combinable)
 *
 * Each option can be combined using bitwise OR. They control character case,
 * byte reversal, truncation, and array iteration order during copy.
 *
 * - `stv_Default`     (0x00): No transformation; plain byte copy.
 *
 * - `stv_ToUpper`     (0x01): For each output byte, convert ASCII `'a'`-`'z'` to
 *                             `'A'`-`'Z'`. All other bytes are copied unchanged.
 *
 * - `stv_ToLower`     (0x02): For each output byte, convert ASCII `'A'`-`'Z'` to
 *                             `'a'`-`'z'`. All other bytes are copied unchanged.
 *
 * - `stv_Reverse`     (0x04): Reverse the byte order of the *entire* output written
 *                             to `mem`. For `stv_join`, the reversal is performed on
 *                             the fully joined result, not on individual elements.
 *
 * - `stv_Truncate`    (0x08): If the output would not fit in `mem`, write as much as
 *                             possible (keeping space for the null terminator) and
 *                             return `mem`. When NOT set, `stv_cstr` / `stv_join`
 *                             return `NULL` on overflow and leave `mem` untouched.
 *
 * - `stv_JoinReverse` (0x10): Only honoured by `stv_join`. Iterate `stv_arr` from the
 *                             last element to the first. Ignored by `stv_cstr`.
 *
 * - `stv_ViewReverse` (0x20): Read each source view from its last byte to its first.
 *                             In `stv_cstr`, this changes which source bytes are
 *                             consumed (see truncation note below). In `stv_join`, it
 *                             is applied independently to each element *before* any
 *                             whole-buffer `stv_Reverse` reversal.
 *
 * @note `stv_Reverse` and `stv_ViewReverse` both invert byte order, but at different
 *       stages:
 *       - `stv_cstr` without truncation: both flags together cancel out, because
 *         every source byte is read backwards and then written backwards.
 *       - `stv_cstr` with truncation (`size - 1 < stv.len`): they no longer cancel.
 *         `stv_ViewReverse` selects the *last* `size - 1` source bytes, whereas the
 *         default would select the *first* `size - 1` bytes.
 *       - `stv_join`: the two flags never cancel, because `stv_ViewReverse` acts on
 *         each element while `stv_Reverse` acts once on the whole joined buffer.
 *
 * @note When both `stv_ToUpper` and `stv_ToLower` are set, the effective behaviour is
 *       swap-case for ASCII letters (e.g., `"Abc"` -> `"aBC"`), because the upper
 *       branch is tested first and the lower branch handles the remaining letters.
 */
typedef enum {
    stv_Default     = 0,  // 0b00000000
    stv_ToUpper     = 1,  // 0b00000001
    stv_ToLower     = 2,  // 0b00000010
    stv_Reverse     = 4,  // 0b00000100
    stv_Truncate    = 8,  // 0b00001000
    stv_JoinReverse = 16, // 0b00010000
    stv_ViewReverse = 32, // 0b00100000
} stv_cstrOptions;

/**
 * @brief Create a string view from a null‑terminated C string
 *
 * @param c_str Pointer to a null‑terminated C string; may be NULL
 * @return A new `strview`; returns an empty view if `c_str` is NULL
 */
LIB_STV_FN strview stv_new(const char* c_str);

/**
 * @brief Create a string view from a character sequence, stopping at a specified terminator or maximum length
 *
 * @param str     Source string pointer; may be NULL
 * @param endchar Terminator character (stop *before* this character; the character itself is not included)
 * @param maxlen  Maximum number of bytes to scan
 * @return A new `strview`; returns an empty view if `str` is NULL
 */
LIB_STV_FN strview stv_create(const char* str, unsigned char endchar, size_t maxlen);

/**
 * @brief Extract a substring view
 *
 * @param stv       Source string view
 * @param begin_pos Start position (inclusive)
 * @param end_pos   End position (exclusive); use `stv_end` to indicate up to the end of the view
 * @return The substring view `[begin_pos, end_pos)` if the range is valid; otherwise an empty view
 */
LIB_STV_FN strview stv_slice(strview stv, size_t begin_pos, size_t end_pos);

/**
 * @brief Remove a given number of bytes from the beginning of the view
 *
 * Returns a new view that starts `len` bytes from the beginning of `stv`.
 * If `len` is greater than or equal to the view length, an empty view is returned.
 *
 * @param stv The source string view
 * @param len Number of bytes to remove from the start
 * @return A new view with the prefix removed; an empty view if `len >= stv.len`
 */
LIB_STV_FN strview stv_removeStart(strview stv, size_t len);

/**
 * @brief Remove a given number of bytes from the end of the view
 *
 * Returns a new view that ends `len` bytes before the end of `stv`.
 * If `len` is greater than or equal to the view length, an empty view is returned.
 *
 * @param stv The source string view
 * @param len Number of bytes to remove from the end
 * @return A new view with the suffix removed; an empty view if `len >= stv.len`
 */
LIB_STV_FN strview stv_removeEnd(strview stv, size_t len);

/**
 * @brief Remove the prefix from the view if it starts with the given pattern
 *
 * Checks if `stv` starts with `prefix`; if so, returns the remainder of the view
 * after the prefix. If the prefix does not match, the original view is returned unchanged.
 *
 * @param stv    The source string view
 * @param prefix The prefix to remove (may be empty; an empty prefix matches and returns `stv` unchanged)
 * @param nocase If true, the prefix comparison ignores ASCII letter case
 * @return A view without the prefix, or the original view if it does not start with `prefix`
 */
LIB_STV_FN strview stv_removePrefix(strview stv, strview prefix, bool nocase);

/**
 * @brief Remove the suffix from the view if it ends with the given pattern
 *
 * Checks if `stv` ends with `suffix`; if so, returns the view without the suffix.
 * If the suffix does not match, the original view is returned unchanged.
 *
 * @param stv    The source string view
 * @param suffix The suffix to remove (may be empty; an empty suffix matches and returns `stv` unchanged)
 * @param nocase If true, the suffix comparison ignores ASCII letter case
 * @return A view without the suffix, or the original view if it does not end with `suffix`
 */
LIB_STV_FN strview stv_removeSuffix(strview stv, strview suffix, bool nocase);

/**
 * @brief Split a string view at the first occurrence of a separator
 *
 * Searches for the separator `sep` in `stv`. If found, returns the part before it and stores the remainder
 * (the part after the separator) in `*remaining` (if `remaining` is not NULL). If the separator is not found,
 * returns the entire `stv` and sets `*remaining` to an empty view.
 *
 * If `sep` is an empty view, the split occurs by single characters: the returned view contains the first
 * character, and `*remaining` receives the rest of the view (which may be empty if `stv` had length 1).
 *
 * When `stv` is empty, the function returns an empty view and sets `*remaining` to the empty view
 * (if `remaining` is not NULL).
 *
 * @param stv       The string view to split
 * @param sep       The separator view (may be empty)
 * @param remaining Optional pointer to a `strview` that receives the remainder after the separator;
 *                  may be NULL if the remainder is not needed
 * @param nocase    If true, the separator search ignores ASCII letter case
 * @return The part before the separator, or the entire view if the separator is not found
 */
LIB_STV_FN strview stv_split(strview stv, strview sep, strview* remaining, bool nocase);

/**
 * @brief Split the view at the first line break, supporting CR, LF, and CRLF
 *
 * Scans the view for the first occurrence of a carriage return (`\r`) or newline (`\n`).
 * Handles Windows‑style CRLF as a single line break. The returned line does not include
 * the line break characters. If no line break is found, the entire view is returned and
 * `*remaining` is set to an empty view.
 *
 * When `stv` is empty, an empty view is returned and `*remaining` is set to the empty view
 * (if `remaining` is not NULL).
 *
 * @param stv       The string view to split
 * @param remaining Optional pointer to a `strview` that receives the remainder after the line break;
 *                  may be NULL
 * @return The current line (without trailing line break), or the entire view if no break is found
 */
LIB_STV_FN strview stv_splitLines(strview stv, strview* remaining);

/**
 * @brief Split the view at the first word (sequence of non‑whitespace characters)
 *
 * Skips leading whitespace (as defined by `stv_whitespace`), then extracts the next contiguous
 * sequence of non‑whitespace characters as the current word. The remainder (starting from
 * the first character after the word) is stored in `*remaining` (if not NULL). If the view
 * contains no word (i.e., is empty or consists entirely of whitespace), an empty view is
 * returned and `*remaining` is set to an empty view.
 *
 * When `stv` is empty, an empty view is returned and `*remaining` is set to the empty view
 * (if `remaining` is not NULL).
 *
 * @param stv       The string view to split
 * @param remaining Optional pointer to a `strview` that receives the remainder after the word;
 *                  may be NULL
 * @return The current word (non‑whitespace sequence), or an empty view if no word is found
 */
LIB_STV_FN strview stv_splitWords(strview stv, strview* remaining);

/**
 * @brief Return the portion of the view before the first occurrence of a delimiter
 *
 * Locates the first occurrence of `delim` in `stv`. If found, returns the substring from the beginning up to,
 * but not including, the delimiter. If the delimiter is not found, the entire `stv` is returned.
 *
 * If `delim` is empty, the result is always an empty view. If `stv` is empty, an empty view is returned.
 *
 * @param stv   The string view to examine
 * @param delim The delimiter view (may be empty)
 * @return The part before the delimiter; empty view if `delim` is empty
 */
LIB_STV_FN strview stv_beforeFirstDelim(strview stv, strview delim);

/**
 * @brief Return the portion of the view before the last occurrence of a delimiter
 *
 * Locates the last occurrence of `delim` in `stv`. If found, returns the substring from the beginning up to,
 * but not including, the delimiter. If the delimiter is not found, the entire `stv` is returned.
 *
 * If `delim` is empty, the result is always an empty view. If `stv` is empty, an empty view is returned.
 *
 * @param stv   The string view to examine
 * @param delim The delimiter view (may be empty)
 * @return The part before the last delimiter; entire view if delimiter not found; empty view if `delim` is empty
 */
LIB_STV_FN strview stv_beforeLastDelim(strview stv, strview delim);

/**
 * @brief Return the portion of the view after the first occurrence of a delimiter
 *
 * Locates the first occurrence of `delim` in `stv`. If found, returns the substring that follows the
 * delimiter to the end of the view. If the delimiter is not found, an empty view is returned.
 *
 * If `delim` is empty, the entire `stv` is returned unchanged (the function behaves as if no split occurred).
 * If `stv` is empty, an empty view is returned.
 *
 * @param stv   The string view to examine
 * @param delim The delimiter view (may be empty)
 * @return The part after the delimiter; entire view if `delim` is empty, empty view if delimiter not found
 */
LIB_STV_FN strview stv_afterFirstDelim(strview stv, strview delim);

/**
 * @brief Return the portion of the view after the last occurrence of a delimiter
 *
 * Locates the last occurrence of `delim` in `stv`. If found, returns the substring that follows the
 * delimiter to the end of the view. If the delimiter is not found, an empty view is returned.
 *
 * If `delim` is empty, the entire `stv` is returned unchanged (the function behaves as if no split occurred).
 * If `stv` is empty, an empty view is returned.
 *
 * @param stv   The string view to examine
 * @param delim The delimiter view (may be empty)
 * @return The part after the last delimiter; entire view if `delim` is empty, empty view if delimiter not found
 */
LIB_STV_FN strview stv_afterLastDelim(strview stv, strview delim);

#ifdef LIB_STV_GENERIC
    /**
     * @brief Generic trim helper: selects stv_trimIf() or stv_trimChs()
     *
     * The second argument is inspected with `_Generic`:
     * - `stv_charClassFn` → `stv_trimIf(stv, handle)` (trim characters satisfying the classification function)
     * - `strview`         → `stv_trimChs(stv, charset)` (trim characters belonging to the charset)
     *
     * Any other type is a compile-time error.
     *
     * Trims both ends of the view.
     *
     * @param stv    Source string view
     * @param target Character classification function or charset view
     */
    #define stv_trim(stv, target) _Generic((target), stv_charClassFn: stv_trimIf, strview: stv_trimChs)((stv), (target))
#endif

/**
 * @brief Remove any characters belonging to a given charset from the beginning and end of the view
 *
 * @param stv     Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with leading and trailing characters removed; if `charset` is empty, the original view is
 *         returned unchanged
 */
LIB_STV_FN strview stv_trimChs(strview stv, strview charset);

/**
 * @brief Remove any characters satisfying a classification function from the beginning and end of the view
 *
 * @param stv    Source string view
 * @param handle Character classification function (e.g., `isspace`). If NULL, no trimming is performed.
 * @return A new view with leading and trailing matching characters removed
 */
LIB_STV_FN strview stv_trimIf(strview stv, stv_charClassFn handle);

#ifdef LIB_STV_GENERIC
    /**
     * @brief Generic trim-start helper: selects stv_trimStartIf() or stv_trimStartChs()
     *
     * The second argument is inspected with `_Generic`:
     * - `stv_charClassFn` → `stv_trimStartIf(stv, handle)`
     * - `strview`         → `stv_trimStartChs(stv, charset)`
     *
     * Any other type is a compile-time error.
     *
     * Only the beginning of the view is trimmed.
     *
     * @param stv    Source string view
     * @param target Character classification function or charset view
     */
    #define stv_trimStart(stv, target)                                                                                 \
        _Generic((target), stv_charClassFn: stv_trimStartIf, strview: stv_trimStartChs)((stv), (target))
#endif // LIB_STV_GENERIC

/**
 * @brief Remove any characters belonging to a given charset from the beginning of the view
 *
 * @param stv     Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with leading characters removed; if `charset` is empty, the original view is returned unchanged
 */
LIB_STV_FN strview stv_trimStartChs(strview stv, strview charset);

/**
 * @brief Remove any characters satisfying a classification function from the beginning of the view
 *
 * @param stv    Source string view
 * @param handle Character classification function (e.g., `isspace`). If NULL, no trimming is performed.
 * @return A new view with leading matching characters removed
 */
LIB_STV_FN strview stv_trimStartIf(strview stv, stv_charClassFn handle);

#ifdef LIB_STV_GENERIC
    /**
     * @brief Generic trim-end helper: selects stv_trimEndIf() or stv_trimEndChs()
     *
     * The second argument is inspected with `_Generic`:
     * - `stv_charClassFn` → `stv_trimEndIf(stv, handle)`
     * - `strview`         → `stv_trimEndChs(stv, charset)`
     *
     * Any other type is a compile-time error.
     *
     * Only the end of the view is trimmed.
     *
     * @param stv    Source string view
     * @param target Character classification function or charset view
     */
    #define stv_trimEnd(stv, target)                                                                                   \
        _Generic((target), stv_charClassFn: stv_trimEndIf, strview: stv_trimEndChs)((stv), (target))
#endif

/**
 * @brief Remove any characters belonging to a given charset from the end of the view
 *
 * @param stv     Source string view
 * @param charset Character set view containing characters to trim
 * @return A new view with trailing characters removed; if `charset` is empty, the original view is returned unchanged
 */
LIB_STV_FN strview stv_trimEndChs(strview stv, strview charset);

/**
 * @brief Remove any characters satisfying a classification function from the end of the view
 *
 * @param stv    Source string view
 * @param handle Character classification function (e.g., `isspace`). If NULL, no trimming is performed.
 * @return A new view with trailing matching characters removed
 */
LIB_STV_FN strview stv_trimEndIf(strview stv, stv_charClassFn handle);

/**
 * @brief Search for a substring within a text view (automatic algorithm selection)
 *
 * Uses naive search when the pattern length is ≤ 4, otherwise uses Sunday search.
 *
 * @param stv_text The text view to search in
 * @param stv_pat  The pattern view to search for
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return Start position of the first match; returns 0 if `stv_pat` is empty, or `stv_npos` if not found
 */
LIB_STV_FN size_t stv_search(strview stv_text, strview stv_pat, bool nocase);

/**
 * @brief Naive string search (suitable for short patterns)
 *
 * @param stv_text The text view to search in
 * @param stv_pat  The pattern view to search for
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return Start position of the first match; returns 0 if `stv_pat` is empty, or `stv_npos` if not found
 */
LIB_STV_FN size_t stv_naiveSearch(strview stv_text, strview stv_pat, bool nocase);

/**
 * @brief Sunday string search (suitable for longer patterns)
 *
 * @param stv_text The text view to search in
 * @param stv_pat  The pattern view to search for
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return Start position of the first match; returns 0 if `stv_pat` is empty, or `stv_npos` if not found
 */
LIB_STV_FN size_t stv_sundaySearch(strview stv_text, strview stv_pat, bool nocase);

/**
 * @brief Search for the last occurrence of a substring within a text view (automatic algorithm selection)
 *
 * Uses naive search when the pattern length is ≤ 4, otherwise uses Sunday search.
 * The search proceeds from right to left.
 *
 * @param stv_text The text view to search in
 * @param stv_pat  The pattern view to search for
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return Start position of the last match; returns `stv_text.len` if `stv_pat` is empty, or `stv_npos` if not found
 */
LIB_STV_FN size_t stv_searchRev(strview stv_text, strview stv_pat, bool nocase);

/**
 * @brief Naive last‑occurrence string search (suitable for short patterns)
 *
 * Searches from the end of the text towards the beginning.
 *
 * @param stv_text The text view to search in
 * @param stv_pat  The pattern view to search for
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return Start position of the last match; returns `stv_text.len` if `stv_pat` is empty, or `stv_npos` if not found
 */
LIB_STV_FN size_t stv_naiveSearchRev(strview stv_text, strview stv_pat, bool nocase);

/**
 * @brief Sunday last‑occurrence string search (suitable for longer patterns)
 *
 * Adapts the Sunday algorithm to search from right to left.
 *
 * @param stv_text The text view to search in
 * @param stv_pat  The pattern view to search for
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return Start position of the last match; returns `stv_text.len` if `stv_pat` is empty, or `stv_npos` if not found
 */
LIB_STV_FN size_t stv_sundaySearchRev(strview stv_text, strview stv_pat, bool nocase);

#ifdef LIB_STV_GENERIC
    /**
     * @brief Generic first-index search: dispatches on the type of `target`
     *
     * The second argument is inspected with `_Generic`:
     * - `int` / `char`      → `stv_firstCh(stv, ch, invert)`
     * - `stv_charClassFn`   → `stv_firstIf(stv, handle, invert)`
     * - `strview`           → `stv_firstChs(stv, charset, invert)`
     *
     * Any other type is a compile-time error.
     *
     * @param stv    The string view to search
     * @param target Character, classification function, or charset view
     * @param invert If true, search for the first character that does *not* match
     * @return Index of the first match, or `stv_npos` if not found
     */
    #define stv_firstIndex(stv, target, invert)                                                                        \
        _Generic((target), int: stv_firstCh, char: stv_firstCh, stv_charClassFn: stv_firstIf, strview: stv_firstChs)(  \
            (stv), (target), (invert))
#endif

/**
 * @brief Find the first occurrence of a specific character
 *
 * @param stv    The string view to search
 * @param ch     The character to locate
 * @param invert If true, search for the first character NOT equal to `ch`
 * @return Index of the first match, or `stv_npos` if not found or the view is empty
 */
LIB_STV_FN size_t stv_firstCh(strview stv, const char ch, bool invert);

/**
 * @brief Find the first occurrence of any character from a charset
 *
 * @param stv     The string view to search
 * @param charset Character set view. An empty charset matches no characters when
 *                `invert` is false, and matches all characters when `invert` is true.
 * @param invert  If true, search for the first character NOT in the charset
 * @return Index of the first match, or `stv_npos` if not found or the view is empty
 */
LIB_STV_FN size_t stv_firstChs(strview stv, strview charset, bool invert);

/**
 * @brief Find the first character that satisfies a classification function
 *
 * @param stv    The string view to search
 * @param handle Character classification function (e.g., `isdigit`). Must not be NULL.
 * @param invert If true, search for the first character that does NOT satisfy `handle`
 * @return Index of the first match, or `stv_npos` if not found or the view is empty
 */
LIB_STV_FN size_t stv_firstIf(strview stv, stv_charClassFn handle, bool invert);

#ifdef LIB_STV_GENERIC
    /**
     * @brief Generic last-index search: dispatches on the type of `target`
     *
     * The second argument is inspected with `_Generic`:
     * - `int` / `char`      → `stv_lastCh(stv, ch, invert)`
     * - `stv_charClassFn`   → `stv_lastIf(stv, handle, invert)`
     * - `strview`           → `stv_lastChs(stv, charset, invert)`
     *
     * Any other type is a compile-time error.
     *
     * Searches from the end of the view towards the beginning.
     *
     * @param stv    The string view to search
     * @param target Character, classification function, or charset view
     * @param invert If true, search for the last character that does *not* match
     * @return Index of the last match, or `stv_npos` if not found
     */
    #define stv_lastIndex(stv, target, invert)                                                                         \
        _Generic((target), int: stv_lastCh, char: stv_lastCh, stv_charClassFn: stv_lastIf, strview: stv_lastChs)(      \
            (stv), (target), (invert))
#endif

/**
 * @brief Find the last occurrence of a specific character
 *
 * @param stv    The string view to search
 * @param ch     The character to locate
 * @param invert If true, search for the last character NOT equal to `ch`
 * @return Index of the last match, or `stv_npos` if not found or the view is empty
 */
LIB_STV_FN size_t stv_lastCh(strview stv, const char ch, bool invert);

/**
 * @brief Find the last occurrence of any character from a charset
 *
 * @param stv     The string view to search
 * @param charset Character set view. An empty charset matches no characters when
 *                `invert` is false, and matches all characters when `invert` is true.
 * @param invert  If true, search for the last character NOT in the charset
 * @return Index of the last match, or `stv_npos` if not found or the view is empty
 */
LIB_STV_FN size_t stv_lastChs(strview stv, strview charset, bool invert);

/**
 * @brief Find the last character that satisfies a classification function
 *
 * @param stv    The string view to search
 * @param handle Character classification function (e.g., `isdigit`). Must not be NULL.
 * @param invert If true, search for the last character that does NOT satisfy `handle`
 * @return Index of the last match, or `stv_npos` if not found or the view is empty
 */
LIB_STV_FN size_t stv_lastIf(strview stv, stv_charClassFn handle, bool invert);

/**
 * @brief Find the first position where two views differ (from left to right)
 *
 * Compares byte from the beginning. If the views are identical
 * (same data and length, or same content and length), returns `stv_npos`.
 * If one view is a prefix of the other, returns the length of the shorter view.
 * Otherwise returns the index of the first differing byte.
 *
 * @param stv_left  Left view
 * @param stv_right Right view
 * @param nocase    If true, ASCII letter case is ignored during comparison
 * @return Index of the first difference, or `stv_npos` if the views are identical
 */
LIB_STV_FN size_t stv_firstDiff(strview stv_left, strview stv_right, bool nocase);

/**
 * @brief Find the last position where two views differ (from right to left)
 *
 * Compares bytes from the end towards the beginning. If the views are identical
 * (same data and length, or same content and length), returns `stv_npos`.
 * If one view is a suffix of the other, the index of the last byte in the
 * longer view that is not part of the shorter view is returned.
 * Otherwise it returns the index of the last differing byte.
 *
 * @param stv_left  Left view
 * @param stv_right Right view
 * @param nocase    If true, ASCII letter case is ignored during comparison
 * @return Index of the last difference, or `stv_npos` if the views are identical
 */
LIB_STV_FN size_t stv_lastDiff(strview stv_left, strview stv_right, bool nocase);

/**
 * @brief Return the length of the view in bytes
 *
 * Returns 0 if `stv.data` is NULL; otherwise returns `stv.len` unchanged.
 * The NULL‑check mirrors the empty‑view semantics used throughout the library.
 *
 * @param stv String view
 * @return Length of the view in bytes, or 0 for a null view
 */
LIB_STV_FN size_t stv_length(strview stv);

#ifdef LIB_STV_GENERIC
    /**
     * @brief Generic character counter: dispatches on the type of `target`
     *
     * The second argument is inspected with `_Generic`:
     * - `int` / `char`      → `stv_countCh(stv, ch)`
     * - `stv_charClassFn`   → `stv_countIf(stv, handle)`
     * - `strview`           → `stv_countChs(stv, charset)`
     *
     * Any other type is a compile-time error.
     *
     * @param stv    String view to examine
     * @param target Character, classification function, or charset view
     * @return Number of matching characters, or 0 if the view is empty (or, for the
     *         function/charset variants, if `handle`/`charset` is empty)
     */
    #define stv_count(stv, target)                                                                                     \
        _Generic((target), int: stv_countCh, char: stv_countCh, stv_charClassFn: stv_countIf, strview: stv_countChs)(  \
            (stv), (target))
#endif

/**
 * @brief Count the occurrences of a specific character
 *
 * @param stv String view to examine
 * @param ch  Character to count
 * @return Number of occurrences of `ch`, or `0` if the view is empty
 */
LIB_STV_FN size_t stv_countCh(strview stv, char ch);

/**
 * @brief Count how many characters in a view appear in a given charset
 *
 * If `charset` is empty the result is 0.
 *
 * @param stv     String view to examine
 * @param charset Character set view containing the characters to count
 * @return Number of characters in `stv` that also appear in `charset`,
 *         or 0 if `stv` is empty or `charset` is empty
 */
LIB_STV_FN size_t stv_countChs(strview stv, strview charset);

/**
 * @brief Count the number of characters that satisfy a classification function
 *
 * @param stv    String view to examine
 * @param handle Character classification function (e.g., `isdigit`). If NULL, the function returns `0`.
 * @return Number of matching characters, or `0` if the view is empty or `handle` is NULL
 */
LIB_STV_FN size_t stv_countIf(strview stv, stv_charClassFn handle);

/**
 * @brief Count non‑overlapping occurrences of a substring in the view
 *
 * Searches for `sub` repeatedly in `stv` from left to right, moving past each match.
 * The matches are non‑overlapping: after a match, the search continues from the character
 * following the matched substring.
 *
 * @param stv The string view to search in
 * @param sub The substring to count (may be empty)
 * @return Number of non‑overlapping occurrences; returns `stv.len` if `sub` is empty,
 *         or `0` if `stv` is empty
 */
LIB_STV_FN size_t stv_countSubstr(strview stv, strview sub);

#ifdef LIB_STV_GENERIC
    /**
     * @brief Generic “every character matches” test: dispatches on the type of `target`
     *
     * The second argument is inspected with `_Generic`:
     * - `int` / `char`      → `stv_everyCh(stv, ch)`
     * - `stv_charClassFn`   → `stv_everyIf(stv, handle)`
     * - `strview`           → `stv_everyChs(stv, charset)`
     *
     * Any other type is a compile-time error.
     *
     * An empty view always yields false.
     *
     * @param stv    String view to examine
     * @param target Character, classification function, or charset view
     * @return true if the view is non-empty and every character matches; false otherwise
     */
    #define stv_every(stv, target)                                                                                     \
        _Generic((target), int: stv_everyCh, char: stv_everyCh, stv_charClassFn: stv_everyIf, strview: stv_everyChs)(  \
            (stv), (target))
#endif

/**
 * @brief Check if every character in the view equals a given character
 *
 * An empty view always returns false.
 *
 * @param stv String view to examine
 * @param ch  Character to compare against
 * @return true if the view is non‑empty and all characters equal `ch`; false otherwise
 */
LIB_STV_FN bool stv_everyCh(strview stv, char ch);

/**
 * @brief Check whether every character in the view belongs to a given charset
 *
 * An empty view always returns false. An empty `charset` also yields false
 * (there is no character to match against).
 *
 * @param stv     String view to examine
 * @param charset Character set view
 * @return true if the view is non‑empty and every character belongs to `charset`;
 *         false otherwise
 */
LIB_STV_FN bool stv_everyChs(strview stv, strview charset);

/**
 * @brief Check if every character in the view satisfies a classification function
 *
 * An empty view or a NULL `handle` will cause the function to return false.
 *
 * @param stv    String view to examine
 * @param handle Character classification function (must not be NULL for a meaningful result)
 * @return true if the view and `handle` are non‑empty and all characters match the class; false otherwise
 */
LIB_STV_FN bool stv_everyIf(strview stv, stv_charClassFn handle);

#ifdef LIB_STV_GENERIC
    /**
     * @brief Generic “at least one character matches” test: dispatches on the type of `target`
     *
     * The second argument is inspected with `_Generic`:
     * - `int` / `char`      → `stv_someCh(stv, ch)`
     * - `stv_charClassFn`   → `stv_someIf(stv, handle)`
     * - `strview`           → `stv_someChs(stv, charset)`
     *
     * Any other type is a compile-time error.
     *
     * @param stv    String view to examine
     * @param target Character, classification function, or charset view
     * @return true if the view is non-empty and at least one character matches;
     *         false otherwise
     */
    #define stv_some(stv, target)                                                                                      \
        _Generic((target), int: stv_someCh, char: stv_someCh, stv_charClassFn: stv_someIf, strview: stv_someChs)(      \
            (stv), (target))
#endif

/**
 * @brief Check if at least one occurrence of a given character exists in the view
 *
 * @param stv String view to examine
 * @param ch  Character to search for
 * @return true if the view is non‑empty and contains `ch`; false otherwise
 */
LIB_STV_FN bool stv_someCh(strview stv, char ch);

/**
 * @brief Check whether at least one character in the view belongs to a given charset
 *
 * @param stv     String view to examine
 * @param charset Character set view
 * @return true if the view is non‑empty and at least one character belongs to
 *         `charset`; false otherwise
 */
LIB_STV_FN bool stv_someChs(strview stv, strview charset);

/**
 * @brief Check if at least one character in the view satisfies a classification function
 *
 * @param stv    String view to examine
 * @param handle Character classification function (must not be NULL)
 * @return true if the view and `handle` are non‑empty and at least one character matches the class; false otherwise
 */
LIB_STV_FN bool stv_someIf(strview stv, stv_charClassFn handle);

/**
 * @brief Check if a view starts with a given prefix
 *
 * @param stv_text The text view to examine
 * @param stv_pat  The prefix pattern (an empty pattern always returns true)
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return true if `stv_text` starts with `stv_pat`, false otherwise
 */
LIB_STV_FN bool stv_startsWith(strview stv_text, strview stv_pat, bool nocase);

/**
 * @brief Check if a view ends with a given suffix
 *
 * @param stv_text The text view to examine
 * @param stv_pat  The suffix pattern (an empty pattern always returns true)
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return true if `stv_text` ends with `stv_pat`, false otherwise
 */
LIB_STV_FN bool stv_endsWith(strview stv_text, strview stv_pat, bool nocase);

/**
 * @brief Check if a view contains a given substring
 *
 * @param stv_text The text view
 * @param stv_sub  The pattern to search for (an empty pattern is considered as contained)
 * @param nocase   If true, character comparisons ignore ASCII letter case
 * @return true if `stv_sub` appears in `stv_text`, false otherwise
 */
LIB_STV_FN bool stv_contains(strview stv_text, strview stv_sub, bool nocase);

/**
 * @brief Check if two views have identical content (byte‑by‑byte comparison)
 *
 * @param stv_left  Left view
 * @param stv_right Right view
 * @return true if contents are equal, false otherwise
 */
LIB_STV_FN bool stv_equal(strview stv_left, strview stv_right);

/**
 * @brief Check if two views have identical content, optionally ignoring ASCII case
 *
 * @param stv_left  Left view
 * @param stv_right Right view
 * @param nocase    If true, ASCII letter case is ignored during comparison
 * @return true if contents are equal, false otherwise
 */
LIB_STV_FN bool stv_equalEx(strview stv_left, strview stv_right, bool nocase);

/**
 * @brief Check if two views reference exactly the same underlying data (same pointer and length)
 *
 * @param stv_left  Left view
 * @param stv_right Right view
 * @return true if both the data pointer and the length are equal, false otherwise
 */
LIB_STV_FN bool stv_same(strview stv_left, strview stv_right);

/**
 * @brief Check if a view is empty
 *
 * @param stv String view
 * @return true if `data` is NULL or `len` is 0, false otherwise
 */
LIB_STV_FN bool stv_empty(strview stv);

/**
 * @brief Compare two string views lexicographically
 *
 * @param stv_left  Left view
 * @param stv_right Right view
 * @return Negative if `left < right`, 0 if equal, positive if `left > right`
 */
LIB_STV_FN int stv_compare(strview stv_left, strview stv_right);

/**
 * @brief Compare two string views lexicographically, optionally ignoring ASCII case
 *
 * Compares the views byte by byte in the same way as `stv_compare()`, but when
 * `nocase` is true, ASCII letters are folded to lowercase before comparison
 * (e.g., `'A'` and `'a'` are considered equal).
 *
 * @param stv_left  Left view
 * @param stv_right Right view
 * @param nocase    If true, ASCII letter case is ignored during comparison
 * @return Negative if `stv_left < stv_right`, 0 if equal, positive if `stv_left > stv_right`
 */
LIB_STV_FN int stv_compareEx(strview stv_left, strview stv_right, bool nocase);

/**
 * @brief Get the first character of the view
 *
 * @param stv String view
 * @return The first character if the view is non‑empty, otherwise `'\0'`
 */
LIB_STV_FN char stv_front(strview stv);

/**
 * @brief Get the last character of the view
 *
 * @param stv String view
 * @return The last character if the view is non‑empty, otherwise `'\0'`
 */
LIB_STV_FN char stv_back(strview stv);

/**
 * @brief Access a character at the specified index
 *
 * Returns the character at position `idx`. If the view is empty or the index is out of bounds,
 * the null character `'\0'` is returned.
 *
 * @param stv String view
 * @param idx Zero‑based index
 * @return The character at `idx`, or `'\0'` if out of range
 */
LIB_STV_FN char stv_at(strview stv, size_t idx);

/**
 * @brief Iterate over each character of a string view
 *
 * Calls the provided callback for each character in the view, passing the character,
 * its zero‑based index, and the user‑provided `ctx` pointer.
 *
 * If the view is empty or `callback` is NULL, the callback is never invoked.
 *
 * @param stv      The string view to iterate over
 * @param callback Callback function of type `stv_forEachFn`
 * @param ctx      User‑provided context pointer passed through to the callback
 */
LIB_STV_FN void stv_forEach(strview stv, stv_forEachFn callback, void* ctx);

/**
 * @brief Iterate over each character of a string view in reverse order
 *
 * Calls the provided callback for each character from the last to the first,
 * passing the character, its zero‑based index (descending), and the user‑provided
 * `ctx` pointer.
 *
 * If the view is empty or `callback` is NULL, the callback is never invoked.
 *
 * @param stv      The string view to iterate over
 * @param callback Callback function of type `stv_forEachFn`
 * @param ctx      User‑provided context pointer passed through to the callback
 */
LIB_STV_FN void stv_forEachRev(strview stv, stv_forEachFn callback, void* ctx);

/**
 * @brief Swap the contents of two string views
 *
 * If either pointer is NULL, the function does nothing.
 *
 * @param stv_left  Pointer to the first view
 * @param stv_right Pointer to the second view
 */
LIB_STV_FN void stv_swap(strview* stv_left, strview* stv_right);

/**
 * @brief Compute a hash value for the string view
 *
 * Uses `stv_hash_FNV1a` as the default hashing algorithm.
 *
 * @param stv The string view to hash
 * @return The computed hash value (0 if the view is empty)
 */
LIB_STV_FN size_t stv_hash(strview stv);

/**
 * @brief Compute an FNV‑1a hash for the string view
 *
 * Uses the FNV‑1a hash algorithm, automatically selecting the 16‑bit, 32‑bit, or 64‑bit variant
 * based on `SIZE_MAX`:
 * - 64‑bit when `SIZE_MAX == UINT64_MAX`
 * - 32‑bit when `SIZE_MAX == UINT32_MAX`
 * - 16‑bit when `SIZE_MAX == UINT16_MAX`
 *
 * If `SIZE_MAX` does not match any of the known widths, or if the view is empty, returns 0.
 *
 * @param stv The string view to hash
 * @return The computed FNV‑1a hash (0 if the view is empty or the platform is unsupported)
 */
LIB_STV_FN size_t stv_hash_FNV1a(strview stv);

/**
 * @brief Write a string view into a character buffer as a null‑terminated C string
 *
 * Copies at most `size - 1` bytes from `stv` into `mem`, appends a null terminator,
 * and optionally applies case conversion and byte reversal controlled by `opts`.
 *
 * Overflow behaviour:
 * - If `stv_Truncate` is set, the output is silently truncated to fit the buffer.
 * - Otherwise the function returns `NULL` without writing anything.
 *
 * Transformation options (see `stv_cstrOptions` for details):
 * - `stv_ToUpper` / `stv_ToLower`: convert ASCII letters; when both are set the
 *   result is swap‑case.
 * - `stv_Reverse`: reverse the byte order of the output.
 * - `stv_ViewReverse`: read the source view from its last byte to its first.
 * - `stv_JoinReverse` has no effect here.
 *
 * @param stv  Source string view
 * @param mem  Destination buffer
 * @param size Size of `mem` in bytes (must be at least 1)
 * @param opts Bitwise combination of `stv_cstrOptions`
 * @return `mem` on success; `NULL` if `mem` is NULL, `size` is 0, or the output
 *         would not fit and `stv_Truncate` is not set
 */
LIB_STV_FN char* stv_cstr(strview stv, char* mem, size_t size, stv_cstrOptions opts);

/**
 * @brief Join an array of string views with a separator into a buffer
 *
 * Writes the concatenation of all views in `stv_arr`, interleaved with `sep`,
 * into the output buffer `mem`. Each element may optionally be transformed using `opts`.
 *
 * If the array is empty, an empty string is written to `mem`, provided the buffer has at least one byte.
 *
 * @param stv_arr Array of `strview` to join (may be NULL only if `arr_len == 0`)
 * @param arr_len Number of elements in the array
 * @param mem     Destination buffer
 * @param size    Size of the destination buffer in bytes
 * @param sep     Separator to insert between elements (may be empty)
 * @param opts    Bitwise combination of `stv_cstrOptions` to apply to each element
 * @return `mem` on success; `NULL` if `mem` is NULL, `size` is 0, or the buffer is
 *         too small and `stv_Truncate` is not set. When `stv_Truncate` is set, the
 *         output is truncated to fit and `mem` is returned.
 */
LIB_STV_FN char* stv_join(strview stv_arr[], size_t arr_len, char* mem, size_t size, strview sep, stv_cstrOptions opts);

/**
 * @brief Convert a character to its numeric digit value (0‑35)
 *
 * @param ch Input character
 * @return Digit value 0‑35 for `'0'`‑`'9'`, `'A'`‑`'Z'`, `'a'`‑`'z'`; -1 if invalid
 */
LIB_STV_FN int stv_ch2digit(char ch);

/**
 * @brief Detect numeric base from the prefix of a string view
 *
 * Supports `0b`/`0B` for binary, `0o`/`0O` for octal, `0d`/`0D` for decimal, `0x`/`0X` for hexadecimal.
 * A lone leading `'0'` without a recognised prefix is treated as decimal and is consumed as part of the number.
 * If no prefix is found, the base defaults to 10 and the view is unchanged.
 *
 * @param stv       Input view to examine
 * @param remaining Optional output pointer to receive the portion after the prefix (if any).
 *                  If NULL, the remainder is discarded.
 * @return The detected base (2, 8, 10, or 16). Returns 0 if the view is empty.
 */
LIB_STV_FN int stv_parseIntBase(strview stv, strview* remaining);

/**
 * @brief Parse a signed integer from a string view
 *
 * Skips leading whitespace, handles an optional `'+'`/`'-'` sign, and then parses digits
 * in the specified base. If `base` is 0, it is auto‑detected via `stv_parseIntBase()`.
 * Overflows are clamped to `INTMAX_MAX` / `INTMAX_MIN` and the remaining view is updated.
 *
 * @param stv       Input view
 * @param base      Numeric base (2‑36), or 0 for auto‑detection
 * @param remaining Optional output pointer to receive the portion after the parsed number.
 *                  If NULL, the remainder is discarded.
 * @return The parsed value; 0 if no digits are found or `base` is invalid; clamped on overflow.
 */
LIB_STV_FN intmax_t stv_parseInum(strview stv, int base, strview* remaining);

/**
 * @brief Parse an unsigned integer from a string view
 *
 * Similar to `stv_parseInum()` but returns `uintmax_t`. Negative values are converted
 * via modulo arithmetic (e.g., `"-40"` yields `UINTMAX_MAX - 39`). Overflows are clamped
 * to `UINTMAX_MAX`.
 *
 * @param stv       Input view
 * @param base      Numeric base (2‑36), or 0 for auto‑detection
 * @param remaining Optional output pointer for the remainder
 * @return The parsed value; 0 if no digits are found or `base` is invalid; clamped on overflow.
 */
LIB_STV_FN uintmax_t stv_parseUnum(strview stv, int base, strview* remaining);

/**
 * @brief Convenience macro: construct a `strview` from a given data pointer and length
 *
 * @param data_v Pointer to the data
 * @param len_v  Length
 */
#ifdef __cplusplus
    #define stv_makestv(data_v, len_v) (strview{(data_v), (len_v)})
#else
    #define stv_makestv(data_v, len_v) ((strview){.data = (data_v), .len = (len_v)})
#endif

/** @brief Convenience macro: construct a `strview` from a string literal */
#define stv_literal(str) stv_makestv((str), sizeof(str) - 1)

/** @brief Predefined empty string view (`data = nullptr`, `len = 0`) */
#define stv_nullstv stv_makestv(nullptr, 0)

/** @brief Predefined whitespace character set view (space, carriage return, newline, tab, vertical tab, form feed) */
#define stv_whitespace stv_literal(" \r\n\t\v\f")

/** @brief Sentinel value indicating "not found" */
#define stv_npos ((size_t)-1)

/** @brief Represents the start position (index 0) */
#define stv_begin (0)

/** @brief Represents the end position (equivalent to `stv_npos`) */
#define stv_end (stv_npos)

/**
 * @brief Convenience macro to create a `strview` array and its length
 *
 * Expands to a compound literal array of `strview` and its element count.
 * Designed for use with functions that take a `strview` array and a length,
 * such as `stv_join()`.
 *
 * Usage:
 * ```
 * strview sv1 = stv_literal("a"), sv2 = stv_literal("b");
 * stv_join(stv_LIST(sv1, sv2), mem, size, sep, opts);
 * ```
 *
 * @param ... Variadic list of `strview` expressions
 */
#ifndef __cplusplus
    #define stv_LIST(...) ((strview[]){__VA_ARGS__}), (sizeof((strview[]){__VA_ARGS__}) / sizeof(strview))
#endif

/**
 * @brief Helper macro for `printf`‑style formatting of a string view
 *
 * If the view length exceeds `INT_MAX`, the length parameter is capped at `INT_MAX`.
 *
 * @note The `stv` argument is evaluated multiple times during macro expansion
 *       (once per `stv_empty` / `.len` / `.data` use). Pass a simple variable or
 *       a `strview` lvalue, never an expression with side effects.
 *
 * Example usage:
 * ```
 * printf("data: " stv_PFFMT "\n", stv_PFARG(myview));
 * ```
 *
 * @param stv The string view to output
 */
#define stv_PFARG(stv)                                                                                                 \
    (int)(stv_empty(stv) ? 0 : (stv).len > INT_MAX ? INT_MAX : (stv).len), (stv_empty(stv) ? "" : (stv).data)

/**
 * @brief `printf` format string helper macro, used together with `stv_PFARG`
 *
 * @see stv_PFARG
 */
#define stv_PFFMT "%.*s"

/******************************* IMPLEMENTATION *******************************/

#ifdef LIB_STV_IMPL

    #define stv_impl_length(stv)      ((stv).data == nullptr ? 0 : (stv).len)
    #define stv_impl_empty(stv)       ((stv).data == nullptr || (stv).len == 0)
    #define stv_impl_same(stv1, stv2) (((stv1).data == (stv2).data) && ((stv1).len == (stv2).len))
    #define stv_impl_min(val1, val2)  ((val1) > (val2) ? (val2) : (val1))
    #define stv_impl_max(val1, val2)  ((val1) < (val2) ? (val2) : (val1))

    #define stv_impl_init_charset(name)                                                                                \
        unsigned char name[(UCHAR_MAX + CHAR_BIT) / CHAR_BIT] = {0};                                                   \
        for (size_t idx = 0; idx < charset.len; idx++) {                                                               \
            const unsigned char ch = charset.data[idx];                                                                \
            name[ch / CHAR_BIT] |= (1u << ch % CHAR_BIT);                                                              \
        }
    #define stv_impl_inchs(ch, chs) ((chs[(ch) / CHAR_BIT] & (1u << (ch) % CHAR_BIT)) != 0)

    #define stv_impl_nocase(ch)                                                                                        \
        do {                                                                                                           \
            if (nocase && (ch) >= 'A' && (ch) <= 'Z') {                                                                \
                (ch) |= 32;                                                                                            \
            }                                                                                                          \
        } while (0)

LIB_STV_FN strview stv_new(const char* c_str) {
    return stv_create(c_str, '\0', stv_npos);
}

LIB_STV_FN strview stv_create(const char* str, unsigned char endchar, size_t maxlen) {
    if (str == nullptr) {
        return stv_nullstv;
    }
    size_t len = 0;
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
    if (len >= stv.len) {
        return stv_nullstv;
    }
    return stv_slice(stv, len, stv_end);
}

LIB_STV_FN strview stv_removeEnd(strview stv, size_t len) {
    if (len >= stv.len) {
        return stv_nullstv;
    }
    return stv_slice(stv, stv_begin, stv.len - len);
}

LIB_STV_FN strview stv_removePrefix(strview stv, strview prefix, bool nocase) {
    if (stv_startsWith(stv, prefix, nocase)) {
        return stv_slice(stv, prefix.len, stv_end);
    }
    return stv;
}

LIB_STV_FN strview stv_removeSuffix(strview stv, strview suffix, bool nocase) {
    if (stv_endsWith(stv, suffix, nocase)) {
        return stv_slice(stv, stv_begin, stv.len - suffix.len);
    }
    return stv;
}

LIB_STV_FN strview stv_split(strview stv, strview sep, strview* remaining, bool nocase) {
    if (stv_impl_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return stv_nullstv;
    }

    const bool   empty_sep = stv_impl_empty(sep);
    const size_t idx       = empty_sep ? 1 : stv_search(stv, sep, nocase);
    const size_t len       = empty_sep ? 0 : sep.len;
    const bool   split_end = (idx == stv_npos);

    if (remaining) {
        *remaining = split_end ? stv_nullstv : stv_slice(stv, idx + len, stv_end);
    }
    return split_end ? stv : stv_slice(stv, stv_begin, idx);
}

LIB_STV_FN strview stv_splitLines(strview stv, strview* remaining) {
    if (stv_impl_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return stv_nullstv;
    }

    const size_t idx = stv_firstChs(stv, stv_literal("\r\n"), false);
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
    if (stv_impl_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return stv_nullstv;
    }

    const size_t word_start = stv_firstChs(stv, stv_whitespace, true);
    if (word_start == stv_npos) {
        if (remaining) {
            *remaining = stv_nullstv;
        }
        return stv_nullstv;
    }

    const strview tail      = stv_slice(stv, word_start, stv_end);
    const size_t  idx       = stv_firstChs(tail, stv_whitespace, false);
    const size_t  word_end  = word_start + ((idx == stv_npos) ? tail.len : idx);
    const bool    split_end = (word_end >= stv.len);

    if (remaining) {
        *remaining = split_end ? stv_nullstv : stv_makestv(stv.data + word_end, stv.len - word_end);
    }
    return stv_slice(stv, word_start, word_end);
}

LIB_STV_FN strview stv_beforeFirstDelim(strview stv, strview delim) {
    if (stv_impl_empty(stv)) {
        return stv;
    }
    if (stv_impl_empty(delim)) {
        return stv_nullstv;
    }
    const size_t pos = stv_search(stv, delim, false);
    return stv_slice(stv, stv_begin, pos);
}

LIB_STV_FN strview stv_beforeLastDelim(strview stv, strview delim) {
    if (stv_impl_empty(stv)) {
        return stv;
    }
    if (stv_impl_empty(delim)) {
        return stv_nullstv;
    }
    const size_t pos = stv_searchRev(stv, delim, false);
    return stv_slice(stv, stv_begin, pos);
}

LIB_STV_FN strview stv_afterFirstDelim(strview stv, strview delim) {
    if (stv_impl_empty(stv) || stv_impl_empty(delim)) {
        return stv;
    }
    const size_t pos = stv_search(stv, delim, false);
    if (pos == stv_npos) {
        return stv_nullstv;
    }
    return stv_slice(stv, pos + delim.len, stv_end);
}

LIB_STV_FN strview stv_afterLastDelim(strview stv, strview delim) {
    if (stv_impl_empty(stv) || stv_impl_empty(delim)) {
        return stv;
    }
    const size_t pos = stv_searchRev(stv, delim, false);
    if (pos == stv_npos) {
        return stv_nullstv;
    }
    return stv_slice(stv, pos + delim.len, stv_end);
}

LIB_STV_FN strview stv_trimChs(strview stv, strview charset) {
    return stv_trimStartChs(stv_trimEndChs(stv, charset), charset);
}

LIB_STV_FN strview stv_trimStartChs(strview stv, strview charset) {
    if (stv_impl_empty(stv) || stv_impl_empty(charset)) {
        return stv;
    }

    stv_impl_init_charset(cbm);

    const char* end_pos = stv.data + stv.len;
    const char* ch_pos  = stv.data;
    size_t      tc      = 0;
    while (ch_pos < end_pos) {
        const unsigned char ch = *ch_pos;
        if (stv_impl_inchs(ch, cbm)) {
            ch_pos++, tc++;
        } else {
            break;
        }
    }
    return stv_makestv(stv.data + tc, stv.len - tc);
}

LIB_STV_FN strview stv_trimEndChs(strview stv, strview charset) {
    if (stv_impl_empty(stv) || stv_impl_empty(charset)) {
        return stv;
    }

    stv_impl_init_charset(cbm);

    const char* start_pos = stv.data;
    const char* ch_pos    = stv.data + stv.len;
    size_t      tc        = 0;
    while (ch_pos > start_pos) {
        const unsigned char ch = *(--ch_pos);
        if (stv_impl_inchs(ch, cbm)) {
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
    if (stv_impl_empty(stv) || handle == nullptr) {
        return stv;
    }
    const char* end_pos = stv.data + stv.len;
    const char* ch_pos  = stv.data;
    size_t      tc      = 0;
    while (ch_pos < end_pos) {
        const unsigned char ch = *ch_pos;
        if (handle(ch)) {
            ch_pos++, tc++;
        } else {
            break;
        }
    }
    return stv_makestv(stv.data + tc, stv.len - tc);
}

LIB_STV_FN strview stv_trimEndIf(strview stv, stv_charClassFn handle) {
    if (stv_impl_empty(stv) || handle == nullptr) {
        return stv;
    }
    const char* start_pos = stv.data;
    const char* ch_pos    = stv.data + stv.len;
    size_t      tc        = 0;
    while (ch_pos > start_pos) {
        const unsigned char ch = *(--ch_pos);
        if (handle(ch)) {
            tc++;
        } else {
            break;
        }
    }
    return stv_makestv(stv.data, stv.len - tc);
}

LIB_STV_FN size_t stv_search(strview stv_text, strview stv_pat, bool nocase) {
    if (stv_pat.len > 4) {
        return stv_sundaySearch(stv_text, stv_pat, nocase);
    } else {
        return stv_naiveSearch(stv_text, stv_pat, nocase);
    }
}

LIB_STV_FN size_t stv_naiveSearch(strview stv_text, strview stv_pat, bool nocase) {
    if (stv_impl_empty(stv_pat)) {
        return 0;
    }
    if (stv_impl_empty(stv_text) || stv_text.len < stv_pat.len) {
        return stv_npos;
    }

    const size_t end_idx = (stv_text.len - stv_pat.len);
    for (size_t idx = 0; idx <= end_idx; idx++) {
        size_t i;
        for (i = 0; i < stv_pat.len; i++) {
            char tch = stv_text.data[idx + i], pch = stv_pat.data[i];
            stv_impl_nocase(tch);
            stv_impl_nocase(pch);
            if (tch != pch) {
                break;
            }
        }
        if (i == stv_pat.len) {
            return idx;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_sundaySearch(strview stv_text, strview stv_pat, bool nocase) {
    if (stv_impl_empty(stv_pat)) {
        return 0;
    }
    if (stv_impl_empty(stv_text) || stv_text.len < stv_pat.len) {
        return stv_npos;
    }

    const size_t txl = stv_text.len, pal = stv_pat.len;
    size_t       shift[UCHAR_MAX + 1];
    for (size_t i = 0; i <= UCHAR_MAX; i++) {
        shift[i] = pal + 1;
    }
    for (size_t i = 0; i < pal; i++) {
        char pch = stv_pat.data[i];
        stv_impl_nocase(pch);
        shift[(unsigned char)pch] = pal - i;
    }

    size_t idx = 0;
    while (idx <= (txl - pal)) {
        size_t i;
        for (i = 0; i < pal; i++) {
            char tch = stv_text.data[idx + i], pch = stv_pat.data[i];
            stv_impl_nocase(tch);
            stv_impl_nocase(pch);
            if (tch != pch) {
                break;
            }
        }
        if (i == pal) {
            return idx;
        }

        if (idx + pal >= txl) {
            return stv_npos;
        }
        unsigned char next_char = stv_text.data[idx + pal];
        stv_impl_nocase(next_char);
        const size_t skip = shift[next_char];
        idx += skip;
    }

    return stv_npos;
}

LIB_STV_FN size_t stv_searchRev(strview stv_text, strview stv_pat, bool nocase) {
    if (stv_pat.len > 4) {
        return stv_sundaySearchRev(stv_text, stv_pat, nocase);
    } else {
        return stv_naiveSearchRev(stv_text, stv_pat, nocase);
    }
}

LIB_STV_FN size_t stv_naiveSearchRev(strview stv_text, strview stv_pat, bool nocase) {
    if (stv_impl_empty(stv_pat)) {
        return stv_text.len;
    }
    if (stv_impl_empty(stv_text) || stv_text.len < stv_pat.len) {
        return stv_npos;
    }

    const size_t begin_idx = (stv_text.len - stv_pat.len + 1);
    for (size_t idx = begin_idx; idx > 0; idx--) {
        size_t i;
        for (i = 0; i < stv_pat.len; i++) {
            char tch = stv_text.data[idx - 1 + i], pch = stv_pat.data[i];
            stv_impl_nocase(tch);
            stv_impl_nocase(pch);
            if (tch != pch) {
                break;
            }
        }
        if (i == stv_pat.len) {
            return idx - 1;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_sundaySearchRev(strview stv_text, strview stv_pat, bool nocase) {
    if (stv_impl_empty(stv_pat)) {
        return stv_text.len;
    }
    if (stv_impl_empty(stv_text) || stv_text.len < stv_pat.len) {
        return stv_npos;
    }

    const size_t txl = stv_text.len, pal = stv_pat.len;
    size_t       shift[UCHAR_MAX + 1];
    for (size_t i = 0; i <= UCHAR_MAX; i++) {
        shift[i] = pal + 1;
    }
    for (size_t i = pal; i > 0; i--) {
        char pch = stv_pat.data[i - 1];
        stv_impl_nocase(pch);
        shift[(unsigned char)pch] = i;
    }

    size_t idx = txl - pal;
    for (;;) {
        size_t i;
        for (i = 0; i < pal; i++) {
            char tch = stv_text.data[idx + i], pch = stv_pat.data[i];
            stv_impl_nocase(tch);
            stv_impl_nocase(pch);
            if (tch != pch) {
                if (idx == 0) {
                    return stv_npos;
                }
                unsigned char prev_char = stv_text.data[idx - 1];
                stv_impl_nocase(prev_char);
                const size_t skip = shift[prev_char];
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

LIB_STV_FN size_t stv_firstCh(strview stv, const char ch, bool invert) {
    if (stv_impl_empty(stv)) {
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

LIB_STV_FN size_t stv_lastCh(strview stv, const char ch, bool invert) {
    if (stv_impl_empty(stv)) {
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

LIB_STV_FN size_t stv_firstChs(strview stv, strview charset, bool invert) {
    if (stv_impl_empty(stv)) {
        return stv_npos;
    }

    stv_impl_init_charset(cbm);

    const char*  curr = stv.data;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        const unsigned char ch = *curr;
        if (stv_impl_inchs(ch, cbm) != invert) {
            return idx;
        }
        curr++;
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_lastChs(strview stv, strview charset, bool invert) {
    if (stv_impl_empty(stv)) {
        return stv_npos;
    }

    stv_impl_init_charset(cbm);

    const char*  curr = stv.data + stv.len;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        const unsigned char ch = *(--curr);
        if (stv_impl_inchs(ch, cbm) != invert) {
            return len - idx - 1;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_firstIf(strview stv, stv_charClassFn handle, bool invert) {
    if (stv_impl_empty(stv) || handle == nullptr) {
        return stv_npos;
    }

    const char*  curr = stv.data;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        const unsigned char ch = *curr;
        if ((handle(ch) != 0) != invert) {
            return idx;
        }
        curr++;
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_lastIf(strview stv, stv_charClassFn handle, bool invert) {
    if (stv_impl_empty(stv) || handle == nullptr) {
        return stv_npos;
    }

    const char*  curr = stv.data + stv.len;
    const size_t len  = stv.len;
    for (size_t idx = 0; idx < len; idx++) {
        const unsigned char ch = *(--curr);
        if ((handle(ch) != 0) != invert) {
            return len - idx - 1;
        }
    }
    return stv_npos;
}

LIB_STV_FN size_t stv_firstDiff(strview stv_left, strview stv_right, bool nocase) {
    if (stv_impl_same(stv_left, stv_right)) {
        return stv_npos;
    }
    if (stv_impl_empty(stv_left) && stv_impl_empty(stv_right)) {
        return stv_npos;
    }
    if (stv_impl_empty(stv_left) || stv_impl_empty(stv_right)) {
        return 0;
    }

    const size_t min_len   = stv_left.len > stv_right.len ? stv_right.len : stv_left.len;
    const char*  left_ptr  = stv_left.data;
    const char*  right_ptr = stv_right.data;
    for (size_t idx = 0; idx < min_len; idx++) {
        char lch = *left_ptr, rch = *right_ptr;
        stv_impl_nocase(lch);
        stv_impl_nocase(rch);
        if (lch != rch) {
            return idx;
        }
        left_ptr++, right_ptr++;
    }
    return (stv_left.len == stv_right.len) ? stv_npos : min_len;
}

LIB_STV_FN size_t stv_lastDiff(strview stv_left, strview stv_right, bool nocase) {
    if (stv_impl_same(stv_left, stv_right)) {
        return stv_npos;
    }
    if (stv_impl_empty(stv_left) && stv_impl_empty(stv_right)) {
        return stv_npos;
    }
    if (stv_impl_empty(stv_left) || stv_impl_empty(stv_right)) {
        return (stv_impl_empty(stv_left) ? stv_right.len : stv_left.len) - 1;
    }

    const size_t min_len   = stv_left.len < stv_right.len ? stv_left.len : stv_right.len;
    const size_t max_len   = stv_left.len > stv_right.len ? stv_left.len : stv_right.len;
    const char*  left_ptr  = stv_left.data + stv_left.len;
    const char*  right_ptr = stv_right.data + stv_right.len;
    for (size_t idx = 0; idx < min_len; idx++) {
        char lch = *(--left_ptr), rch = *(--right_ptr);
        stv_impl_nocase(lch);
        stv_impl_nocase(rch);
        if (lch != rch) {
            return max_len - idx - 1;
        }
    }
    return (stv_left.len == stv_right.len) ? stv_npos : max_len - min_len - 1;
}

LIB_STV_FN size_t stv_length(strview stv) {
    return stv_impl_length(stv);
}

LIB_STV_FN size_t stv_countCh(strview stv, char ch) {
    if (stv_impl_empty(stv)) {
        return 0;
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

LIB_STV_FN size_t stv_countChs(strview stv, strview charset) {
    if (stv_impl_empty(stv)) {
        return 0;
    }

    stv_impl_init_charset(cbm);

    size_t      sum     = 0;
    const char* pos     = stv.data;
    const char* end_pos = stv.data + stv.len;
    while (pos < end_pos) {
        const unsigned char ch = *pos;
        if (stv_impl_inchs(ch, cbm)) {
            sum++;
        }
        pos++;
    }
    return sum;
}

LIB_STV_FN size_t stv_countIf(strview stv, stv_charClassFn handle) {
    if (stv_impl_empty(stv) || handle == nullptr) {
        return 0;
    }

    size_t      sum     = 0;
    const char* pos     = stv.data;
    const char* end_pos = stv.data + stv.len;
    while (pos < end_pos) {
        const unsigned char ch = *pos;
        if (handle(ch)) {
            sum++;
        }
        pos++;
    }
    return sum;
}

LIB_STV_FN size_t stv_countSubstr(strview stv, strview sub) {
    if (stv_impl_empty(stv)) {
        return 0;
    }
    if (stv_impl_empty(sub)) {
        return stv.len;
    }

    size_t sum = 0, len = sub.len;
    for (;;) {
        const size_t pos = stv_search(stv, sub, false);
        if (pos == stv_npos) {
            break;
        }
        stv = stv_slice(stv, pos + len, stv_end);
        sum++;
    }
    return sum;
}

LIB_STV_FN bool stv_everyCh(strview stv, char ch) {
    const size_t sum = stv_countCh(stv, ch);
    return sum != 0 && sum == stv.len;
}

LIB_STV_FN bool stv_everyChs(strview stv, strview charset) {
    const size_t sum = stv_countChs(stv, charset);
    return sum != 0 && sum == stv.len;
}

LIB_STV_FN bool stv_everyIf(strview stv, stv_charClassFn handle) {
    const size_t sum = stv_countIf(stv, handle);
    return sum != 0 && sum == stv.len;
}

LIB_STV_FN bool stv_someCh(strview stv, char ch) {
    const size_t sum = stv_countCh(stv, ch);
    return sum > 0;
}

LIB_STV_FN bool stv_someChs(strview stv, strview charset) {
    const size_t sum = stv_countChs(stv, charset);
    return sum > 0;
}

LIB_STV_FN bool stv_someIf(strview stv, stv_charClassFn handle) {
    const size_t sum = stv_countIf(stv, handle);
    return sum > 0;
}

LIB_STV_FN bool stv_startsWith(strview stv_text, strview stv_pat, bool nocase) {
    if (stv_impl_empty(stv_pat)) {
        return true;
    }
    if (stv_impl_empty(stv_text) || stv_text.len < stv_pat.len) {
        return false;
    }

    const char* text_ptr = stv_text.data;
    const char* pat_ptr  = stv_pat.data;
    const char* end_ptr  = pat_ptr + stv_pat.len;
    while (pat_ptr < end_ptr) {
        char tch = *text_ptr, pch = *pat_ptr;
        stv_impl_nocase(tch);
        stv_impl_nocase(pch);
        if (tch != pch) {
            return false;
        }
        text_ptr++, pat_ptr++;
    }
    return true;
}

LIB_STV_FN bool stv_endsWith(strview stv_text, strview stv_pat, bool nocase) {
    if (stv_impl_empty(stv_pat)) {
        return true;
    }
    if (stv_impl_empty(stv_text) || stv_text.len < stv_pat.len) {
        return false;
    }

    const char* text_ptr  = stv_text.data + stv_text.len;
    const char* pat_ptr   = stv_pat.data + stv_pat.len;
    const char* begin_ptr = stv_pat.data;
    while (pat_ptr > begin_ptr) {
        char tch = *(--text_ptr), pch = *(--pat_ptr);
        stv_impl_nocase(tch);
        stv_impl_nocase(pch);
        if (tch != pch) {
            return false;
        }
    }
    return true;
}

LIB_STV_FN bool stv_contains(strview stv_text, strview stv_sub, bool nocase) {
    return stv_impl_empty(stv_sub) || stv_search(stv_text, stv_sub, nocase) != stv_npos;
}

LIB_STV_FN bool stv_equal(strview stv_left, strview stv_right) {
    return stv_firstDiff(stv_left, stv_right, false) == stv_npos;
}

LIB_STV_FN bool stv_equalEx(strview stv_left, strview stv_right, bool nocase) {
    return stv_firstDiff(stv_left, stv_right, nocase) == stv_npos;
}

LIB_STV_FN bool stv_same(strview stv_left, strview stv_right) {
    return stv_impl_same(stv_left, stv_right);
}

LIB_STV_FN bool stv_empty(strview stv) {
    return stv_impl_empty(stv);
}

LIB_STV_FN int stv_compare(strview stv_left, strview stv_right) {
    const size_t  pos = stv_firstDiff(stv_left, stv_right, false);
    unsigned char c1  = (pos < stv_left.len) ? stv_left.data[pos] : '\0';
    unsigned char c2  = (pos < stv_right.len) ? stv_right.data[pos] : '\0';
    return c1 - c2;
}

LIB_STV_FN int stv_compareEx(strview stv_left, strview stv_right, bool nocase) {
    const size_t  pos = stv_firstDiff(stv_left, stv_right, nocase);
    unsigned char c1  = (pos < stv_left.len) ? stv_left.data[pos] : '\0';
    unsigned char c2  = (pos < stv_right.len) ? stv_right.data[pos] : '\0';
    stv_impl_nocase(c1);
    stv_impl_nocase(c2);
    return c1 - c2;
}

LIB_STV_FN char stv_front(strview stv) {
    return (stv_impl_empty(stv) ? '\0' : stv.data[0]);
}

LIB_STV_FN char stv_back(strview stv) {
    return (stv_impl_empty(stv) ? '\0' : stv.data[stv.len - 1]);
}

LIB_STV_FN char stv_at(strview stv, size_t idx) {
    return ((stv_impl_empty(stv) || idx >= stv.len) ? '\0' : stv.data[idx]);
}

LIB_STV_FN void stv_forEach(strview stv, stv_forEachFn callback, void* ctx) {
    if (!stv_impl_empty(stv) && callback) {
        for (size_t idx = 0; idx < stv.len; idx++) {
            const char ch = stv.data[idx];
            callback(ch, idx, ctx);
        }
    }
}

LIB_STV_FN void stv_forEachRev(strview stv, stv_forEachFn callback, void* ctx) {
    if (!stv_impl_empty(stv) && callback) {
        for (size_t idx = stv.len; idx > 0; idx--) {
            const char ch = stv.data[idx - 1];
            callback(ch, idx - 1, ctx);
        }
    }
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
    if (stv_impl_empty(stv)) {
        return 0;
    }

    #if SIZE_MAX == UINT64_MAX
    const size_t fnv_offset_basis = 0xcbf29ce484222325ULL;
    const size_t fnv_prime        = 0x00000100000001b3ULL;
    #elif SIZE_MAX == UINT32_MAX
    const size_t fnv_offset_basis = 0x811c9dc5UL;
    const size_t fnv_prime        = 0x01000193UL;
    #elif SIZE_MAX == UINT16_MAX
    const size_t fnv_offset_basis = 0x811cU;
    const size_t fnv_prime        = 0x0101U;
    #else
    return 0;
    #endif

    size_t hash = fnv_offset_basis;
    for (size_t i = 0; i < stv.len; i++) {
        hash ^= (unsigned char)stv.data[i];
        hash *= fnv_prime;
    }
    return hash;
}

LIB_STV_FN char* stv_cstr(strview stv, char* mem, size_t size, stv_cstrOptions opts) {
    const bool UPPER        = (opts & stv_ToUpper);
    const bool LOWER        = (opts & stv_ToLower);
    const bool REVERSE      = (opts & stv_Reverse);
    const bool TRUNCATE     = (opts & stv_Truncate);
    const bool VIEW_REVERSE = (opts & stv_ViewReverse);

    const bool overflow = size <= stv.len;
    if (mem == nullptr || size == 0 || (overflow && !TRUNCATE)) {
        return nullptr;
    }

    if (stv_impl_empty(stv) || size == 1) {
        *mem = '\0';
        return mem;
    }

    size_t endidx = overflow ? (size - 1) : stv.len;

    for (size_t i = 0; i < endidx; i++) {
        size_t si = VIEW_REVERSE ? (stv.len - i - 1) : i;
        size_t di = REVERSE ? (endidx - i - 1) : i;

        char ch = stv.data[si];
        if (UPPER && ch >= 'a' && ch <= 'z') {
            ch -= 32;
        } else if (LOWER && ch >= 'A' && ch <= 'Z') {
            ch += 32;
        }
        mem[di] = ch;
    }

    mem[endidx] = '\0';
    return mem;
}

LIB_STV_FN char* stv_join(strview stv_arr[], size_t arr_len, char* mem, size_t size, strview sep,
                          stv_cstrOptions opts) {
    if (mem == nullptr || size == 0 || (stv_arr == nullptr && arr_len > 0)) {
        return nullptr;
    }

    const bool REVERSE      = (opts & stv_Reverse);
    const bool TRUNCATE     = (opts & stv_Truncate);
    const bool JOIN_REVERSE = (opts & stv_JoinReverse);

    const size_t sep_len    = stv_impl_length(sep);
    size_t       needed_len = 0;
    bool         overflow   = false;
    if (arr_len > 0) {
        for (size_t i = 0; i < arr_len; i++) {
            const size_t len = ((i > 0) ? sep_len : 0) + stv_impl_length(stv_arr[i]);
            if ((size - needed_len) <= len) {
                overflow = true;
                break;
            }
            needed_len += len;
        }
    }

    if (overflow && !TRUNCATE) {
        return nullptr;
    }

    size_t total_len = overflow ? (size - 1) : needed_len;

    if (total_len == 0) {
        *mem = '\0';
        return mem;
    }

    stv_cstrOptions view_opts = (stv_cstrOptions)((opts & ~stv_Reverse) | stv_Truncate);

    size_t pos     = 0;
    size_t rem_len = total_len;

    for (size_t idx = 0; idx < arr_len && rem_len > 0; idx++) {
        const strview view     = stv_arr[(JOIN_REVERSE ? (arr_len - idx - 1) : idx)];
        const size_t  view_len = stv_impl_length(view);

        if (sep_len > 0 && idx > 0) {
            const size_t take = stv_impl_min(rem_len, sep_len);
            (void)stv_cstr(sep, mem + pos, take + 1, stv_Truncate);
            pos += take, rem_len -= take;
        }

        if (rem_len == 0) {
            break;
        }

        if (view_len > 0) {
            const size_t take = stv_impl_min(rem_len, view_len);
            (void)stv_cstr(view, mem + pos, take + 1, view_opts);
            pos += take, rem_len -= take;
        }
    }

    if (REVERSE) {
        char *left = mem, *right = mem + total_len - 1;
        while (left < right) {
            const char tmp = *left;
            *left          = *right;
            *right         = tmp;
            left++, right--;
        }
    }

    mem[total_len] = '\0';
    return mem;
}

LIB_STV_FN intmax_t stv_parseInum(strview stv, int base, strview* remaining) {
    stv = stv_trimStartChs(stv, stv_whitespace);
    if (stv_impl_empty(stv)) {
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
        const char* pre = (base == 2) ? "0b" : (base == 8) ? "0o" : (base == 10) ? "0d" : (base == 16) ? "0x" : nullptr;
        if (pre) {
            if (stv_startsWith(stv, stv_makestv(pre, 2), true)) {
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

    if (negative && (acc == (uintmax_t)INTMAX_MAX + 1)) {
        return INTMAX_MIN;
    }

    return negative ? -(intmax_t)acc : (intmax_t)acc;
}

LIB_STV_FN uintmax_t stv_parseUnum(strview stv, int base, strview* remaining) {
    stv = stv_trimStartChs(stv, stv_whitespace);
    if (stv_impl_empty(stv)) {
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
        const char* pre = (base == 2) ? "0b" : (base == 8) ? "0o" : (base == 10) ? "0d" : (base == 16) ? "0x" : nullptr;
        if (pre) {
            if (stv_startsWith(stv, stv_makestv(pre, 2), true)) {
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
    if (stv_impl_empty(stv)) {
        if (remaining) {
            *remaining = stv;
        }
        return 0;
    }

    int        base      = 10;
    size_t     num_start = 2;
    const char zero_ch   = stv.data[0];
    if (zero_ch == '0' && stv.len >= 2) {
        const char base_ch = (char)((unsigned char)stv.data[1] | 32);
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

    #undef stv_impl_length
    #undef stv_impl_empty
    #undef stv_impl_same
    #undef stv_impl_min
    #undef stv_impl_max
    #undef stv_impl_init_charset
    #undef stv_impl_inchs
    #undef stv_impl_nocase

#endif // LIB_STV_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !LIB_STV_H
