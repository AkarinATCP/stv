# stv.h — C语言轻量级 String-View 库

> Language: **简体中文** | [English](./README.md)  

提供 `strview` 类型对字符串进行操作。  
支持 **C99** 和 **C++11** 及以上标准。  

## 特性

- Header-only 。  
- 只读字符串视图类型 `strview`，不影响源数据。
- 无须 `<stdlib.h>` 和 `<string.h>`，完全无动态内存分配。
- 所有函数均为纯函数，不依赖全局状态。
- 提供常用的字符串操作：创建、切片、修剪、搜索、查找、比较、计数、谓词等。 

## 开始使用

```c
#define LIB_STV_IMPL    // 定义此宏以启用实现代码
#include "stv.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    strview myview = stv_new("This is a example string: Hello, world!");
    myview = stv_slice(myview, stv_lastIndex(myview, 'H', false), stv_end);  

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

## API 参考

主要功能分组速览：

| 分类        | 关键函数 / 宏                                                                                         |
|-------------|-------------------------------------------------------------------------------------------------------|
| 创建        | `stv_new`, `stv_create`, `stv_literal`, `stv_makestv`, `stv_nullstv`                                  |
| 切片        | `stv_slice`, `stv_removeStart`, `stv_removeEnd`, `stv_removePrefix`, `stv_removeSuffix`               |
| 分割        | `stv_split`, `stv_splitLines`, `stv_splitWords`, `stv_beforeDelim`, `stv_afterDelim`                  |
| 修剪        | `stv_trim`, `stv_trimStart`, `stv_trimEnd`, `stv_whitespace`                                          |
| 搜索        | `stv_search`, `stv_rev_search`, `stv_firstIndex`, `stv_lastIndex`, `stv_firstDiff`, `stv_lastDiff`    |
| 比较        | `stv_compare`, `stv_equal`, `stv_same`, `stv_startsWith`, `stv_endsWith`, `stv_contains`, `stv_empty` |
| 计数 / 谓词 | `stv_count`, `stv_every`, `stv_some`                                                                  |
| 工具函数    | `stv_front`, `stv_back`, `stv_at`, `stv_swap`, `stv_cstr`, `stv_hash`, `stv_PFARG`, `stv_PFFMT`       |

> [!NOTE]
> 完整的参数说明、返回值及示例，请访问 [API Reference](docs/zh_CN/api_reference.md) 。

## 许可证

MIT © 2026 Akarin <akarin@icatcp.work>  
完整许可证文本见 [LICENSE](./LICENSE) 文件。
