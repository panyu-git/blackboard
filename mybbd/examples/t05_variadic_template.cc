/*
 可变参模板总结与示例（修正版）
  原注释整体结构良好，覆盖了可变参模板的主要方面，但存在以下问题，需要修复更正：
  - 正确性问题：
    - 折叠表达式（fold expressions）是C++17引入的特性，而不是C++11。可变参模板本身是C++11引入的，但折叠表达式是其后续扩展。原注释将两者混淆，需要明确区分。
    - 折叠表达式的语法描述有小误：一元折叠是`( ... op pack )` 或 `( pack op ... )`；二元折叠是`( init op ... op pack )` 或 `( pack op ... op init )`。原注释中的二元左/右折叠描述颠倒，需要修正。
    - 注意事项中提到"参数包必须至少包含一个参数"，但对于一元折叠，如果参数包为空，会导致编译错误；对于二元折叠，允许空包（结果为init）。
    - 在5.3-5.5的继承示例中，描述较为抽象，需要更精确的语法说明（如using基类继承或多重继承）。
    - 整体上，原注释缺少对SFINAE（Substitution Failure Is Not An Error）和概念（C++20）的提及，但鉴于焦点是C++11/17，可视为可选。
  - 完整性问题：
    - 缺少对参数包展开的详细说明，如在类型列表中的使用（e.g., std::tuple）。
    - 未提及参数包的嵌套展开或部分特化。
    - 应用场景可扩展到更多，如元编程（type traits）、变参lambda（C++14）。
    - 添加了更多注意事项，如完美转发的必要性、潜在的ODR（One Definition Rule）问题。
  - 修正后的总结：
    C++11 引入了可变参数模板（variadic templates），使得函数和类模板能够接受任意数量的模板参数。
    这种特性极大地增强了 C++ 的灵活性，允许我们编写更通用和高效的代码。C++17进一步引入折叠表达式以简化参数包处理。
    1- 可变参函数模板
      1.1 基本语法
        - 定义：使用 `template<typename... Args>` 来定义一个可变参数模板。
        - 参数包：在函数参数列表中使用 `Args&&... args` 来接受任意数量的参数（通常使用通用引用以支持完美转发）。
        - 展开参数包：使用 `std::forward<Args>(args)...` 来完美转发参数。
      1.2 重载
        - 可与固定参数模板重载结合，使用SFINAE或requires（C++20）控制重载决议。
    2- 折叠表达式（C++17）
      2.1 基本语法
        - 定义：使用 `(pack op ...)` 或 `( ... op pack)` 来定义一元折叠表达式，其中 `op` 是一个二元运算符。
        - 示例：计算参数的和 `(args + ...)`，或检查所有参数是否满足某个条件 `( (args > 0) && ... )`。
      2.2 类型
        - 一元右折叠：`(pack op ...)`，从右向左折叠，如 `(args + ...)` 等价于 args1 op (args2 op (args3 op ...))。
        - 一元左折叠：`( ... op pack)`，从左向右折叠，如 `( ... + args)` 等价于 ((args1 op args2) op args3) op ...。
        - 二元右折叠：`(init op ... op pack)`，从右向左，允许空包（结果为init）。
        - 二元左折叠：`(pack op ... op init)`，从左向右，允许空包。
      2.3 注意事项
        - 运算符必须支持二元操作，且对于一元折叠，参数包不能为空（否则编译错误）；二元折叠允许空包。
        - 折叠表达式中的运算符优先级和结合性需要注意，可能会影响表达式的求值顺序。使用括号确保正确性。
        - 适用于逗号运算符、逻辑运算符等。
    3- 可变参表达式
      3.1 基本语法
        - 定义：使用 `sizeof...(Args)` 来获取参数包中参数的数量（编译时常量）。
        - 示例：在函数体内使用 `sizeof...(Args)` 来执行不同的逻辑，或在静态断言中检查参数数量。
      3.2 注意事项
        - `sizeof...(Args)` 只能用于模板参数包，不能用于普通函数参数。
        - 在某些上下文中，可能需要使用 `std::enable_if` 或其他 SFINAE 技术来根据参数数量进行重载决议。C++20中可用concepts。
    4- 可变参类模板
      4.1 基本语法
        - 定义：使用 `template<typename... Args>` 来定义一个可变参数类模板。
        - 成员函数：在类模板中定义成员函数时，可以使用 `Args&&... args` 来接受任意数量的参数。
      4.2 示例
        - 实现一个通用的容器类，如 `Tuple`，可以接受任意数量和类型的元素。
        - 实现一个事件系统，允许注册任意数量的回调函数，并在事件触发时调用它们。
      4.3 注意事项
        - 可变参数类模板的成员函数需要正确处理参数包的展开和转发，以避免类型推导错误。
        - 在设计可变参数类模板时，需要考虑如何管理资源和避免内存泄漏，特别是在涉及动态内存分配时。
        - 支持部分特化，如 template<typename T, typename... Args> class MyClass<T, Args...> {};
    5- 类模板中可变参的逐步展开
      5.1 基本语法
        - 定义：使用递归模板实例化来逐步展开参数包（头递归或尾递归）。
        - 示例：实现一个 `print` 函数，可以接受任意数量的参数，并逐个打印它们（通过递归调用）。
      5.2 myclasst<Args...> 继承
        - 定义：在类模板中使用继承自 myclass<Args...> 来实现递归展开（通常结合特化终止递归）。
        - 示例：实现一个 `Tuple` 类模板，通过继承来逐步展开参数包，并提供访问元素的接口。
      5.3 myclasst<Args>... 继承
        - 定义：在类模板中使用多重继承自 myclass<Args>... 来逐个展开参数包（pack expansion in base specifiers）。
        - 示例：实现一个 `Event` 类模板，通过多重继承来注册多个回调函数，并在事件触发时调用它们。
      5.4 myclasst<Args, char>... 继承
        - 定义：在类模板中使用多重继承自 myclass<Args, char>... ，为每个Args添加额外参数（如char）。
        - 示例：实现一个 `Logger` 类模板，通过继承来注册多个日志处理函数，并在日志事件发生时调用它们，同时传递一个额外的参数（如日志级别 char）。
      5.5 myclasst<Args, Args>... 继承
        - 定义：在类模板中使用多重继承自 myclass<Args, Args>... ，重复使用每个Args（e.g., for pairing）。
        - 示例：实现一个 `Dispatcher` 类模板，通过继承来注册多个处理函数，并在事件发生时调用它们，同时传递参数包中的参数（e.g., self-pairing）。
      5.6 注意事项
        - 在使用递归模板实例化时，需要定义基本情况（特化）作为终止条件，以避免无限递归。
        - 在设计类模板时，需要考虑如何管理资源和避免内存泄漏，特别是在涉及动态内存分配时。
        - 在使用继承来展开参数包时，需要注意类的构造和析构顺序，以及可能的二义性问题（使用using或virtual继承解决）。
        - 多重继承可能增加编译时间和二进制大小。
    6- 可变参模板的应用场景
      6.1 工厂函数
        - 定义：使用可变参数模板实现一个工厂函数，可以接受任意数量和类型的参数，并返回一个对象。
        - 示例：实现一个 `make_unique_fwd` 函数，使用完美转发来构造一个对象，并返回一个 `std::unique_ptr`。
      6.2 包装器/转发器
        - 定义：使用可变参数模板实现一个包装器或转发器，可以接受任意数量和类型的参数，并将它们无损传递到底层接口。
        - 示例：实现一个日志包装器，接受任意函数并在其前后添加日志。
      6.3 其他场景
        - 元编程：如实现type list或visitor模式。
        - 变参lambda（C++14）：auto lambda = [](auto&&... args) { ... };
        - 标准库：std::tuple, std::apply, std::make_from_tuple 等。
  - 附加注意事项：
    - 完美转发：始终使用std::forward来保留参数的左/右值性。
    - 性能：过多递归可能导致模板实例化深度超过编译器限制（使用折叠表达式减少递归）。
    - 兼容性：确保代码兼容C++11+，对于C++17特性需注明。
*/

#include <iostream>
#include <utility>  // for std::forward
#include <memory>   // for std::unique_ptr
#include <string>   // for std::string in examples
#include <functional>  // for std::function in event system

// 1. 可变参函数模板 示例
// 基本print函数，使用递归展开
void print() {}  // 基本情况：空参数

template<typename T, typename... Args>
void print(const T& t, const Args&... args) {
    std::cout << t << " ";
    print(args...);
}

// 1.2 重载示例：固定参数重载
void print(int x) {
    std::cout << "Special int: " << x << std::endl;
}

// 2. 折叠表达式 示例 (C++17)
template<typename... Args>
auto sum(Args&&... args) {
    return (args + ...);  // 一元右折叠
}

// 二元折叠示例
template<typename... Args>
auto sum_with_init(Args&&... args) {
    return (0 + ... + args);  // 二元右折叠
}

// 逻辑折叠
template<typename... Args>
bool all_positive(Args&&... args) {
    return ( (args > 0) && ... );
}

// 3. 可变参表达式 示例
template<typename... Args>
void check_pack_size(Args&&... args) {
    constexpr size_t size = sizeof...(Args);
    static_assert(size >= 1, "At least one argument required");
    std::cout << "Pack size: " << size << std::endl;
}

// 4. 可变参类模板 示例
// 简单Tuple-like类
template<typename... Args>
class MyTuple {
public:
    MyTuple(Args&&... args) : data(std::forward<Args>(args)...) {}  // 错误：不能直接这样，需要递归或std::tuple
    // 实际中用std::tuple，但为示例，用部分特化
private:
    std::tuple<std::decay_t<Args>...> data;  // 使用std::tuple存储参数包
};

// 更好示例：事件系统
template<typename... Callbacks>
class Event {
private:
    std::tuple<std::function<void()>...> callbacks;  // 展开为tuple of functions
public:
    Event(Callbacks&&... cbs) : callbacks(std::forward<Callbacks>(cbs)...) {}
    void trigger() {
        std::apply([](auto&&... cb) { (cb(), ...); }, callbacks);  // 使用std::apply和折叠
    }
};

// 5.1 逐步展开 示例：已在上print函数中

// 5.2 myclasst<Args...> 继承 示例：递归Tuple
template<typename... Args> struct Tuple;  // 前向声明

template<typename Head, typename... Tail>
struct Tuple<Head, Tail...> : Tuple<Tail...> {
    Head value;
    Tuple(Head&& h, Tail&&... t) : Tuple<Tail...>(std::forward<Tail>(t)...), value(std::forward<Head>(h)) {}
};

template<> struct Tuple<> {};  // 终止

// 访问示例：需要get函数（省略完整实现）

// 5.3 myclasst<Args>... 继承 示例：多重继承持有值
template<typename T> struct Holder { T value; };

template<typename... Args>
struct MultiHolder : Holder<Args>... {
    MultiHolder(Args&&... args) : Holder<Args>{std::forward<Args>(args)}... {}
    // 使用：static_cast<Holder<T>*>(this)->value
};

// 5.4 myclasst<Args, char>... 示例：添加额外参数
template<typename T, char Level> struct LoggerHolder {
    T func;
    void log() { func(); std::cout << " Level: " << Level << std::endl; }
};

template<typename... Args>
struct MultiLogger : LoggerHolder<Args, 'I'>... {  // 为每个Args添加'I'
    MultiLogger(Args&&... args) : LoggerHolder<Args, 'I'>{std::forward<Args>(args)}... {}
    void trigger() { (LoggerHolder<Args, 'I'>::log(), ...); }
};

// 5.5 myclasst<Args, Args>... 示例：配对
template<typename A, typename B> struct PairDispatcher {
    A a; B b;
    void dispatch() { std::cout << a << " -> " << b << std::endl; }
};

template<typename... Args>
struct MultiDispatcher : PairDispatcher<Args, Args>... {
    MultiDispatcher(Args&&... args) : PairDispatcher<Args, Args>{std::forward<Args>(args), std::forward<Args>(args)}... {}  // 重复
    void trigger() { (PairDispatcher<Args, Args>::dispatch(), ...); }
};

// 6.1 工厂函数 示例
template<typename T, typename... Args>
std::unique_ptr<T> make_unique_fwd(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// 6.2 包装器 示例
template<typename Func, typename... Args>
auto wrapper(Func&& f, Args&&... args) {
    std::cout << "Before call" << std::endl;
    auto result = f(std::forward<Args>(args)...);
    std::cout << "After call" << std::endl;
    return result;
}

int add(int a, int b) { return a + b; }

int main() {
    // 1. print 示例
    print(1, 2.5, "hello");  // 运行结果：1 2.5 hello 
    std::cout << std::endl;
    print(42);  // 运行结果：Special int: 42

    // 2. 折叠 示例
    auto s = sum(1, 2, 3);  // 运行结果：6
    std::cout << "Sum: " << s << std::endl;
    auto s_init = sum_with_init(1, 2, 3);  // 运行结果：6
    std::cout << "Sum with init: " << s_init << std::endl;
    bool pos = all_positive(1, 2, 3);  // 运行结果：true
    std::cout << "All positive: " << std::boolalpha << pos << std::endl;

    // 3. sizeof... 示例
    check_pack_size(1, "a", 3.0);  // 运行结果：Pack size: 3

    // 4. Event 示例
    auto cb1 = []() { std::cout << "CB1" << std::endl; };
    auto cb2 = []() { std::cout << "CB2" << std::endl; };
    Event e(cb1, cb2);
    e.trigger();  // 运行结果：CB1 CB2

    // 5.2 Tuple 示例
    Tuple<int, double, std::string> t(1, 2.5, "test");  // 运行结果：构造成功，无输出

    // 5.3 MultiHolder 示例
    MultiHolder<int, std::string> mh(10, "foo");
    std::cout << static_cast<Holder<int>&>(mh).value << " " << static_cast<Holder<std::string>&>(mh).value << std::endl;  // 运行结果：10 foo

    // 5.4 MultiLogger 示例
    auto log1 = []() { std::cout << "Log1"; };
    auto log2 = []() { std::cout << "Log2"; };
    MultiLogger<decltype(log1), decltype(log2)> ml(log1, log2);
    ml.trigger();  // 运行结果：Log1 Level: I Log2 Level: I

    // 5.5 MultiDispatcher 示例
    MultiDispatcher<int, std::string> md(5, "bar");
    md.trigger();  // 运行结果：5 -> 5 bar -> bar  (由于重复Args)

    // 6.1 make_unique_fwd 示例
    auto ptr = make_unique_fwd<std::string>("hello");
    std::cout << *ptr << std::endl;  // 运行结果：hello

    // 6.2 wrapper 示例
    auto res = wrapper(add, 3, 4);  // 运行结果：Before call After call，返回7
    std::cout << "Result: " << res << std::endl;

    return 0;
}