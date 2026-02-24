#pragma once
#include <tuple>
#include <type_traits>

namespace bbd {
// 内部工具：用于推导函数或Lambda的签名类型
namespace detail {

template <typename T>
struct function_traits : function_traits<decltype(&T::operator())> {};

template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)> {
  using R = ReturnType;
  using ArgsTuple = std::tuple<std::decay_t<Args>...>;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const> {
  using R = ReturnType;
  using ArgsTuple = std::tuple<std::decay_t<Args>...>;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)> {
  using R = ReturnType;
  using ArgsTuple = std::tuple<std::decay_t<Args>...>;
};

}  // namespace detail
}  // namespace bbd