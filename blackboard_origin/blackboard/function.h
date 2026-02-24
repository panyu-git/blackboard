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
#include <utility>

#include <boost/type_traits.hpp>

#include "function_base.h"
#include "function_help.h"
#include "logger.h"
#include "type_utility.h"

namespace bbd {
template <class F, typename T>
class Function : public FunctionBase {};

template <class F, size_t... I>
class Function<F, IndexSequence<I...>> : public FunctionBase {
 public:
  template <typename U>
  explicit Function(U&& f) : m_func(std::forward<U>(f)) {
  }
  virtual ~Function() = default;
  virtual boost::any invoke(const std::string& name,
                            const std::shared_ptr<Message>& msg) {
    try {
      auto tp = boost::any_cast <
                std::tuple<typename FunctionHelp<F>::template
                Paratypecvf<I>... >> (msg->get_tuple());
      return tupleHelp(tp, FunctionHelp<F>::ParaSeq);
    } catch (const std::exception& e) {
      BBD_ERROR("Message=%s,invoke error check pub or call param type",
                name.c_str());
    }
    return boost::any{};
  }

 private:
  template <typename U = F, typename Tp, size_t... Is,
            enable_if_t<
              boost::is_same<typename FunctionHelp<U>::RetType, void>::value,
              int> = 0>
  boost::any tupleHelp(Tp& tp, IndexSequence<Is...>) {  // NOLINT
    invoke(std::get<Is>(tp)...);
    return boost::any{};
  }
  template < typename U = F, typename Tp, size_t... Is,
             enable_if_t <
               !boost::is_same<typename FunctionHelp<U>::RetType, void>::value,
               int > = 0 >
  boost::any tupleHelp(Tp& tp, IndexSequence<Is...>) {  // NOLINT
    return invoke(std::get<Is>(tp)...);
  }
  template <typename... Args>
  typename FunctionHelp<F>::RetType invoke(Args&&... args) {
    return m_func(args...);
  }

 private:
  std::function<typename FunctionHelp<F>::RetType(
    typename FunctionHelp<F>::template Paratype<I>...)>
  m_func;
};
}  // namespace bbd