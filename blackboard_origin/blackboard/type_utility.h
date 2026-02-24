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
#include <type_traits>
#include <utility>

namespace bbd {
template <size_t...>
struct IndexSequence {};

template <size_t N, size_t... Next>
struct IndexSequenceHelper : public IndexSequenceHelper < N - 1, N - 1,
  Next... > {
};

template <size_t... Next>
struct IndexSequenceHelper<0, Next...> {
  using type = IndexSequence<Next...>;
};

template <size_t N>
using MakeIndexSequence = typename IndexSequenceHelper<N>::type;

template <typename... Types>
using IndexSequenceFor = MakeIndexSequence<sizeof...(Types)>;

template <typename R, typename ClassType, typename... Args, std::size_t... I>
std::function<R(Args...)> transfer(ClassType* pclass,
                                   R(ClassType::*f)(Args...),
                                   IndexSequence<I...>) {
  return std::bind(f, pclass, std::_Placeholder < I + 1 > {}...);
}
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template <typename T>
using remove_cvf_t = std::remove_cv_t<std::remove_reference_t<T>>;


}  // namespace bbd