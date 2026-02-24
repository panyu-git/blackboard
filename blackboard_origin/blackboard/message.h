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
#include <iostream>
#include <utility>

#include <boost/any.hpp>
#include <boost/type_index.hpp>

namespace bbd {
class Message {
 public:
  template <typename Tp>
  explicit Message(Tp&& tp) : m_tuple(boost::any(std::forward<Tp>(tp))) {
  }
  ~Message() = default;
  boost::any get_tuple() {
    return m_tuple;
  }
  const boost::typeindex::type_info& get_type() {
    return m_tuple.type();
  }

 private:
  boost::any m_tuple;
};
}  // namespace bbd