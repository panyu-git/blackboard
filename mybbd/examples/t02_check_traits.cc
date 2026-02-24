#include <iostream>
#include <typeinfo>
#include <cxxabi.h>
#include <cstdlib>
#include "blackboard/function_traits.h"

// 判定哪种部分特化匹配的辅助 trait
template<typename T>
struct which_specialization { static constexpr const char* value = "unknown"; };

template<typename R, typename... Args>
struct which_specialization<R(*)(Args...)> { static constexpr const char* value = "free_function"; };

template<typename C, typename R, typename... Args>
struct which_specialization<R(C::*)(Args...)> { static constexpr const char* value = "member_nonconst"; };

template<typename C, typename R, typename... Args>
struct which_specialization<R(C::*)(Args...) const> { static constexpr const char* value = "member_const"; };

std::string demangle(const char* name) {
  int status = 0;
  char* s = abi::__cxa_demangle(name, nullptr, nullptr, &status);
  std::string ret = (status == 0 && s) ? s : name;
  free(s);
  return ret;
}

void free_func(int, int) {}

struct MyClass {
  int mem(int, int) { return 0; }
  int memc(int, int) const { return 0; }
};

int main() {
  auto lambda = [](int, int) -> void {};
  auto lambda2 = [](int) -> void {};

  using Op_free = decltype(&free_func);
  using Op_mem = decltype(&MyClass::mem);
  using Op_memc = decltype(&MyClass::memc);
  using Op_lambda = decltype(&decltype(lambda)::operator());
  using Op_lambda2 = decltype(&decltype(lambda2)::operator());

  std::cout << "which_specialization free: " << which_specialization<Op_free>::value << "\n";
  std::cout << "which_specialization mem:  " << which_specialization<Op_mem>::value << "\n";
  std::cout << "which_specialization memc: " << which_specialization<Op_memc>::value << "\n";
  std::cout << "which_specialization lambda: " << which_specialization<Op_lambda>::value << "\n";
  std::cout << "which_specialization lambda2: " << which_specialization<Op_lambda2>::value << "\n";

  std::cout << "demangled types:\n";
  std::cout << " Op_lambda: " << demangle(typeid(Op_lambda).name()) << "\n";
  std::cout << " Op_lambda2: " << demangle(typeid(Op_lambda2).name()) << "\n";
  std::cout << " Op_mem:    " << demangle(typeid(Op_mem).name()) << "\n";
  std::cout << " Op_free:   " << demangle(typeid(Op_free).name()) << "\n";

  return 0;
}
