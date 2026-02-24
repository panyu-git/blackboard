/*******************************************************************************
  Copyright (c) 2020-2030  Midea Co., Ltd 版权所有 2020-2030
  PROPRIETARY RIGHTS of Midea Company are involved in the subject matterof this
  material. All manufacturing,reproduction, use,and sales rights pertaining to
  this subject matter aregoverned by the license agreement.The recipient of this
  software implicitlyaccepts the terms of the license.
  本软件文档资料是美的公司的资产,任何人士阅读和使用本资料必须获得相应的书面授权,承担保密
  责任和接受相应的法律约束.
 *******************************************************************************/
#pragma once
#include <memory>
#include <string>
#include <tuple>

#include <boost/any.hpp>
#include "message.h"

namespace bbd {
class FunctionBase {
 public:
  FunctionBase() = default;
  virtual ~FunctionBase() = default;
  virtual boost::any invoke(const std::string& name,
                            const std::shared_ptr<Message>& msg) {
    return boost::any{};
  }
};
}  // namespace bbd