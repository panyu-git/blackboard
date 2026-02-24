/*
 完美转发（Perfect Forwarding）总结与示例

 知识点：
 - 转发引用（又叫通用引用）：模板参数推导出的 `T&&`，在不同上下文可绑定左值或右值。
 - 使用 `std::forward<T>(x)` 根据模板参数 `T` 保留原始值类别（lvalue/rvalue）。
 - `std::move(x)` 总是将其变为右值（破坏原值可复用性）。
 - `std::forward` 只能在函数模板内部配合类型参数使用；直接对非模板参数使用是未定义语义。
 - 常见陷阱：与 `initializer_list`、重载决议、const/volatile 以及引用折叠相关的问题。

 使用场景：
 - 工厂函数（类似 `make_unique` / `emplace`），在内部构造对象并传递参数。
 - 包装器/转发器（log/trace/dispatch wrappers），将参数无损传递到底层接口。
 - 转发构造函数（完美转发构造子模板），实现泛型转发构造。

 示例代码展示：
 - 如何通过 `std::forward` 保持参数的值类别并调用不同重载。
 - 一个简单的工厂 `make_unique_fwd` 使用完美转发。
*/

#include <iostream>
#include <utility>
#include <memory>

// 两个重载用于展示左值/右值的不同处理
void process(int &v) { std::cout << "process(int&) called, value=" << v << "\n"; }
void process(int &&v) { std::cout << "process(int&&) called, value=" << v << "\n"; }

// 通用包装器：接受转发引用并完美转发到 process
template<typename T>
void wrapper(T&& t) {
  std::cout << "wrapper forwarding -> ";
  process(std::forward<T>(t));
}

// 一个简单的 make_unique 风格工厂，使用完美转发构造对象
template<typename T, typename... Args>
std::unique_ptr<T> make_unique_fwd(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// 用于演示构造函数重载
struct Obj {
  Obj(const int &v) { std::cout << "Obj constructed from lvalue: " << v << "\n"; }
  Obj(int &&v) { std::cout << "Obj constructed from rvalue: " << v << "\n"; }
};

int main() {
  std::cout << "-- Perfect Forwarding Demo --\n";

  int a = 10;
  // 保持左值 -> 调用 process(int&)
  wrapper(a);

  // 传入临时量（右值） -> 调用 process(int&&)
  wrapper(20);

  // std::move 将 a 转为右值，后续 a 的值可能被视为已移动（对内置类型仍然可读）
  wrapper(std::move(a));
  std::cout << "a after move (still readable for int): " << a << "\n";

  std::cout << "-- Factory (make_unique_fwd) Demo --\n";
  int v = 5;
  auto p1 = make_unique_fwd<Obj>(v);   // 使用 lvalue 构造
  auto p2 = make_unique_fwd<Obj>(10);  // 使用 rvalue 构造

  (void)p1; (void)p2;
  return 0;
}
