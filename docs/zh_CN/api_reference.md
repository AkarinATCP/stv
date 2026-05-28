# API Reference

> Language: **简体中文** | [English](../en_US/api_reference.md)

*本文档提供 stv.h 中每个公开函数和宏的详细信息。*  
*快速概览和集成指南请参阅 [README](../../README.zh_CN.md)。*

## 目录

- [类型](#类型)
  - [strview](#strview)
  - [stv_charClassFn](#stv_charclassfn)
  - [stv_cstrOptions](#stv_cstroptions)
- [创建](#创建)
  - [stv_new](#stv_new)
  - [stv_create](#stv_create)
  - [stv_literal](#stv_literal)
  - [stv_makestv](#stv_makestv)
  - [stv_nullstv](#stv_nullstv)
- [切片](#切片)
  - [stv_slice](#stv_slice)
  - [stv_removeStart](#stv_removestart)
  - [stv_removeEnd](#stv_removeend)
  - [stv_removePrefix](#stv_removeprefix)
  - [stv_removeSuffix](#stv_removesuffix)
- [分割](#分割)
  - [stv_split](#stv_split)
  - [stv_splitLines](#stv_splitlines)
  - [stv_splitWords](#stv_splitwords)
  - [stv_beforeDelim](#stv_beforedelim)
  - [stv_afterDelim](#stv_afterdelim)
- [修剪](#修剪)
  - [泛型修剪宏](#泛型修剪宏)
    - [stv_trim](#stv_trim)
    - [stv_trimStart](#stv_trimstart)
    - [stv_trimEnd](#stv_trimend)
  - [基于字符集的修剪](#基于字符集的修剪)
    - [stv_trimChs](#stv_trimchs)
    - [stv_trimStartChs](#stv_trimstartchs)
    - [stv_trimEndChs](#stv_trimendchs)
  - [基于分类函数的修剪](#基于分类函数的修剪)
    - [stv_trimIf](#stv_trimif)
    - [stv_trimStartIf](#stv_trimstartif)
    - [stv_trimEndIf](#stv_trimendif)
- [搜索](#搜索)
  - [stv_search](#stv_search)
  - [stv_naiveSearch](#stv_naivesearch)
  - [stv_sundaySearch](#stv_sundaysearch)
- [逆向搜索](#逆向搜索)
  - [stv_rev_search](#stv_rev_search)
  - [stv_rev_naiveSearch](#stv_rev_naivesearch)
  - [stv_rev_sundaySearch](#stv_rev_sundaysearch)
- [首尾索引查询](#首尾索引查询)
  - [泛型宏](#泛型宏)
    - [stv_firstIndex](#stv_firstindex)
    - [stv_lastIndex](#stv_lastindex)
  - [字符查找](#字符查找)
    - [stv_firstChar](#stv_firstchar)
    - [stv_lastChar](#stv_lastchar)
  - [字符集查找](#字符集查找)
    - [stv_firstCharset](#stv_firstcharset)
    - [stv_lastCharset](#stv_lastcharset)
  - [分类函数查找](#分类函数查找)
    - [stv_firstCharClass](#stv_firstcharclass)
    - [stv_lastCharClass](#stv_lastcharclass)
- [差异检测](#差异检测)
  - [stv_firstDiff](#stv_firstdiff)
  - [stv_lastDiff](#stv_lastdiff)
- [计数](#计数)
  - [泛型宏](#泛型宏-1)
    - [stv_count](#stv_count)
  - [stv_countIf](#stv_countif)
  - [stv_countChar](#stv_countchar)
  - [stv_countSubstr](#stv_countsubstr)
- [谓词判断](#谓词判断)
  - [泛型宏](#泛型宏-2)
    - [stv_every](#stv_every)
    - [stv_some](#stv_some)
  - [stv_everyIf](#stv_everyif)
  - [stv_everyChar](#stv_everychar)
  - [stv_someIf](#stv_someif)
  - [stv_someChar](#stv_somechar)
- [比较与检查](#比较与检查)
  - [stv_compare](#stv_compare)
  - [stv_equal](#stv_equal)
  - [stv_same](#stv_same)
  - [stv_startsWith](#stv_startswith)
  - [stv_endsWith](#stv_endswith)
  - [stv_contains](#stv_contains)
  - [stv_empty](#stv_empty)
- [工具方法](#工具方法)
  - [stv_front](#stv_front)
  - [stv_back](#stv_back)
  - [stv_at](#stv_at)
  - [stv_swap](#stv_swap)
  - [stv_hash](#stv_hash)
  - [stv_hash_FNV1a](#stv_hash_fnv1a)
  - [stv_cstr](#stv_cstr)
  - [stv_opt_cstr](#stv_opt_cstr)
  - [stv_PFARG / stv_PFFMT](#stv_pfarg--stv_pffmt)
- [宏与常量](#宏与常量)
  - [stv_begin](#stv_begin)
  - [stv_end](#stv_end)
  - [stv_npos](#stv_npos)
  - [stv_whitespace](#stv_whitespace)

---

## 类型

### strview
```c
typedef struct stv_strview_t strview;
struct stv_strview_t {
    const char* data;
    size_t      len;
};
```
字符串视图类型。表示字符串的只读片段，不以空字符结尾，且不拥有数据的所有权。

**成员：**
- `data` : 指向首个字符的指针。
- `len` : 视图的字节长度。

### stv_charClassFn
```c
typedef int (*stv_charClassFn)(int);
```
字符分类函数指针类型。应表现为标准 `is*` 系列函数（如 `isdigit`、`isspace`）——若字符属于该类则返回非零值，否则返回零。

### stv_cstrOptions
```c
typedef enum {
    stv_Default = 0,  // 0b00000000
    stv_ToUpper = 1,  // 0b00000001
    stv_ToLower = 2,  // 0b00000010
    stv_Reverse = 4,  // 0b00000100
} stv_cstrOptions;
```
`stv_opt_cstr` 的按位组合选项，用于控制输出转换。

**枚举值：**
- `stv_Default` (0)：无转换。
- `stv_ToUpper` (1)：通过 ASCII 掩码转换为大写。
- `stv_ToLower` (2)：通过 ASCII 掩码转换为小写。
- `stv_Reverse` (4)：反转字符顺序。

多个选项可通过 `|` 组合。若同时设置 `stv_ToUpper` 与 `stv_ToLower`，结果为小写。

---

## 创建

### stv_new
```c
strview stv_new(const char* c_str);
```
从 C 字符串创建视图（直到空终止符）。

**参数：**
- `c_str` : 指向以空字符结尾的 C 字符串的指针，可为 `NULL`。

**返回值：**
- 从 `c_str` 到空终止符的 `strview`。若 `c_str` 为 `NULL`，则返回空视图（[`stv_nullstv`](#stv_nullstv)）。

**详情：**
- 等价于 `stv_create(c_str, '\0', stv_npos)`。

### stv_create
```c
strview stv_create(const char* str, unsigned char endchar, size_t maxlen);
```
从连续字符序列创建视图，在指定终止字符或最大长度处停止。

**参数：**
- `str` : 源字符串指针，可为 `NULL`。
- `endchar` : 终止字符（在此字符前停止，该字符本身不被包含）。
- `maxlen` : 最大扫描字节数。

**返回值：**
- 从 `str` 开始直到首次遇到 `endchar` 或 `maxlen` 字节（以先到者为准）的 `strview`。若 `str` 为 `NULL`，则返回空视图。

### stv_literal
```c
#define stv_literal(str) stv_makestv((str), sizeof(str) - 1)
```
用字符串字面量创建视图的宏。长度在编译期通过 `sizeof` 计算。

### stv_makestv
```c
#ifdef __cplusplus
    #define stv_makestv(data_v, len_v) (strview{/* .data= */ (data_v), /* .len= */ (len_v)})
#else
    #define stv_makestv(data_v, len_v) ((strview){.data = (data_v), .len = (len_v)})
#endif
```
用指针和长度构造 `strview` 的宏。

### stv_nullstv
```c
#define stv_nullstv stv_makestv(nullptr, 0)
```
预定义的空视图，其 `data = nullptr` 且 `len = 0`。

---

## 切片

### stv_slice
```c
strview stv_slice(strview stv, size_t begin_pos, size_t end_pos);
```
提取覆盖 `[begin_pos, end_pos)` 范围的子视图。

**参数：**
- `stv` : 源视图。
- `begin_pos` : 起始索引（包含）。
- `end_pos` : 结束索引（不包含）。可使用 [`stv_end`](#stv_end) 表示直到末尾。

**返回值：**
- 若范围有效，返回子视图；否则返回空视图。

**详情：**
- 若 `end_pos` 为 `stv_npos`（即 `stv_end`），则按 `stv.len` 处理。
- 当 `begin_pos > end_pos`、`begin_pos > stv.len` 或 `data` 为 `NULL` 时返回空视图。

### stv_removeStart
```c
strview stv_removeStart(strview stv, size_t len);
```
从开头移除 `len` 个字节。等价于 `stv_slice(stv, len, stv_end)`。

**参数：**
- `stv` : 源视图。
- `len` : 要移除的字节数。

**返回值：**
- 去掉前 `len` 字节的视图，若 `len >= stv.len` 则返回空视图。

### stv_removeEnd
```c
strview stv_removeEnd(strview stv, size_t len);
```
从末尾移除 `len` 个字节。等价于 `stv_slice(stv, stv_begin, stv.len - len)`。

**参数：**
- `stv` : 源视图。
- `len` : 要移除的字节数。

**返回值：**
- 去掉后 `len` 字节的视图，若 `len >= stv.len` 则返回空视图。

### stv_removePrefix
```c
strview stv_removePrefix(strview stv, strview prefix);
```
若视图以 `prefix` 开头，则将其移除；否则原样返回。

**参数：**
- `stv` : 源视图。
- `prefix` : 要移除的前缀。空前缀视为匹配，返回原视图。

**返回值：**
- 去除前缀后的视图，或原视图（若不匹配）。

### stv_removeSuffix
```c
strview stv_removeSuffix(strview stv, strview suffix);
```
若视图以 `suffix` 结尾，则将其移除；否则原样返回。

**参数：**
- `stv` : 源视图。
- `suffix` : 要移除的后缀。空后缀视为匹配，返回原视图。

**返回值：**
- 去除后缀后的视图，或原视图（若不匹配）。

---

## 分割

### stv_split
```c
strview stv_split(strview stv, strview sep, strview* remaining);
```
在分隔符首次出现处分割。返回分隔符之前的部分，剩余部分存入 `*remaining`（若非 `NULL`）。

**参数：**
- `stv` : 待分割的视图。
- `sep` : 分隔符视图。若为空，则按字符逐个分割（返回第一个字符，剩余部分存入 `*remaining`）。
- `remaining` : 可选的输出指针，用于接收分隔符之后的部分。若为 `NULL`，则丢弃剩余部分。

**返回值：**
- 分隔符之前的部分，若未找到分隔符则返回整个视图。

**详情：**
- 当 `stv` 为空时，返回空视图且不修改 `*remaining`。

### stv_splitLines
```c
strview stv_splitLines(strview stv, strview* remaining);
```
在首个换行符处分割（支持 CR、LF 或 CRLF）。返回的行不包含换行符。

**参数：**
- `stv` : 待分割的视图。
- `remaining` : 可选的输出指针，用于接收换行符之后的部分。若为 `NULL`，则丢弃。

**返回值：**
- 当前行，若无换行符则返回整个视图。

**详情：**
- 若 `stv` 为空，返回空视图且不修改 `*remaining`。
- 支持将 Windows 风格的 CRLF 视为单个换行符。

### stv_splitWords
```c
strview stv_splitWords(strview stv, strview* remaining);
```
跳过前导空白字符，然后提取下一个连续的非空白字符序列作为单词。

**参数：**
- `stv` : 待分割的视图。
- `remaining` : 可选的输出指针，用于接收单词之后的部分。若为 `NULL`，则丢弃。

**返回值：**
- 当前单词，若未找到单词（空视图或全为空白）则返回空视图。

**详情：**
- 空白字符定义见 [`stv_whitespace`](#stv_whitespace)。
- 当 `stv` 为空时，不修改 `*remaining`。

### stv_beforeDelim
```c
strview stv_beforeDelim(strview stv, strview delim);
```
返回分隔符首次出现之前的部分。

**参数：**
- `stv` : 待检查的视图。
- `delim` : 分隔符视图。若为空，返回空视图。

**返回值：**
- 分隔符之前的部分；若未找到分隔符则返回整个视图；若 `delim` 为空则返回空视图。

### stv_afterDelim
```c
strview stv_afterDelim(strview stv, strview delim);
```
返回分隔符首次出现之后的部分。

**参数：**
- `stv` : 待检查的视图。
- `delim` : 分隔符视图。若为空，返回整个视图不变。

**返回值：**
- 分隔符之后的部分；若未找到分隔符则返回空视图。

---

## 修剪

### 泛型修剪宏

#### stv_trim
```c
#define stv_trim(stv, target) _Generic((target), strview: stv_trimChs, stv_charClassFn: stv_trimIf)((stv), (target))
```
根据 `target` 的类型分派到 `stv_trimChs` 或 `stv_trimIf`。  
若 `target` 为 `strview`，视为字符集；若为 `stv_charClassFn`，则作为分类函数使用。

#### stv_trimStart
```c
#define stv_trimStart(stv, target) _Generic((target), strview: stv_trimStartChs, stv_charClassFn: stv_trimStartIf)((stv), (target))
```
分派到 `stv_trimStartChs` 或 `stv_trimStartIf`。仅修剪开头。

#### stv_trimEnd
```c
#define stv_trimEnd(stv, target) _Generic((target), strview: stv_trimEndChs, stv_charClassFn: stv_trimEndIf)((stv), (target))
```
分派到 `stv_trimEndChs` 或 `stv_trimEndIf`。仅修剪末尾。

### 基于字符集的修剪

#### stv_trimChs
```c
strview stv_trimChs(strview stv, strview charset);
```
从两端移除 `charset` 中出现的所有字符。

**参数：**
- `stv` : 源视图。
- `charset` : 字符集视图。若为空，返回原视图不变。

**返回值：**
- 修剪后的视图。

#### stv_trimStartChs
```c
strview stv_trimStartChs(strview stv, strview charset);
```
仅从开头移除 `charset` 中的字符。

**参数：**
- `stv` : 源视图。
- `charset` : 字符集视图。

**返回值：**
- 修剪后的视图。

#### stv_trimEndChs
```c
strview stv_trimEndChs(strview stv, strview charset);
```
仅从末尾移除 `charset` 中的字符。

**参数：**
- `stv` : 源视图。
- `charset` : 字符集视图。

**返回值：**
- 修剪后的视图。

### 基于分类函数的修剪

#### stv_trimIf
```c
strview stv_trimIf(strview stv, stv_charClassFn handle);
```
从两端移除满足分类函数的字符。

**参数：**
- `stv` : 源视图。
- `handle` : 分类函数。若为 `NULL`，则不执行修剪。

**返回值：**
- 修剪后的视图。

#### stv_trimStartIf
```c
strview stv_trimStartIf(strview stv, stv_charClassFn handle);
```
类似 `stv_trimIf`，但仅修剪开头。

**参数：**
- `stv` : 源视图。
- `handle` : 分类函数。

**返回值：**
- 修剪后的视图。

#### stv_trimEndIf
```c
strview stv_trimEndIf(strview stv, stv_charClassFn handle);
```
类似 `stv_trimIf`，但仅修剪末尾。

**参数：**
- `stv` : 源视图。
- `handle` : 分类函数。

**返回值：**
- 修剪后的视图。

---

## 搜索

### stv_search
```c
size_t stv_search(strview stv_text, strview stv_pat);
```
在 `stv_text` 中搜索 `stv_pat` 的首次出现。自动选择算法（短模式用朴素算法，否则用 Sunday）。

**参数：**
- `stv_text` : 待搜索文本。
- `stv_pat` : 要搜索的模式。

**返回值：**
- 首次匹配的起始索引；若模式为空则返回 `0`；未找到返回 [`stv_npos`](#stv_npos)。

### stv_naiveSearch
```c
size_t stv_naiveSearch(strview stv_text, strview stv_pat);
```
朴素（暴力）子串搜索。

**参数：**
- `stv_text` : 待搜索文本。
- `stv_pat` : 模式。

**返回值：**
- 首次匹配的起始索引；模式为空返回 `0`；未找到或文本过短返回 `stv_npos`。

### stv_sundaySearch
```c
size_t stv_sundaySearch(strview stv_text, strview stv_pat);
```
Sunday 算法子串搜索，长模式通常更快。

**参数：**
- `stv_text` : 待搜索文本。
- `stv_pat` : 模式。

**返回值：**
- 首次匹配的起始索引；模式为空返回 `0`；否则未找到返回 `stv_npos`。

---

## 逆向搜索

### stv_rev_search
```c
size_t stv_rev_search(strview stv_text, strview stv_pat);
```
搜索 `stv_pat` 的最后一次出现。自动选择算法。

**参数：**
- `stv_text` : 待搜索文本。
- `stv_pat` : 模式。

**返回值：**
- 最后匹配的起始索引；模式为空返回 `stv_text.len`；未找到返回 `stv_npos`。

### stv_rev_naiveSearch
```c
size_t stv_rev_naiveSearch(strview stv_text, strview stv_pat);
```
朴素逆向搜索。

**参数：**
- `stv_text` : 待搜索文本。
- `stv_pat` : 模式。

**返回值：**
- 最后匹配的起始索引；模式为空返回 `stv_text.len`；未找到返回 `stv_npos`。

### stv_rev_sundaySearch
```c
size_t stv_rev_sundaySearch(strview stv_text, strview stv_pat);
```
适用于逆向搜索的 Sunday 算法。

**参数：**
- `stv_text` : 待搜索文本。
- `stv_pat` : 模式。

**返回值：**
- 最后匹配的起始索引；模式为空返回 `stv_text.len`；未找到返回 `stv_npos`。

---

## 首尾索引查询

### 泛型宏

#### stv_firstIndex
```c
#define stv_firstIndex(stv, target, invert) _Generic((target), char: stv_firstChar, strview: stv_firstCharset, stv_charClassFn: stv_firstCharClass)((stv), (target), (invert))
```
根据 `target` 的类型分派到合适的首索引函数。若 `invert` 为 `true`，则查找**不匹配**的首个字符。

#### stv_lastIndex
```c
#define stv_lastIndex(stv, target, invert) _Generic((target), char: stv_lastChar, strview: stv_lastCharset, stv_charClassFn: stv_lastCharClass)((stv), (target), (invert))
```
分派到合适的尾索引函数。若 `invert` 为 `true`，则查找**不匹配**的最后一个字符。

### 字符查找

#### stv_firstChar
```c
size_t stv_firstChar(strview stv, const char ch, bool invert);
```
查找 `ch` 首次出现的位置（或若 `invert` 为真，则查找首个不等于 `ch` 的位置）。

**参数：**
- `stv` : 待搜索视图。
- `ch` : 要定位的字符。
- `invert` : 若为 `true`，则查找首个**不等于** `ch` 的字符。

**返回值：**
- 匹配索引，若未找到或视图为空则返回 `stv_npos`。

#### stv_lastChar
```c
size_t stv_lastChar(strview stv, const char ch, bool invert);
```
查找 `ch` 最后一次出现的位置（或若 `invert` 为真，则查找最后一个不等于 `ch` 的位置）。

**参数：**
- `stv` : 待搜索视图。
- `ch` : 要定位的字符。
- `invert` : 若为 `true`，则查找最后一个**不等于** `ch` 的字符。

**返回值：**
- 匹配索引，若未找到或视图为空则返回 `stv_npos`。

### 字符集查找

#### stv_firstCharset
```c
size_t stv_firstCharset(strview stv, strview charset, bool invert);
```
查找首个属于（或若 `invert` 为真则首个不属于）`charset` 的字符。

**参数：**
- `stv` : 待搜索视图。
- `charset` : 字符集视图。若为空，始终返回 `stv_npos`。
- `invert` : 若为 `true`，则查找首个**不在**字符集中的字符。

**返回值：**
- 匹配索引，或 `stv_npos`。

#### stv_lastCharset
```c
size_t stv_lastCharset(strview stv, strview charset, bool invert);
```
查找最后一个属于（或不属于）`charset` 的字符。

**参数：**
- `stv` : 待搜索视图。
- `charset` : 字符集视图。
- `invert` : 若为 `true`，则查找最后一个**不在**字符集中的字符。

**返回值：**
- 匹配索引，或 `stv_npos`。

### 分类函数查找

#### stv_firstCharClass
```c
size_t stv_firstCharClass(strview stv, stv_charClassFn handle, bool invert);
```
查找首个满足 `handle`（或若 `invert` 为真则不满足）的字符。

**参数：**
- `stv` : 待搜索视图。
- `handle` : 分类函数。若为 `NULL` 或视图为空，返回 `stv_npos`。
- `invert` : 若为 `true`，则查找首个**不**满足分类函数的字符。

**返回值：**
- 匹配索引，或 `stv_npos`。

#### stv_lastCharClass
```c
size_t stv_lastCharClass(strview stv, stv_charClassFn handle, bool invert);
```
查找最后一个满足 `handle`（或不满足）的字符。

**参数：**
- `stv` : 待搜索视图。
- `handle` : 分类函数。
- `invert` : 若为 `true`，则查找最后一个**不**满足的字符。

**返回值：**
- 匹配索引，或 `stv_npos`。

---

## 差异检测

### stv_firstDiff
```c
size_t stv_firstDiff(strview stv_left, strview stv_right);
```
找出两个视图首个不同的字节位置（从左到右）。

**参数：**
- `stv_left` : 左视图。
- `stv_right` : 右视图。

**返回值：**
- 首个差异索引；若完全相同则返回 `stv_npos`。

**详情：**
- 若一者是另一者的前缀，返回较短视图的长度。
- 若一者为空而另一者非空，返回 `0`。

### stv_lastDiff
```c
size_t stv_lastDiff(strview stv_left, strview stv_right);
```
找出两个视图最后一个不同的字节位置（从右到左）。

**参数：**
- `stv_left` : 左视图。
- `stv_right` : 右视图。

**返回值：**
- 最后一个差异（从开头算起）的索引；若完全相同则返回 `stv_npos`。

---

## 计数

### 泛型宏

#### stv_count
```c
#define stv_count(stv, target) _Generic((target), char: stv_countChar, strview: stv_countSubstr, stv_charClassFn: stv_countIf)((stv), (target))
```
根据 `target` 的类型分派到适当的计数函数。

### stv_countIf
```c
size_t stv_countIf(strview stv, stv_charClassFn handle);
```
统计满足分类函数的字符个数。

**参数：**
- `stv` : 待检查视图。
- `handle` : 分类函数。若为 `NULL` 或视图为空，返回 `stv_npos`。

**返回值：**
- 匹配的字符数，或 `stv_npos`。

### stv_countChar
```c
size_t stv_countChar(strview stv, const char ch);
```
统计特定字符的出现次数。

**参数：**
- `stv` : 待检查视图。
- `ch` : 要计数的字符。

**返回值：**
- 出现次数；若视图为空返回 `stv_npos`。

### stv_countSubstr
```c
size_t stv_countSubstr(strview stv, strview sub);
```
统计子串的不重叠出现次数。

**参数：**
- `stv` : 待搜索视图。
- `sub` : 要计数的子串。若为空，返回 `stv.len`。

**返回值：**
- 不重叠出现次数；若 `stv` 为空返回 `stv_npos`。

---

## 谓词判断

### 泛型宏

#### stv_every
```c
#define stv_every(stv, target) _Generic((target), char: stv_everyChar, stv_charClassFn: stv_everyIf)((stv), (target))
```
分派到 `stv_everyChar` 或 `stv_everyIf`。若所有字符均满足条件，返回 `true`。

#### stv_some
```c
#define stv_some(stv, target) _Generic((target), char: stv_someChar, stv_charClassFn: stv_someIf)((stv), (target))
```
分派到 `stv_someChar` 或 `stv_someIf`。若至少有一个字符满足条件，返回 `true`。

### stv_everyIf
```c
bool stv_everyIf(strview stv, stv_charClassFn handle);
```
检查所有字符是否都满足分类函数。

**参数：**
- `stv` : 待检查视图。
- `handle` : 分类函数。若为 `NULL` 或视图为空，返回 `false`。

**返回值：**
- 若所有字符均匹配则返回 `true`；否则返回 `false`。

### stv_everyChar
```c
bool stv_everyChar(strview stv, const char ch);
```
检查所有字符是否都等于给定字符。

**参数：**
- `stv` : 待检查视图。
- `ch` : 要比较的字符。

**返回值：**
- 若非空且所有字符均等于 `ch` 则返回 `true`；否则返回 `false`。

### stv_someIf
```c
bool stv_someIf(strview stv, stv_charClassFn handle);
```
检查是否至少有一个字符满足分类函数。

**参数：**
- `stv` : 待检查视图。
- `handle` : 分类函数。若为 `NULL` 或视图为空，返回 `false`。

**返回值：**
- 若有任何字符匹配则返回 `true`；否则返回 `false`。

### stv_someChar
```c
bool stv_someChar(strview stv, const char ch);
```
检查给定字符是否至少出现一次。

**参数：**
- `stv` : 待检查视图。
- `ch` : 要查找的字符。

**返回值：**
- 若非空且包含 `ch` 则返回 `true`；否则返回 `false`。

---

## 比较与检查

### stv_compare
```c
int stv_compare(strview stv_left, strview stv_right);
```
字典序比较两个视图。

**参数：**
- `stv_left` : 左视图。
- `stv_right` : 右视图。

**返回值：**
- 若左 < 右则返回负值，相等返回零，左 > 右返回正值。

### stv_equal
```c
bool stv_equal(strview stv_left, strview stv_right);
```
逐字节内容相等判断。

**参数：**
- `stv_left` : 左视图。
- `stv_right` : 右视图。

**返回值：**
- 若内容相同返回 `true`；否则返回 `false`。

### stv_same
```c
bool stv_same(strview stv_left, strview stv_right);
```
检查两个视图是否引用完全相同的存储区域（相同的指针和长度）。

**参数：**
- `stv_left` : 左视图。
- `stv_right` : 右视图。

**返回值：**
- 若 `data` 和 `len` 均相等返回 `true`；否则返回 `false`。

### stv_startsWith
```c
bool stv_startsWith(strview stv_text, strview stv_pat);
```
检查 `stv_text` 是否以 `stv_pat` 开头。

**参数：**
- `stv_text` : 待检查文本。
- `stv_pat` : 前缀模式。空模式始终返回 `true`。

**返回值：**
- 若文本以模式开头返回 `true`；否则返回 `false`。

### stv_endsWith
```c
bool stv_endsWith(strview stv_text, strview stv_pat);
```
检查 `stv_text` 是否以 `stv_pat` 结尾。

**参数：**
- `stv_text` : 待检查文本。
- `stv_pat` : 后缀模式。空模式始终返回 `true`。

**返回值：**
- 若文本以模式结尾返回 `true`；否则返回 `false`。

### stv_contains
```c
bool stv_contains(strview stv_text, strview stv_sub);
```
检查 `stv_text` 是否包含 `stv_sub`。

**参数：**
- `stv_text` : 待检查文本。
- `stv_sub` : 要查找的子串。空子串视为包含。

**返回值：**
- 若模式出现返回 `true`；否则返回 `false`。

### stv_empty
```c
bool stv_empty(strview stv);
```
检查视图是否为空。

**参数：**
- `stv` : 待检查视图。

**返回值：**
- 若 `data` 为 `NULL` 或 `len` 为 `0` 返回 `true`；否则返回 `false`。

---

## 工具方法

### stv_front
```c
char stv_front(strview stv);
```
获取首个字符。

**参数：**
- `stv` : 待检查视图。

**返回值：**
- 首个字符，若视图为空返回 `'\0'`。

### stv_back
```c
char stv_back(strview stv);
```
获取最后一个字符。

**参数：**
- `stv` : 待检查视图。

**返回值：**
- 最后一个字符，若视图为空返回 `'\0'`。

### stv_at
```c
char stv_at(strview stv, size_t idx);
```
访问指定索引处的字符。

**参数：**
- `stv` : 待检查视图。
- `idx` : 从零开始的索引。

**返回值：**
- `idx` 处的字符，若越界或视图为空返回 `'\0'`。

### stv_swap
```c
void stv_swap(strview* stv_left, strview* stv_right);
```
交换两个视图的内容。

**参数：**
- `stv_left` : 第一个视图的指针。
- `stv_right` : 第二个视图的指针。若任一为 `NULL`，则无操作。

**返回值：**
- 无。

### stv_hash
```c
size_t stv_hash(strview stv);
```
计算视图内容的哈希值。

**参数：**
- `stv` : 要哈希的视图。

**返回值：**
- 哈希值；若视图为空则返回 `0`。

### stv_hash_FNV1a
```c
size_t stv_hash_FNV1a(strview stv);
```
直接的 FNV‑1a 哈希实现。根据 `size_t` 宽度选择 16/32/64 位参数。

**参数：**
- `stv` : 要哈希的视图。

**返回值：**
- 哈希值；若视图为空或不支持的平台宽度返回 `0`。

### stv_cstr
```c
char* stv_cstr(strview stv, char* mem, size_t size);
```
将视图内容复制到缓冲区并追加空终止符。

**参数：**
- `stv` : 源视图。
- `mem` : 目标缓冲区。
- `size` : 缓冲区字节大小。

**返回值：**
- 若缓冲区足够大（`size > stv.len`）返回 `mem`；否则返回 `NULL`。视图为空时写入空字符串。

### stv_opt_cstr
```c
char* stv_opt_cstr(strview stv, char* mem, size_t size, stv_cstrOptions opts);
```
复制视图内容并可选择应用转换（大小写、反转）。

**参数：**
- `stv` : 源视图。
- `mem` : 目标缓冲区。
- `size` : 缓冲区大小。
- `opts` : `stv_cstrOptions` 的按位组合。

**返回值：**
- 若缓冲区足够大返回 `mem`；否则返回 `NULL`。

### stv_PFARG / stv_PFFMT
```c
#define stv_PFARG(stv) (int)(...), (...)
#define stv_PFFMT       "%.*s"
```
用于 `printf` 风格格式化字符串视图的辅助宏。

**用法：**
```c
printf("data: " stv_PFFMT "\n", stv_PFARG(myview));
```
- 若视图长度超过 `INT_MAX`，长度将被截断。
- 对于空视图，传入空字符串和长度 0。

---

## 宏与常量

### stv_begin
```c
#define stv_begin (0)
```
切片起始位置常量。

### stv_end
```c
#define stv_end (stv_npos)
```
切片结束位置常量（表示“直至末尾”）。

### stv_npos
```c
#define stv_npos ((size_t)-1)
```
搜索/索引函数返回的哨兵值，表示“未找到”。

### stv_whitespace
```c
#define stv_whitespace stv_literal(" \r\n\t\v\f")
```
包含常见空白字符（空格、CR、LF、制表符、垂直制表符、换页符）的预定义视图，常用于修剪和分割。
```