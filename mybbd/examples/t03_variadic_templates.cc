#include <iostream>
#include <tuple>
#include <utility>

// 1) Fold expression 打印（C++17）
template<typename... Ts>
void print_fold(const Ts&... ts) {
  ((std::cout << ts << ' '), ...);
  std::cout << '\n';
}

// 2) 完美转发并调用
template<typename F, typename... Args>
decltype(auto) invoke(F&& f, Args&&... args) {
  return std::forward<F>(f)(std::forward<Args>(args)...);
}

// 3) 展开 tuple 到可调用对象（自实现 apply）
template<class F, class Tuple, std::size_t... I>
auto apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
  return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}
template<class F, class Tuple>
auto my_apply(F&& f, Tuple&& t) {
  constexpr std::size_t N = std::tuple_size<std::decay_t<Tuple>>::value;
  return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), std::make_index_sequence<N>{});
}

// 4) 递归打印（Pre-C++17 风格）
void print_recursive() { std::cout << "(end)\n"; }
template<typename T, typename... Ts>
void print_recursive(T t, Ts... ts) {
  std::cout << t << ' ';
  print_recursive(ts...);
}

int main() {
  std::cout << "-- print_fold --\n";
  print_fold(1, 2, 3, "hello");

  std::cout << "-- invoke (forwarding) --\n";
  int x = 5;
  auto inc = [](int &v){ ++v; };
  invoke(inc, x);
  std::cout << "x after invoke inc: " << x << '\n';

  std::cout << "-- apply (tuple -> callable) --\n";
  auto tup = std::make_tuple(10, 20);
  auto sum = [](int a, int b) -> int { return a + b; };
  std::cout << "sum: " << my_apply(sum, tup) << '\n';

  std::cout << "-- print_recursive --\n";
  print_recursive(42, "and", 7);

  return 0;
}
