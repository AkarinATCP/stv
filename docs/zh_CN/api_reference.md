# API Reference

> Language: **简体中文** | [English](../en_US/api_reference.md)

*本文档提供 stv.h 中每个公开函数和宏的详细信息。*  
*快速概览和集成指南请参阅 [README](../../README.zh_CN.md)。*

## 目录

- [类型定义](#类型定义)
  - [strview](#strview)
  - [stv_charClassFn](#stv_charclassfn)
  - [stv_forEachFn](#stv_foreachfn)
  - [stv_cstrOptions](#stv_cstroptions)
- [创建](#创建)
  - [stv_new](#stv_new)
  - [stv_create](#stv_create)
  - [stv_literal（宏）](#stv_literal宏)
  - [stv_makestv（宏）](#stv_makestv宏)
  - [stv_nullstv（宏）](#stv_nullstv宏)
- [切片](#切片)
  - [stv_slice](#stv_slice)
  - [stv_removeStart](#stv_removestart)
  - [stv_removeEnd](#stv_removeend)
  - [stv_removePrefix](#stv_removeprefix)
  - [stv_removePrefixNocase](#stv_removeprefixnocase)
  - [stv_removeSuffix](#stv_removesuffix)
  - [stv_removeSuffixNocase](#stv_removesuffixnocase)
- [分割](#分割)
  - [stv_split](#stv_split)
  - [stv_splitLines](#stv_splitlines)
  - [stv_splitWords](#stv_splitwords)
  - [stv_beforeFirstDelim](#stv_beforefirstdelim)
  - [stv_beforeLastDelim](#stv_beforelastdelim)
  - [stv_afterFirstDelim](#stv_afterfirstdelim)
  - [stv_afterLastDelim](#stv_afterlastdelim)
- [修剪](#修剪)
  - [stv_trim（宏）](#stv_trim宏)
  - [stv_trimStart（宏）](#stv_trimstart宏)
  - [stv_trimEnd（宏）](#stv_trimend宏)
  - [stv_trimChs](#stv_trimchs)
  - [stv_trimStartChs](#stv_trimstartchs)
  - [stv_trimEndChs](#stv_trimendchs)
  - [stv_trimIf](#stv_trimif)
  - [stv_trimStartIf](#stv_trimstartif)
  - [stv_trimEndIf](#stv_trimendif)
  - [stv_whitespace（宏）](#stv_whitespace宏)
- [搜索](#搜索)
  - [stv_firstIndex（宏）](#stv_firstindex宏)
  - [stv_lastIndex（宏）](#stv_lastindex宏)
  - [stv_firstChar](#stv_firstchar)
  - [stv_lastChar](#stv_lastchar)
  - [stv_firstCharset](#stv_firstcharset)
  - [stv_lastCharset](#stv_lastcharset)
  - [stv_firstCharClass](#stv_firstcharclass)
  - [stv_lastCharClass](#stv_lastcharclass)
  - [stv_search](#stv_search)
  - [stv_naiveSearch](#stv_naivesearch)
  - [stv_sundaySearch](#stv_sundaysearch)
  - [stv_rev_search](#stv_rev_search)
  - [stv_rev_naiveSearch](#stv_rev_naivesearch)
  - [stv_rev_sundaySearch](#stv_rev_sundaysearch)
- [比较](#比较)
  - [stv_firstDiff](#stv_firstdiff)
  - [stv_lastDiff](#stv_lastdiff)
  - [stv_compare](#stv_compare)
  - [stv_compareNocase](#stv_comparenocase)
  - [stv_startsWith](#stv_startswith)
  - [stv_startsWithNocase](#stv_startswithnocase)
  - [stv_endsWith](#stv_endswith)
  - [stv_endsWithNocase](#stv_endswithnocase)
  - [stv_contains](#stv_contains)
  - [stv_containsNocase](#stv_containsnocase)
  - [stv_equal](#stv_equal)
  - [stv_equalNocase](#stv_equalnocase)
  - [stv_same](#stv_same)
  - [stv_empty](#stv_empty)
- [计数 / 谓词](#计数--谓词)
  - [stv_count（宏）](#stv_count宏)
  - [stv_countIf](#stv_countif)
  - [stv_countChar](#stv_countchar)
  - [stv_countSubstr](#stv_countsubstr)
  - [stv_every（宏）](#stv_every宏)
  - [stv_everyIf](#stv_everyif)
  - [stv_everyChar](#stv_everychar)
  - [stv_some（宏）](#stv_some宏)
  - [stv_someIf](#stv_someif)
  - [stv_someChar](#stv_somechar)
- [工具函数](#工具函数)
  - [stv_front](#stv_front)
  - [stv_back](#stv_back)
  - [stv_at](#stv_at)
  - [stv_forEach](#stv_foreach)
  - [stv_forEachRev](#stv_foreachrev)
  - [stv_swap](#stv_swap)
  - [stv_hash](#stv_hash)
  - [stv_hash_FNV1a](#stv_hash_fnv1a)
  - [stv_PFARG / stv_PFFMT（宏）](#stv_pfarg--stv_pffmt宏)
  - [stv_LIST（宏）](#stv_list宏)
  - [stv_npos / stv_begin / stv_end（宏）](#stv_npos--stv_begin--stv_end宏)
- [C 字符串转换](#c-字符串转换)
  - [stv_cstr](#stv_cstr)
  - [stv_opt_cstr](#stv_opt_cstr)
  - [stv_opt_join](#stv_opt_join)
- [数值解析](#数值解析)
  - [stv_parseInum](#stv_parseinum)
  - [stv_parseUnum](#stv_parseunum)
  - [stv_ch2digit](#stv_ch2digit)
  - [stv_parseIntBase](#stv_parseintbase)

---

## 类型定义

### `strview`
```c
typedef struct {
    const char* data;
    size_t      len;
} strview;
```
字符串视图，表示一段只读字符序列。`data` 指向首字符（可为 NULL），`len` 是字节数。它不以空字符结尾，且不拥有内存。

### `stv_charClassFn`
```c
typedef int (*stv_charClassFn)(int);
```
字符分类函数指针，如 `isspace`、`isdigit`。接收一个字符，返回非零表示属于该类别。

### `stv_forEachFn`
```c
typedef void (*stv_forEachFn)(char ch, size_t idx, strview ctx);
```
遍历回调类型。参数为当前字符、索引和原始视图。

### `stv_cstrOptions`
```c
typedef enum {
    stv_Default  = 0,
    stv_ToUpper  = 1,
    stv_ToLower  = 2,
    stv_Reverse  = 4,
    stv_Truncate = 8,
} stv_cstrOptions;
```
输出选项，可位或组合：
- `stv_Default`：无变换。
- `stv_ToUpper`：转大写（仅 ASCII 字母）。
- `stv_ToLower`：转小写。
- `stv_Reverse`：反转字符顺序。
- `stv_Truncate`：若视图长度 ≥ 缓冲区大小则截断。

同时设置 `stv_ToUpper` 和 `stv_ToLower` 会实现大小写翻转（swapCase）。

---

## 创建

### `stv_new`
```c
strview stv_new(const char* c_str);
```
从 C 字符串构造视图。视图长度不含空终止符。

示例：
```c
strview sv = stv_new("hello"); // sv = {"hello", 5}
```

| 参数    | 说明                             |
|---------|----------------------------------|
| `c_str` | 空终止的 C 字符串指针，可为 NULL |

| 返回    | 说明                           |
|---------|--------------------------------|
| 新视图  | 若 `c_str` 为 NULL，返回空视图 |

### `stv_create`
```c
strview stv_create(const char* str, unsigned char endchar, size_t maxlen);
```
扫描 `str` 构造视图，最多扫描 `maxlen` 字节，达到上限或遇到 `endchar` 停止。

示例：
```c
strview sv  = stv_create("abc:def", ':', 10); // sv = {"abc", 3}
strview sv2 = stv_create("abc", '\0', 2);     // sv2 = {"ab", 2}
```

| 参数      | 说明                      |
|-----------|---------------------------|
| `str`     | 源字符串指针，可为 NULL   |
| `endchar` | 终止字符（不包含在视图中）|
| `maxlen`  | 最大扫描长度              |

| 返回   | 说明                       |
|--------|----------------------------|
| 新视图 | `str` 为 NULL 时返回空视图 |

### `stv_literal`（宏）
```c
#define stv_literal(str) stv_makestv((str), sizeof(str) - 1)
```
用字符串字面量在编译期构造视图，自动计算长度（不含空字符）。

示例：
```c
strview sv = stv_literal("Hello"); // sv = {"Hello", 5}
```

### `stv_makestv`（宏）
```c
#ifdef __cplusplus
    #define stv_makestv(data_v, len_v) (strview{(data_v), (len_v)})
#else
    #define stv_makestv(data_v, len_v) ((strview){.data = (data_v), .len = (len_v)})
#endif
```
通用构造宏，用指针和长度生成视图。

示例：
```c
strview sv = stv_makestv(buf, 3); // sv = {buf, 3}
```

### `stv_nullstv`（宏）
```c
#define stv_nullstv stv_makestv(nullptr, 0)
```
预定义的空视图，`data == NULL`，`len == 0`。

---

## 切片

### `stv_slice`
```c
strview stv_slice(strview stv, size_t begin_pos, size_t end_pos);
```
提取子串视图 `[begin_pos, end_pos)`。索引越界或 `begin_pos >= end_pos` 则范围无效。

可用 [`stv_end`](#stv_npos--stv_begin--stv_end宏) 表示切片到末尾

示例：
```c
strview sv   = stv_literal("Hello");
strview sub  = stv_slice(sv, 1, 4);       // sub = "ell"
strview rest = stv_slice(sv, 2, stv_end); // rest = "llo"
```

| 参数        | 说明            |
|-------------|-----------------|
| `stv`       | 源视图          |
| `begin_pos` | 起始索引（含）  |
| `end_pos`   | 结束索引（不含）|

| 返回   | 说明               |
|--------|--------------------|
| 子视图 | 范围无效时为空视图 |

### `stv_removeStart`
```c
strview stv_removeStart(strview stv, size_t len);
```
从开头移除 `len` 字节。若 `len >= stv.len` 返回空视图。相当于 `stv_slice(stv, len, stv_end)`。

示例：
```c
strview sv = stv_removeStart(stv_literal("Hello World"), 6); // "World"
```

| 参数  | 说明           |
|-------|----------------|
| `stv` | 源视图         |
| `len` | 要移除的字节数 |

| 返回   | 说明         |
|--------|--------------|
| 新视图 | 移除后的视图 |

### `stv_removeEnd`
```c
strview stv_removeEnd(strview stv, size_t len);
```
从末尾移除 `len` 字节。若 `len >= stv.len` 返回空视图。相当于 `stv_slice(stv, stv_begin, stv.len - len)`。

示例：
```c
strview sv = stv_removeEnd(stv_literal("Hello World"), 6); // "Hello"
```

| 参数  | 说明           |
|-------|----------------|
| `stv` | 源视图         |
| `len` | 要移除的字节数 |

| 返回   | 说明             |
|--------|------------------|
| 新视图 | 移除后缀后的视图 |

### `stv_removePrefix`
```c
strview stv_removePrefix(strview stv, strview prefix);
```
若 `stv` 以 `prefix` 开头（通过 [`stv_startsWith`](#stv_startswith) 判断），则返回去除该前缀后的视图；否则返回原视图。

示例：
```c
strview sv = stv_literal("http://example.com");
strview result = stv_removePrefix(sv, stv_literal("http://"));
// result = "example.com"
```

| 参数     | 说明         |
|----------|--------------|
| `stv`    | 源视图       |
| `prefix` | 要移除的前缀 |

| 返回 | 说明                     |
|------|--------------------------|
| 视图 | 去除前缀后的视图或原视图 |

### `stv_removePrefixNocase`
```c
strview stv_removePrefixNocase(strview stv, strview prefix);
```
同 [`stv_removePrefix`](#stv_removeprefix)，但前缀匹配忽略大小写（通过 [`stv_startsWithNocase`](#stv_startswithnocase) 判断）。

示例：
```c
strview sv = stv_literal("http://example.com");
strview result = stv_removePrefixNocase(sv, stv_literal("HTTP://"));
// result = "example.com"
```

| 参数     | 说明         |
|----------|--------------|
| `stv`    | 源视图       |
| `prefix` | 要移除的前缀 |

| 返回 | 说明                     |
|------|--------------------------|
| 视图 | 去除前缀后的视图或原视图 |

### `stv_removeSuffix`
```c
strview stv_removeSuffix(strview stv, strview suffix);
```
若 `stv` 以 `suffix` 结尾（通过 [`stv_endsWith`](#stv_endswith) 判断），则返回去除该后缀后的视图；否则返回原视图。

示例：
```c
strview sv = stv_literal("document.txt");
strview result = stv_removeSuffix(sv, stv_literal(".txt"));
// result = "document"
```

| 参数     | 说明         |
|----------|--------------|
| `stv`    | 源视图       |
| `suffix` | 要移除的后缀 |

| 返回 | 说明                     |
|------|--------------------------|
| 视图 | 去除后缀后的视图或原视图 |

### `stv_removeSuffixNocase`
```c
strview stv_removeSuffixNocase(strview stv, strview suffix);
```
同 [`stv_removeSuffix`](#stv_removesuffix)，但后缀匹配忽略大小写（通过 [`stv_endsWithNocase`](#stv_endswithnocase) 判断）。

示例：
```c
strview sv = stv_literal("document.txt");
strview result = stv_removeSuffixNocase(sv, stv_literal(".TXT"));
// result = "document"
```

| 参数     | 说明         |
|----------|--------------|
| `stv`    | 源视图       |
| `suffix` | 要移除的后缀 |

| 返回 | 说明                     |
|------|--------------------------|
| 视图 | 去除后缀后的视图或原视图 |

---

## 分割

### `stv_split`
```c
strview stv_split(strview stv, strview sep, bool nocase, strview* remaining);
```
找到第一个分隔符 `sep`，返回其前的部分，并通过 `remaining` 输出剩余部分（不含 `sep`）。

若未找到，返回完整视图且 `*remaining` 为空视图。

若 `sep` 为空视图，则按字符分割。

示例：
```c
strview rem;
strview first = stv_split(stv_literal("hello world"), stv_literal(" "), false, &rem);
// first = "hello", rem = "world"
```

| 参数        | 说明                                         |
|-------------|----------------------------------------------|
| `stv`       | 待分割视图                                   |
| `sep`       | 分隔符视图（为空则按字符分割）               |
| `nocase`    | 是否忽略大小写                               |
| `remaining` | 输出参数，接收分隔符后的剩余视图（可为 NULL）|

| 返回 | 说明           |
|------|----------------|
| 视图 | 分隔符前的部分 |

### `stv_splitLines`
```c
strview stv_splitLines(strview stv, strview* remaining);
```
按行分割，支持 LF、CR、CRLF。返回第一行（不含换行符），`remaining` 接收剩余内容。

示例：
```c
strview line, rest;
line = stv_splitLines(stv_literal("hello\r\nworld"), &rest);
// line = "hello", rest = "world"
```

| 参数        | 说明                     |
|-------------|--------------------------|
| `stv`       | 待分割视图               |
| `remaining` | 输出剩余部分（可为 NULL）|

| 返回   | 说明                               |
|--------|------------------------------------|
| 第一行 | 不含换行符；若无换行符返回整个视图 |

### `stv_splitWords`
```c
strview stv_splitWords(strview stv, strview* remaining);
```
跳过空白，提取下一个连续非空白单词。剩余内容通过 `remaining` 输出。

若无单词，返回空视图且 `*remaining` 为空视图。

> 空白符定义见 [`stv_whitespace`](#stv_whitespace宏)。

示例：
```c
strview rem;
strview word = stv_splitWords(stv_literal("  hello world  "), &rem);
// word = "hello", rem = " world  "
```

| 参数        | 说明                     |
|-------------|--------------------------|
| `stv`       | 待分割视图               |
| `remaining` | 输出剩余部分（可为 NULL）|

| 返回     | 说明                           |
|----------|--------------------------------|
| 首个单词 | 无空白序列；若无单词返回空视图 |

### `stv_beforeFirstDelim`
```c
strview stv_beforeFirstDelim(strview stv, strview delim);
```
返回第一个 `delim` 之前的部分；若 `delim` 为空则返回空视图；若未找到则返回原视图。

示例：
```c
strview base = stv_beforeFirstDelim(stv_literal("key=value"), stv_literal("=")); // "key"
```

| 参数    | 说明                          |
|---------|-------------------------------|
| `stv`   | 源视图                        |
| `delim` | 分隔符视图（为空则返回空视图）|

| 返回 | 说明             |
|------|------------------|
| 视图 | 分隔符之前的部分 |

### `stv_beforeLastDelim`
```c
strview stv_beforeLastDelim(strview stv, strview delim);
```
返回最后一个 `delim` 之前的部分；若 `delim` 为空则返回空视图；若未找到则返回原视图。

示例：
```c
strview dir = stv_beforeLastDelim(stv_literal("a/b/c"), stv_literal("/")); // "a/b"
```

| 参数    | 说明                          |
|---------|-------------------------------|
| `stv`   | 源视图                        |
| `delim` | 分隔符视图（为空则返回空视图）|

| 返回 | 说明             |
|------|------------------|
| 视图 | 分隔符之前的部分 |

### `stv_afterFirstDelim`
```c
strview stv_afterFirstDelim(strview stv, strview delim);
```
返回第一个 `delim` 之后的部分；若未找到返回空视图；若 `delim` 为空返回原视图。

示例：
```c
strview val = stv_afterFirstDelim(stv_literal("key=value"), stv_literal("=")); // "value"
```

| 参数    | 说明                          |
|---------|-------------------------------|
| `stv`   | 源视图                        |
| `delim` | 分隔符视图（为空则返回源视图）|

| 返回 | 说明             |
|------|------------------|
| 视图 | 分隔符之后的部分 |

### `stv_afterLastDelim`
```c
strview stv_afterLastDelim(strview stv, strview delim);
```
返回最后一个 `delim` 之后的部分；若未找到返回空视图；若 `delim` 为空返回原视图。

示例：
```c
strview file = stv_afterLastDelim(stv_literal("a/b/c"), stv_literal("/")); // "c"
```

| 参数    | 说明                          |
|---------|-------------------------------|
| `stv`   | 源视图                        |
| `delim` | 分隔符视图（为空则返回源视图）|

| 返回 | 说明             |
|------|------------------|
| 视图 | 分隔符之后的部分 |

---

## 修剪

### `stv_trim`（宏）
```c
#define stv_trim(stv, target) /* ... */
```
移除首尾字符。

C11 `_Generic` 宏。根据 `target` 类型分派：
- `strview` -> [`stv_trimChs`](#stv_trimchs)
- `stv_charClassFn` -> [`stv_trimIf`](#stv_trimif)

示例：
```c
stv_trim(sv, stv_whitespace);   // 调用 stv_trimChs
stv_trim(sv, isspace);          // 调用 stv_trimIf
```

### `stv_trimStart`（宏）
```c
#define stv_trimStart(stv, target) /* ... */
```
类似 [`stv_trim`](#stv_trim宏)，但仅修剪首部。

分派到 [`stv_trimStartChs`](#stv_trimstartchs) 或 [`stv_trimStartIf`](#stv_trimstartif)。

### `stv_trimEnd`（宏）
```c
#define stv_trimEnd(stv, target) /* ... */
```
类似 [`stv_trim`](#stv_trim宏)，但仅修剪尾部。

分派到 [`stv_trimEndChs`](#stv_trimendchs) 或 [`stv_trimEndIf`](#stv_trimendif)。

### `stv_trimChs`
```c
strview stv_trimChs(strview stv, strview charset);
```
移除首尾所有出现在 `charset` 中的字符，若为空集合（空视图）则不做修剪。

示例：
```c
strview sv = stv_literal("  hello  ");
strview trimmed = stv_trimChs(sv, stv_whitespace);
// trimmed = "hello"
```

| 参数      | 说明             |
|-----------|------------------|
| `stv`     | 源视图           |
| `charset` | 要移除的字符集合 |

| 返回   | 说明         |
|--------|--------------|
| 新视图 | 修剪后的视图 |

### `stv_trimStartChs`
```c
strview stv_trimStartChs(strview stv, strview charset);
```
仅移除首部的 `charset` 字符，若为空集合（空视图）则不做修剪。

示例：
```c
strview sv = stv_literal("  hello  ");
strview trimmed = stv_trimStartChs(sv, stv_whitespace);
// trimmed = "hello  "
```

| 参数      | 说明             |
|-----------|------------------|
| `stv`     | 源视图           |
| `charset` | 要移除的字符集合 |

| 返回   | 说明         |
|--------|--------------|
| 新视图 | 修剪后的视图 |

### `stv_trimEndChs`
```c
strview stv_trimEndChs(strview stv, strview charset);
```
仅移除尾部的 `charset` 字符，若为空集合（空视图）则不做修剪。

示例：
```c
strview sv = stv_literal("  hello  ");
strview trimmed = stv_trimEndChs(sv, stv_whitespace);
// trimmed = "  hello"
```

| 参数      | 说明             |
|-----------|------------------|
| `stv`     | 源视图           |
| `charset` | 要移除的字符集合 |

| 返回   | 说明         |
|--------|--------------|
| 新视图 | 修剪后的视图 |

### `stv_trimIf`
```c
strview stv_trimIf(strview stv, stv_charClassFn handle);
```
移除首尾所有满足 `handle` 分类函数的字符，若 `handle` 为 NULL 则不做修剪。

> 有关字符分类函数，参见 [`stv_charClassFn`](#stv_charclassfn)

示例：
```c
strview sv = stv_literal("  text  ");
strview trimmed = stv_trimIf(sv, isspace);
// trimmed = "text"
```

| 参数     | 说明             |
|----------|------------------|
| `stv`    | 源视图           |
| `handle` | 字符分类函数指针 |

| 返回   | 说明         |
|--------|--------------|
| 新视图 | 修剪后的视图 |

### `stv_trimStartIf`
```c
strview stv_trimStartIf(strview stv, stv_charClassFn handle);
```
仅移除首部满足 `handle` 的字符，若 `handle` 为 NULL 则不做修剪。

> 有关字符分类函数，参见 [`stv_charClassFn`](#stv_charclassfn)

示例：
```c
strview sv = stv_literal("  text  ");
strview trimmed = stv_trimStartIf(sv, isspace);
// trimmed = "text  "
```

| 参数     | 说明             |
|----------|------------------|
| `stv`    | 源视图           |
| `handle` | 字符分类函数指针 |

| 返回   | 说明         |
|--------|--------------|
| 新视图 | 修剪后的视图 |

### `stv_trimEndIf`
```c
strview stv_trimEndIf(strview stv, stv_charClassFn handle);
```
仅移除尾部满足 `handle` 的字符，若 `handle` 为 NULL 则不做修剪。

> 有关字符分类函数，参见 [`stv_charClassFn`](#stv_charclassfn)

示例：
```c
strview sv = stv_literal("  text  ");
strview trimmed = stv_trimEndIf(sv, isspace);
// trimmed = "  text"
```

| 参数     | 说明             |
|----------|------------------|
| `stv`    | 源视图           |
| `handle` | 字符分类函数指针 |

| 返回   | 说明         |
|--------|--------------|
| 新视图 | 修剪后的视图 |

### `stv_whitespace`（宏）
```c
#define stv_whitespace stv_literal(" \r\n\t\v\f")
```
预定义的空白字符集合视图。用作 [`stv_trimChs`](#stv_trimchs)、[`stv_splitWords`](#stv_splitwords) 等函数的参数。

---

## 搜索

### `stv_firstIndex`（宏）
```c
#define stv_firstIndex(stv, target, invert) /* ... */
```
查找第一个符合条件的字符索引。

C11 `_Generic` 宏。根据 `target` 类型分派：
- `char` -> [`stv_firstChar`](#stv_firstchar)
- `strview` -> [`stv_firstCharset`](#stv_firstcharset)
- `stv_charClassFn` -> [`stv_firstCharClass`](#stv_firstcharclass)

示例：
```c
stv_firstIndex(sv, 'a', false);                  // 调用 stv_firstChar
stv_firstIndex(sv, stv_literal("aeiou"), false); // 调用 stv_firstCharset
stv_firstIndex(sv, isalpha, false);              // 调用 stv_firstCharClass
```

### `stv_lastIndex`（宏）
```c
#define stv_lastIndex(stv, target, invert) /* ... */
```
类似 [`stv_firstIndex`](#stv_firstindex宏)，但查找最后一个符合条件的字符索引。

分派到 [`stv_lastChar`](#stv_lastchar)、[`stv_lastCharset`](#stv_lastcharset) 或 [`stv_lastCharClass`](#stv_lastcharclass)。

### `stv_firstChar`
```c
size_t stv_firstChar(strview stv, const char ch, bool invert);
```
查找第一个**等于** `ch` 的字符索引，若未找到则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若 `invert` 为 true 则改为查找第一个**不等于** `ch` 的字符索引

示例：
```c
strview sv = stv_literal("hello");
size_t p1 = stv_firstChar(sv, 'l', false); // 2
size_t p2 = stv_firstChar(sv, 'h', true);  // 1 (第一个不等于 'h' 的字符位置)
```

| 参数     | 说明             |
|----------|------------------|
| `stv`    | 源视图           |
| `ch`     | 要匹配的字符     |
| `invert` | 是否反转搜索逻辑 |

| 返回 | 说明               |
|------|--------------------|
| 索引 | 符合条件的字符下标 |

### `stv_lastChar`
```c
size_t stv_lastChar(strview stv, const char ch, bool invert);
```
查找最后一个**等于** `ch` 的字符索引，若未找到则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若 `invert` 为 true 则改为查找最后一个**不等于** `ch` 的字符索引。

示例：
```c
strview sv = stv_literal("hello");
size_t p1 = stv_lastChar(sv, 'l', false); // 3
size_t p2 = stv_lastChar(sv, 'h', true);  // 4 (最后一个不等于 'h' 的字符位置)
```

| 参数     | 说明             |
|----------|------------------|
| `stv`    | 源视图           |
| `ch`     | 要匹配的字符     |
| `invert` | 是否反转搜索逻辑 |

| 返回 | 说明               |
|------|--------------------|
| 索引 | 符合条件的字符下标 |

### `stv_firstCharset`
```c
size_t stv_firstCharset(strview stv, strview charset, bool invert);
```
查找第一个**属于**字符集 `charset` 的字符索引，若未找到或空 `charset` 则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若 `invert` 为 true 则改为查找第一个**不属于** `charset` 的字符索引。

示例：
```c
strview sv = stv_literal("abc123");
size_t p1 = stv_firstCharset(sv, stv_literal("abc"), false); // 0
size_t p2 = stv_firstCharset(sv, stv_literal("abc"), true);  // 3 (第一个不在 "abc" 中的字符)
```

| 参数      | 说明             |
|-----------|------------------|
| `stv`     | 源视图           |
| `charset` | 要匹配的字符集   |
| `invert`  | 是否反转搜索逻辑 |

| 返回 | 说明               |
|------|--------------------|
| 索引 | 符合条件的字符下标 |

### `stv_lastCharset`
```c
size_t stv_lastCharset(strview stv, strview charset, bool invert);
```
查找最后一个**属于**字符集 `charset` 的字符索引，若未找到或空 `charset` 则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若 `invert` 为 true 则改为查找最后一个**不属于** `charset` 的字符索引。

示例：
```c
strview sv = stv_literal("abc123def456");
size_t p1 = stv_lastCharset(sv, stv_literal("0123456789"), false); // 11
size_t p2 = stv_lastCharset(sv, stv_literal("0123456789"), true);  // 8 (最后一个不是数字的字符索引)
```

| 参数      | 说明             |
|-----------|------------------|
| `stv`     | 源视图           |
| `charset` | 要匹配的字符集   |
| `invert`  | 是否反转搜索逻辑 |

| 返回 | 说明               |
|------|--------------------|
| 索引 | 符合条件的字符下标 |

### `stv_firstCharClass`
```c
size_t stv_firstCharClass(strview stv, stv_charClassFn handle, bool invert);
```
查找第一个**满足** `handle` 分类的字符索引，若未找到或 `handle` 为 NULL 则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若 `invert` 为 true 则改为查找第一个**不满足** `handle` 的字符索引。

> 有关字符分类函数，参见 [`stv_charClassFn`](#stv_charclassfn)

示例：
```c
strview sv = stv_literal("abc123");
size_t p1 = stv_firstCharClass(sv, isdigit, false); // 3
size_t p2 = stv_firstCharClass(sv, isdigit, true);  // 0 (第一个不是数字的字符)
```
| 参数     | 说明             |
|----------|------------------|
| `stv`    | 源视图           |
| `handle` | 字符分类函数指针 |
| `invert` | 是否反转搜索逻辑 |

| 返回 | 说明               |
|------|--------------------|
| 索引 | 符合条件的字符下标 |

### `stv_lastCharClass`
```c
size_t stv_lastCharClass(strview stv, stv_charClassFn handle, bool invert);
```
查找最后一个**满足** `handle` 分类的字符索引，若未找到或 `handle` 为 NULL 则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若 `invert` 为 true 则改为查找最后一个**不满足** `handle` 的字符索引。

> 有关字符分类函数，参见 [`stv_charClassFn`](#stv_charclassfn)

示例：
```c
strview sv = stv_literal("123abc456");
size_t p1 = stv_lastCharClass(sv, isdigit, false); // 8
size_t p2 = stv_lastCharClass(sv, isdigit, true);  // 5 (最后一个不是数字的字符索引)
```

| 参数     | 说明             |
|----------|------------------|
| `stv`    | 源视图           |
| `handle` | 字符分类函数指针 |
| `invert` | 是否反转搜索逻辑 |

| 返回 | 说明               |
|------|--------------------|
| 索引 | 符合条件的字符下标 |

### `stv_search`
```c
size_t stv_search(strview stv_text, strview stv_pat, bool nocase);
```
正向搜索，在 `stv_text` 中查找第一次出现 `stv_pat` 的位置，若 `stv_pat` 为空视图则返回 `0`，  
若未找到则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

对于 `stv_pat`，`len > 4` 时使用 [`stv_sundaySearch`](#stv_sundaysearch)，否则使用 [`stv_naiveSearch`](#stv_naivesearch)。

示例：
```c
strview text = stv_literal("hello world");
size_t pos = stv_search(text, stv_literal("world"), false); // 6
```

| 参数       | 说明           |
|------------|----------------|
| `stv_text` | 文本视图       |
| `stv_pat`  | 模式视图       |
| `nocase`   | 是否忽略大小写 |

| 返回 | 说明                     |
|------|--------------------------|
| 索引 | 模式第一次出现的起始位置 |

### `stv_naiveSearch`
```c
size_t stv_naiveSearch(strview stv_text, strview stv_pat, bool nocase);
```
朴素字符串搜索算法，适用于短模式。参数和返回值同 [`stv_search`](#stv_search)。

### `stv_sundaySearch`
```c
size_t stv_sundaySearch(strview stv_text, strview stv_pat, bool nocase);
```
Sunday 算法搜索，适用于较长模式。参数和返回值同 [`stv_search`](#stv_search)。

### `stv_rev_search`
```c
size_t stv_rev_search(strview stv_text, strview stv_pat, bool nocase);
```
反向搜索，在 `stv_text` 中查找最后一次出现 `stv_pat` 的位置，若 `stv_pat` 为空视图则返回 `stv_text.len`，  
若未找到则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

对于 `stv_pat`，`len > 4` 时使用 [`stv_rev_sundaySearch`](#stv_rev_sundaysearch)，否则使用 [`stv_rev_naiveSearch`](#stv_rev_naivesearch)。

示例：
```c
strview text = stv_literal("hello world hello");
size_t pos = stv_rev_search(text, stv_literal("hello"), false); // 12
```

| 参数       | 说明           |
|------------|----------------|
| `stv_text` | 文本视图       |
| `stv_pat`  | 模式视图       |
| `nocase`   | 是否忽略大小写 |

| 返回 | 说明                       |
|------|----------------------------|
| 索引 | 模式最后一次出现的起始位置 |

### `stv_rev_naiveSearch`
```c
size_t stv_rev_naiveSearch(strview stv_text, strview stv_pat, bool nocase);
```
反向朴素搜索，参数和返回值同 [`stv_rev_search`](#stv_rev_search)。

### `stv_rev_sundaySearch`
```c
size_t stv_rev_sundaySearch(strview stv_text, strview stv_pat, bool nocase);
```
反向 Sunday 搜索，参数和返回值同 [`stv_rev_search`](#stv_rev_search)。

---

## 比较

### `stv_firstDiff`
```c
size_t stv_firstDiff(strview stv_left, strview stv_right, bool nocase);
```
查找两个视图第一个不同字节的位置，若未找到（两个视图完全相同）则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若一个视图是另一个的前缀，返回较短视图的长度。

示例：
```c
size_t pos = stv_firstDiff(stv_literal("abc"), stv_literal("abx"), false); // 2
```

| 参数        | 说明           |
|-------------|----------------|
| `stv_left`  | 第一个视图     |
| `stv_right` | 第二个视图     |
| `nocase`    | 是否忽略大小写 |

| 返回 | 说明           |
|------|----------------|
| 索引 | 不同的位置索引 |

### `stv_lastDiff`
```c
size_t stv_lastDiff(strview stv_left, strview stv_right, bool nocase);
```
查找两个视图最后一个不同字节位置，若未找到（两个视图完全相同）则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若一个视图是另一个的后缀，则返回从右边开始第一个不同字节的索引。

示例：
```c
size_t pos = stv_lastDiff(stv_literal("hello"), stv_literal("hella"), false); // 4
```

| 参数        | 说明           |
|-------------|----------------|
| `stv_left`  | 第一个视图     |
| `stv_right` | 第二个视图     |
| `nocase`    | 是否忽略大小写 |

| 返回 | 说明                                |
|------|-------------------------------------|
| 索引 | 不同的位置索引（以较长的视图为基准）|

### `stv_compare`
```c
int stv_compare(strview stv_left, strview stv_right);
```
按字典序比较两个视图。

示例：
```c
int res = stv_compare(stv_literal("abc"), stv_literal("abd")); // < 0
```

| 返回 | 说明             |
|------|------------------|
| 负值 | `left` < `right` |
| 0    | 相等             |
| 正值 | `left` > `right` |

### `stv_compareNocase`
```c
int stv_compareNocase(strview stv_left, strview stv_right);
```
忽略大小写的字典序比较。参数与返回值规则同 [`stv_compare`](#stv_compare)。

示例：
```c
int res = stv_compareNocase(stv_literal("abc"), stv_literal("ABC")); // 0
```

### `stv_startsWith`
```c
bool stv_startsWith(strview stv_text, strview stv_pat);
```
判断 `stv_text` 是否以 `stv_pat` 开头。空模式始终返回 true。

示例：
```c
bool starts = stv_startsWith(stv_literal("hello world"), stv_literal("hello")); // true
```

| 返回  | 说明           |
|-------|----------------|
| true  | 以该前缀开头   |
| false | 不以该前缀开头 |

### `stv_startsWithNocase`
```c
bool stv_startsWithNocase(strview stv_text, strview stv_pat);
```
忽略大小写的前缀判断。参数与返回值规则同 [`stv_startsWith`](#stv_startswith)。

示例：
```c
bool starts = stv_startsWithNocase(stv_literal("Hello"), stv_literal("hello")); // true
```

### `stv_endsWith`
```c
bool stv_endsWith(strview stv_text, strview stv_pat);
```
判断 `stv_text` 是否以 `stv_pat` 结尾。空模式始终返回 true。

示例：
```c
bool ends = stv_endsWith(stv_literal("document.txt"), stv_literal(".txt")); // true
```

| 返回  | 说明           |
|-------|----------------|
| true  | 以该后缀结尾   |
| false | 不以该后缀结尾 |

### `stv_endsWithNocase`
```c
bool stv_endsWithNocase(strview stv_text, strview stv_pat);
```
忽略大小写的后缀判断。参数与返回值规则同 [`stv_endsWith`](#stv_endswith)。

示例：
```c
bool ends = stv_endsWithNocase(stv_literal("FILE.TXT"), stv_literal(".txt")); // true
```

### `stv_contains`
```c
bool stv_contains(strview stv_text, strview stv_sub);
```
判断 `stv_sub` 是否在 `stv_text` 中出现。空模式视为包含。内部调用 [`stv_search`](#stv_search)。

示例：
```c
bool found = stv_contains(stv_literal("hello world"), stv_literal("lo wo")); // true
```

| 返回  | 说明      |
|-------|-----------|
| true  | 包含子串  |
| false | 不包含    |

### `stv_containsNocase`
```c
bool stv_containsNocase(strview stv_text, strview stv_sub);
```
忽略大小写的字串判断。参数与返回值规则同 [`stv_contains`](#stv_contains)。

示例：
```c
bool found = stv_containsNocase(stv_literal("Hello World"), stv_literal("world")); // true
```

### `stv_equal`
```c
bool stv_equal(strview stv_left, strview stv_right);
```
比较两个视图内容是否完全一致。内部调用 [`stv_firstDiff`](#stv_firstdiff)。

示例：
```c
bool eq = stv_equal(stv_literal("hello"), stv_literal("hello")); // true
```

| 返回  | 说明     |
|-------|----------|
| true  | 内容相等 |
| false | 内容不同 |

### `stv_equalNocase`
```c
bool stv_equalNocase(strview stv_left, strview stv_right);
```
忽略大小写的内容相等判断。内部调用 [`stv_firstDiff`](#stv_firstdiff) 并开启 nocase。返回值规则同 [`stv_equal`](#stv_equal)。

示例：
```c
bool eq = stv_equalNocase(stv_literal("ABC"), stv_literal("abc")); // true
```

### `stv_same`
```c
bool stv_same(strview stv_left, strview stv_right);
```
判断两个视图是否引用完全相同的底层数据（指针相同且长度相同）。

示例：
```c
char buf[] = "data";
strview a = stv_makestv(buf, 4);
strview b = stv_makestv(buf, 4);
bool same = stv_same(a, b); // true
```

| 返回  | 说明       |
|-------|------------|
| true  | 同一块数据 |
| false | 不同数据   |

### `stv_empty`
```c
bool stv_empty(strview stv);
```
判断视图是否为空（`data == NULL` 或 `len == 0`）。

示例：
```c
bool empty = stv_empty(stv_nullstv); // true
```

| 返回  | 说明 |
|-------|------|
| true  | 为空 |
| false | 非空 |

---

## 计数 / 谓词

### `stv_count`（宏）
```c
#define stv_count(stv, target) /* ... */
```
统计满足条件的字符（或子串）数量。

C11 `_Generic` 宏。根据 `target` 类型分派：
- `stv_charClassFn` -> [`stv_countIf`](#stv_countif)
- `char` -> [`stv_countChar`](#stv_countchar)
- `strview` -> [`stv_countSubstr`](#stv_countsubstr)

示例：
```c
size_t n = stv_count(stv_literal("hello"), 'l'); // 2
size_t m = stv_count(stv_literal("ababa"), stv_literal("aba")); // 1
```

### `stv_countIf`
```c
size_t stv_countIf(strview stv, stv_charClassFn handle);
```
统计满足 `handle` 分类函数的字符数量，若视图为空或 `handle` 为 NULL 则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

> 有关字符分类函数，参见 [`stv_charClassFn`](#stv_charclassfn)

示例：
```c
size_t digits = stv_countIf(stv_literal("abc123"), isdigit); // 3
```

### `stv_countChar`
```c
size_t stv_countChar(strview stv, const char ch);
```
统计字符 `ch` 的出现次数，若视图为空则返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

示例：
```c
size_t cnt = stv_countChar(stv_literal("hello"), 'l'); // 2
```

### `stv_countSubstr`
```c
size_t stv_countSubstr(strview stv, strview sub);
```
统计非重叠子串 `sub` 的出现次数，若视图为空返回 [`stv_npos`](#stv_npos--stv_begin--stv_end宏)。

若 `sub` 为空视图返回 `stv.len`。

示例：
```c
size_t cnt = stv_countSubstr(stv_literal("abcabcdeabcabed"), stv_literal("abc")); // 3
```

### `stv_every`（宏）
```c
#define stv_every(stv, target) /* ... */
```
检查是否所有字符都满足条件。

C11 `_Generic` 宏。根据 `target` 类型分派：
- `stv_charClassFn` -> [`stv_everyIf`](#stv_everyif)
- `char` -> [`stv_everyChar`](#stv_everychar) 

示例：
```c
bool all_a = stv_every(stv_literal("aaa"), 'a'); // true
```

### `stv_everyIf`
```c
bool stv_everyIf(strview stv, stv_charClassFn handle);
```
检查是否所有字符都满足分类函数，若视图为空或 `handle` 为 NULL 则返回 false。内部调用 [`stv_countIf`](#stv_countif)。

示例：
```c
bool all_digits = stv_everyIf(stv_literal("12345"), isdigit); // true
```

### `stv_everyChar`
```c
bool stv_everyChar(strview stv, const char ch);
```
检查是否所有字符都等于 `ch`，若视图为空返回 false。内部调用 [`stv_countChar`](#stv_countchar)。

示例：
```c
bool all_a = stv_everyChar(stv_literal("aaaa"), 'a'); // true
```

### `stv_some`（宏）
```c
#define stv_some(stv, target) /* ... */
```
检查是否有任意字符满足条件。

C11 `_Generic` 宏。根据 `target` 类型分派：
- `stv_charClassFn` -> [`stv_someIf`](#stv_someif)。
- `char` -> [`stv_someChar`](#stv_somechar)

示例：
```c
bool has_digit = stv_some(stv_literal("abc1"), isdigit); // true
```

### `stv_someIf`
```c
bool stv_someIf(strview stv, stv_charClassFn handle);
```
检查是否有任意字符满足分类函数，若视图为空或 `handle` 为 NULL 则返回 false。内部调用 [`stv_countIf`](#stv_countif)。

示例：
```c
bool has_digit = stv_someIf(stv_literal("abc1"), isdigit); // true
```

### `stv_someChar`
```c
bool stv_someChar(strview stv, const char ch);
```
检查是否有任意字符等于 `ch`，若视图为空返回 false。内部调用 [`stv_countChar`](#stv_countchar)。

示例：
```c
bool has_e = stv_someChar(stv_literal("hello"), 'e'); // true
```

---

## 工具函数

### `stv_front`
```c
char stv_front(strview stv);
```
返回第一个字符；空视图返回 `'\0'`。

示例：
```c
char c = stv_front(stv_literal("Hello")); // 'H'
```

### `stv_back`
```c
char stv_back(strview stv);
```
返回最后一个字符；空视图返回 `'\0'`。

示例：
```c
char c = stv_back(stv_literal("Hello")); // 'o'
```

### `stv_at`
```c
char stv_at(strview stv, size_t idx);
```
返回索引 `idx` 处的字符；越界或空视图返回 `'\0'`。

示例：
```c
char c = stv_at(stv_literal("Hello"), 1); // 'e'
```

### `stv_forEach`
```c
void stv_forEach(strview stv, stv_forEachFn callback);
```
遍历视图中的每个字符，调用 `callback`。空视图或 NULL 回调时不执行。

> 回调函数指针类型定义查看 [`stv_forEachFn`](#stv_foreachfn)

示例：
```c
void print(char ch, size_t idx, strview ctx) {
  printf("%c", ch);
}
stv_forEach(stv_literal("abc"), print); // 输出 "abc"
```

### `stv_forEachRev`
```c
void stv_forEachRev(strview stv, stv_forEachFn callback);
```
反向遍历，参数和用法同 [`stv_forEach`](#stv_foreach)。

> 回调函数指针类型定义查看 [`stv_forEachFn`](#stv_foreachfn)

示例：
```c
stv_forEachRev(stv_literal("abc"), print); // 输出 "cba"
```

### `stv_swap`
```c
void stv_swap(strview* stv_left, strview* stv_right);
```
交换两个视图的内容。任一指针为 NULL 则无操作。

示例：
```c
strview a = stv_literal("first");
strview b = stv_literal("second");
stv_swap(&a, &b);
// a = "second", b = "first"
```

### `stv_hash`
```c
size_t stv_hash(strview stv);
```
计算哈希值，默认使用 FNV-1a 算法（调用 [`stv_hash_FNV1a`](#stv_hash_fnv1a)）。空视图返回 0。

示例：
```c
size_t h = stv_hash(stv_literal("hello"));
```

### `stv_hash_FNV1a`
```c
size_t stv_hash_FNV1a(strview stv);
```
FNV-1a 哈希，根据 `SIZE_MAX` 选择 16/32/64 位变体。平台不支持时返回 0。

示例：
```c
size_t h = stv_hash_FNV1a(stv_literal("hello"));
```

### `stv_PFARG / stv_PFFMT`（宏）
```c
#define stv_PFARG(stv) \
    (int)(stv_empty(stv) ? 0 : (stv).len > INT_MAX ? INT_MAX : (stv).len), (stv_empty(stv) ? "" : (stv).data)
#define stv_PFFMT "%.*s"
```
用于 `printf` 格式化视图。`stv_PFARG` 产生长度和指针参数，配合 `stv_PFFMT` 使用。长度超过 `INT_MAX` 时截断。

示例：
```c
printf("[" stv_PFFMT "]\n", stv_PFARG(myview));
```

### `stv_LIST`（宏）
```c
#define stv_LIST(...) ((strview[]){__VA_ARGS__}), (sizeof((strview[]){__VA_ARGS__}) / sizeof(strview))
```
创建视图数组并计算元素个数，用于 [`stv_opt_join`](#stv_opt_join)。

示例：
```c
strview sv1 = stv_literal("a"), sv2 = stv_literal("b");
stv_opt_join(stv_LIST(sv1, sv2), buf, sizeof(buf), sep, opts);
```

### `stv_npos / stv_begin / stv_end`（宏）
```c
#define stv_npos ((size_t)-1)   // 表示“未找到”
#define stv_begin (0)           // 起始索引 0
#define stv_end (stv_npos)      // 表示切片到末尾
```

---

## C 字符串转换

### `stv_cstr`
```c
char* stv_cstr(strview stv, char* mem, size_t size);
```
字符串输出。将视图内容写入到 `mem` 并附加空字符，缓冲区可用空间必须至少为 `stv.len + 1` 字节。

示例：
```c
char buf[6];
strview sv = stv_literal("   Hello   ");
stv_cstr(stv_trimChs(sv, stv_whitespace), buf, sizeof(buf));
// buf = "Hello"
```

| 参数   | 说明                     |
|--------|--------------------------|
| `stv`  | 源视图                   |
| `mem`  | 目标缓冲区（不能为 NULL）|
| `size` | 缓冲区大小（字节）       |

| 返回  | 说明                                     |
|-------|------------------------------------------|
| `mem` | 若缓冲区不足或 `mem` 为 NULL 则返回 NULL |

### `stv_opt_cstr`
```c
char* stv_opt_cstr(strview stv, char* mem, size_t size, stv_cstrOptions opts);
```
带转换选项的字符串输出。将视图内容写入到 `mem` 并附加空字符，缓冲区可用空间必须至少为 `stv.len + 1` 字节。

> 可用选项查看 [`stv_cstrOptions`](#stv_cstroptions)

示例：
```c
char buf[6];
stv_opt_cstr(stv_literal("Hello"), buf, 6, stv_Reverse | stv_ToUpper); // "OLLEH"
```

| 参数   | 说明                     |
|--------|--------------------------|
| `stv`  | 源视图                   |
| `mem`  | 目标缓冲区（不能为 NULL）|
| `size` | 缓冲区大小（字节）       |
| `opts` | 按位组合的转换选项       |

| 返回  | 说明                                     |
|-------|------------------------------------------|
| `mem` | 若缓冲区不足或 `mem` 为 NULL 则返回 NULL |

### `stv_opt_join`
```c
char* stv_opt_join(strview stv_arr[], size_t arr_len, char* mem, size_t size, strview sep, stv_cstrOptions opts);
```
带转换选项的多个字符串连接输出，字符串之间用分隔符 `sep` 连接。写入 `mem` 并在结尾附加一个空字符。

> 分隔符 `sep` 不受转换选项影响

示例：
```c
strview arr[] = {stv_literal("Hello"), stv_literal("World")};
char buf[20];
stv_opt_join(arr, 2, buf, sizeof(buf), stv_literal(", "), stv_Default);
// buf = "Hello, World"
```

| 参数      | 说明                                       |
|-----------|--------------------------------------------|
| `stv_arr` | 视图数组（仅在 `arr_len == 0` 时可为 NULL）|
| `arr_len` | 数组元素个数                               |
| `mem`     | 目标缓冲区（不能为 NULL）                  |
| `size`    | 缓冲区大小（字节）                         |
| `sep`     | 分隔符视图                                 |
| `opts`    | 应用于每个数组元素的转换选项               |

| 返回  | 说明                                     |
|-------|------------------------------------------|
| `mem` | 若缓冲区不足或 `mem` 为 NULL 则返回 NULL |

---

## 数值解析

### `stv_parseInum`
```c
intmax_t stv_parseInum(strview stv, int base, strview* remaining);
```
解析有符号整数，跳过前导空白，处理可选的 `+`/`-`，支持前导零。

支持基数为 2-36 进制或**根据进制前缀**自动检测（base=0）。

溢出时返回 `INTMAX_MAX`/`INTMAX_MIN` 并消耗全部数字。

> 进制前缀判定规则查看 [`stv_parseIntBase`](#stv_parseintbase)

示例：
```c
strview rem;
intmax_t val = stv_parseInum(stv_literal("-42"), 10, &rem);
// val = -42, rem = ""
```

| 参数        | 说明                           |
|-------------|--------------------------------|
| `stv`       | 输入视图                       |
| `base`      | 基数（2-36），0 为自动检测     |
| `remaining` | 输出剩余未解析部分（可为 NULL）|

| 返回   | 说明                   |
|--------|------------------------|
| 解析值 | 无数字或无效基数返回 0 |

### `stv_parseUnum`
```c
uintmax_t stv_parseUnum(strview stv, int base, strview* remaining);
```
解析无符号整数，跳过前导空白，处理可选的 `+`/`-`，支持前导0。

支持基数为 2-36 进制或**根据进制前缀**自动检测（base=0）。

负数按取模方式转换（如 `-40` → `UINTMAX_MAX - 39`），溢出时返回 `UINTMAX_MAX` 并消耗全部数字。

> 进制前缀判定规则查看 [`stv_parseIntBase`](#stv_parseintbase)

示例：
```c
strview rem;
uintmax_t val = stv_parseUnum(stv_literal("0xFF"), 0, &rem);
// val = 255, rem = ""
```

| 参数        | 说明                           |
|-------------|--------------------------------|
| `stv`       | 输入视图                       |
| `base`      | 基数（2-36），0 为自动检测     |
| `remaining` | 输出剩余未解析部分（可为 NULL）|

| 返回   | 说明                   |
|--------|------------------------|
| 解析值 | 无数字或无效基数返回 0 |

### `stv_ch2digit`
```c
int stv_ch2digit(char ch);
```
将 `[0-9A-Za-z]` 转换为 36 进制数字。无效字符返回 -1。

被 [`stv_parseInum`](#stv_parseinum) 和 [`stv_parseUnum`](#stv_parseunum) 内部使用。

示例：
```c
int d1 = stv_ch2digit('5');   // 5
int d2 = stv_ch2digit('B');   // 11
int d3 = stv_ch2digit('!');   // -1
```

| 参数 | 说明         |
|------|--------------|
| `ch` | 要转换的字符 |

| 返回 | 说明                  |
|------|-----------------------|
| 数字 | 0-36；无效数字返回 -1 |

### `stv_parseIntBase`
```c
int stv_parseIntBase(strview stv, strview* remaining);
```
检测并跳过进制前缀，返回检测到的基数，并通过 `remaining` 输出前缀后的数值部分。空视图返回 0。

若无可检测前缀则默认 10 进制，此时视图不消耗字符。

被 [`stv_parseInum`](#stv_parseinum) 和 [`stv_parseUnum`](#stv_parseunum) 内部使用。

进制前缀判定规则：
- `0B/0b` -> 2 进制
- `0O/0o` -> 8 进制
- `0D/0d` -> 10 进制
- `0X/0x` -> 16 进制

示例：
```c
strview rem;
int base = stv_parseIntBase(stv_literal("0xFF"), &rem);
// base = 16, rem = "FF"
```

| 参数        | 说明                         |
|-------------|------------------------------|
| `stv`       | 输入视图                     |
| `remaining` | 接受剩余部分输出（可为 NULL）|

| 返回 | 说明                    |
|------|-------------------------|
| 基数 | 2/8/10/16；空视图返回 0 |
