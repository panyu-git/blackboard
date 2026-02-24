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

#include <tuple>
#include <type_traits>

#include "type_utility.h"

namespace bbd {
template <typename T>
struct FunctionHelp;

template <typename R, typename... Args>
struct FunctionHelp<R(&)(Args...)> {
  static constexpr size_t paraCount = sizeof...(Args);

  using IndexSequence = MakeIndexSequence<sizeof...(Args)>;

  using RetType = R;

  template <std::size_t N>
  using Paratype = typename std::tuple_element<N, std::tuple<Args...>>::type;

  template <std::size_t N>
  using Paratypecvf = typename std::tuple_element <
                      N,
                      std::tuple<remove_cvf_t<Args>... >>::type;

  // 移除 boost::remove_cv_t 相关内容

  static constexpr auto ParaSeq = IndexSequenceFor<Args...> {};
};
template <typename R, typename... Args>
struct FunctionHelp<R(*)(Args...)> {
  static constexpr size_t paraCount = sizeof...(Args);

  using IndexSequence = MakeIndexSequence<sizeof...(Args)>;

  using RetType = R;

  template <std::size_t N>
  using Paratype = typename std::tuple_element<N, std::tuple<Args...>>::type;

  template <std::size_t N>
  using Paratypecvf = typename std::tuple_element <
                      N,
                      std::tuple<remove_cvf_t<Args>... >>::type;

  // 移除 boost::remove_cv_t 相关内容

  template <std::size_t N>
  using Paratyperef =
    typename std::tuple_element < N, std::tuple < Args && ... >>::type;

  static constexpr auto ParaSeq = IndexSequenceFor<Args...> {};
};

template <typename R, typename ClassType, typename... Args>
struct FunctionHelp<R(ClassType::*)(Args...) const> {
  static constexpr size_t paraCount = sizeof...(Args);

  using IndexSequence = MakeIndexSequence<sizeof...(Args)>;

  using RetType = R;

  template <std::size_t N>
  using Paratype = typename std::tuple_element<N, std::tuple<Args...>>::type;

  template <std::size_t N>
  using Paratypecvf = typename std::tuple_element <
                      N,
                      std::tuple<remove_cvf_t<Args>... >>::type;

  static constexpr auto ParaSeq = IndexSequenceFor<Args...> {};

  template <std::size_t N>
  using Paratyperef =
    typename std::tuple_element < N, std::tuple < Args && ... >>::type;
};
}  // namespace bbd