/*******************************************************************************
Copyright (c) 2020-2030  Midea Co., Ltd 版权所有 1996-1998
PROPRIETARY RIGHTS of Midea Company are involved in the subject matter of this
material. All manufacturing,reproduction, use,and sales rights pertaining to
this subject matter are governed by the license agreement.The recipient of this
software implicitly accepts the terms of the license.
本软件文档资料是美的公司的资产,任何人士阅读和使用本资料必须获得相应的书面授权,承担保密
责任和接受相应的法律约束.
*******************************************************************************/

#pragma once

#include <sstream>
#include <iostream>

namespace bbd {

using namespace std;  // NOLINT

#define BBD_ERROR(...)                                                                                 \
  do {                                                                                                \
    std::ostringstream oss;                                                                            \
    oss << "[BlackBoard] [" << __FUNCTION__ << "][" << __LINE__ << "] - ";                        \
    oss << __VA_ARGS__;                                                                               \
    std::cout << oss.str() << std::endl;                                                              \
  } while (0)

}  // namespace bbd