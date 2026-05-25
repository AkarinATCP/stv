# stv.h — C语言轻量级 String-View 库

> Language: **简体中文** | [English](./README.md)  

提供 `strview` 类型对字符串进行操作。  
支持 **C99** 和 **C++11** 及以上标准。  

## 特性

- Header-only 。  
- 只读字符串视图类型 `strview`，不影响源数据。
- 无须 `<stdlib.h>` 和 `<string.h>`，完全无动态内存分配。
- 所有函数均为纯函数，不依赖全局状态。
- 提供常见字符串操作：创建、切片、修剪、搜索、查找、比较等。

## 开始使用

```c
#define LIB_STV_IMPL    // 定义此宏以启用实现代码
#include "stv.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    strview myview = stv_new("This is a example string: Hello, world!");
    myview = stv_slice(myview, stv_lastChar(myview, 'H'), stv_end);  

    printf("myview: " stv_PFFMT "\n", stv_PFARG(myview));   // Hello, world!
    return 0;
}
```

## 集成方式

直接将 [stv.h](./include/stv.h) 复制到你的工程中并包含它。

然后在 **某一个** 源文件（如 `stv.c`）的 `#include` 前定义 `LIB_STV_IMPL`：

```c
// main.c
#include "stv.h"        // 仅包含声明

// stv.c
#define LIB_STV_IMPL    // 必须先定义宏再 include 以获得实现
#include "stv.h"
```

或在每次包含前定义 `LIB_STV_STATIC_INLINE_IMPL`，令所有函数都以 `static inline` 方式内联。

## 注意事项

> [!WARNING]
> `strview` 为只读视图，只包含源字符串的指针和长度信息，不拥有数据。  
> 这意味着必须保证源字符串的生命周期长于视图。

## API 概览

### 类型定义
| 类型              | 说明                                                              |
|-------------------|-------------------------------------------------------------------|
| `strview`         | 字符串视图类型，成员：`data`（const char*）、`len`（size_t）。    |
| `stv_charClassFn` | 字符分类函数指针类型：`int (*)(int)`. 用于 count/every/some 方法。|

### 创建视图
| 函数 / 宏                          | 说明                                                    |
|------------------------------------|---------------------------------------------------------|
| `stv_new(cstr)`                    | 从 C 字符串创建视图。                                   |
| `stv_create(str, endchar, maxlen)` | 从连续字符序列创建视图，需要指定终止字符和最大扫描长度。|
| `stv_makestv(data, len)`           |（宏）用指针和长度构造视图字面量。                       |
| `stv_nullstv`                      |（宏）预定义空视图常量。                                 |

### 切片
| 函数 / 宏                     | 说明                                   |
|-------------------------------|----------------------------------------|
| `stv_slice(stv, begin, end)`  | 获取 `[begin, end)` 切片的子视图。     |
| `stv_begin`                   |（宏）预定义视图起始位置常量，用于切片。|
| `stv_end`                     |（宏）预定义视图结束位置常量，用于切片。|

### 修剪
| 函数 / 宏                     | 说明                                   |
|-------------------------------|----------------------------------------|
| `stv_trim(stv, charset)`      | 去除两端属于 charset 的字符。          |
| `stv_trimStart(stv, charset)` | 去除开头属于 charset 的字符。          |
| `stv_trimEnd(stv, charset)`   | 去除结尾属于 charset 的字符。          |
| `stv_whitespace`              |（宏）预定义空白字符视图常量，用于修剪。|

### 搜索
| 函数                               | 说明                                       |
|------------------------------------|--------------------------------------------|
| `stv_search(text, pat)`            | 在 text 中搜索 pat 并返回第一次出现的位置。|
| `stv_naiveSearch(text, pat)`       | 朴素搜索算法实现。                         |
| `stv_sundaySearch(text, pat)`      | Sunday 搜索算法实现。                      |
| `stv_firstChar(stv, ch)`           | 第一个字符 ch 的位置。                     |
| `stv_lastChar(stv, ch)`            | 最后一个字符 ch 的位置。                   |
| `stv_firstNotChar(stv, ch)`        | 第一个不等于 ch 的字符位置。               |
| `stv_lastNotChar(stv, ch)`         | 最后一个不等于 ch 的字符位置。             |
| `stv_firstDiff(stv_a, stv_b)`      | 两个视图第一个不相同的字符位置。           |
| `stv_lastDiff(stv_a, stv_b)`       | 两个视图最后一个不相同的字符位置。         |

### 比较与检查
| 函数                           | 说明                            |
|--------------------------------|---------------------------------|
| `stv_compare(stv_a, stv_b)`    | 字典序比较两个视图。            |
| `stv_equal(stv_a, stv_b)`      | 逐字节判断内容是否相等。        |
| `stv_same(stv_a, stv_b)`       | 判断是否引用同一块内存区域。    |
| `stv_startsWith(text, prefix)` | 检查 text 是否以 prefix 为前缀。|
| `stv_endsWith(text, suffix)`   | 检查 text 是否以 suffix 为后缀。|
| `stv_contains(text, substr)`   | 检查 text 是否包含 substr 。    |
| `stv_empty(v)`                 | 检查视图是否为空。              |

### 计数与谓词
| 函数                     | 描述                                         |
|--------------------------|----------------------------------------------|
| `stv_count(stv, fn)`     | 统计通过分类函数 fn 的字符数量。             |
| `stv_countChar(stv, ch)` | 统计给定字符出现的次数。                     |
| `stv_every(stv, fn)`     | 如果所有字符都满足分类函数，则返回 true。    |
| `stv_everyChar(stv, ch)` | 如果所有字符都等于给定字符，则返回 true。    |
| `stv_some(stv, fn)`      | 如果至少有一个字符满足分类函数，则返回 true。|
| `stv_someChar(stv, ch)`  | 如果至少有一个字符等于给定字符，则返回 true。|

### 工具方法
| 函数 / 宏                      | 说明                               |
|--------------------------------|------------------------------------|
| `stv_front(stv)`               | 获取首字符。                       |
| `stv_back(stv)`                | 获取尾字符。                       |
| `stv_swap(&stv_a, &stv_b)`     | 交换两个视图。                     |
| `stv_cstr(stv, buf, size)`     | 复制视图内容到 C 字符串缓冲区。    |
| `stv_rev_cstr(stv, buf, size)` | 逆序复制视图内容到 C 字符串缓冲区。|
| `stv_PFFMT`                    |（宏）`printf` 格式化辅助宏         |
| `stv_PFARG(v)`                 |（宏）`printf` 格式化辅助宏。       |
| `stv_npos`                     |（宏）预定义哨兵值常量。            |

详细说明见 [`stv.h`](./include/stv.h) 内的文档注释。

## 许可证

MIT © 2026 Akarin <akarin@icatcp.work>  
完整许可证文本见[许可证](./LICENSE)文件。