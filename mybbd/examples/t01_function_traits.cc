#include <tuple>
#include <type_traits>
#include <iostream>
#include "blackboard/function_traits.h"

using bbd::detail::function_traits;

void RegisteCallback(int a, std::string b) {}

struct MyClass {
  int Multiply(int x) { return x + x; }
  int ConstMul(int x, int y) const { return x * y; }
};

int main() {
  auto lambda = [](int a, int b) -> void {};

  // 普通函数指针
  using FT_free = function_traits<decltype(&RegisteCallback)>;
  static_assert(std::is_same_v<FT_free::R, void>);
  static_assert(std::is_same_v<FT_free::ArgsTuple, std::tuple<int, std::string>>);

  // Lambda：primary 模板会展开为 decltype(&T::operator())
  using FT_lambda = function_traits<decltype(lambda)>;
  static_assert(std::is_same_v<FT_lambda::R, void>);
  static_assert(std::is_same_v<FT_lambda::ArgsTuple, std::tuple<int, int>>);

  // 非 const 成员函数指针
  using FT_mem = function_traits<decltype(&MyClass::Multiply)>;
  static_assert(std::is_same_v<FT_mem::R, int>);
  static_assert(std::is_same_v<FT_mem::ArgsTuple, std::tuple<int>>);

  // const 成员函数指针
  using FT_mem_const = function_traits<decltype(&MyClass::ConstMul)>;
  static_assert(std::is_same_v<FT_mem_const::R, int>);
  static_assert(std::is_same_v<FT_mem_const::ArgsTuple, std::tuple<int, int>>);

  std::cout << "function_traits examples OK\n";
  return 0;
}
