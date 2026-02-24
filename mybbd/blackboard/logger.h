#pragma once

#include <iostream>

namespace bbd {

// 使用单一语句的宏，避免多线程打印穿插，并遵循 Google 规范不滥用 using
#define BBD_ERROR(msg)                                              \
  do {                                                              \
    std::cerr << "[BlackBoard] [" << __FUNCTION__ << ":"            \
              << __LINE__ << "] - " << msg << '\n';                 \
  } while (0)

}  // namespace bbd