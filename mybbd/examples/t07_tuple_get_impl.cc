#include <iostream>
#include <utility>  // for std::forward
#include <memory>   // for std::unique_ptr
#include <string>   // for std::string in examples
#include <boost/type_index.hpp>

template<typename... T>
struct MyTuple {};

template<> struct MyTuple<> {};

template<typename Head, typename... Tail>
struct MyTuple<Head, Tail...> : MyTuple<Tail...> {
  Head head;
  MyTuple(Head&& h, Tail&&... t) : MyTuple<Tail...>(std::forward<Tail>(t)...), head(std::forward<Head>(h)) {
    std::cout << "MyTuple constructor: " << boost::typeindex::type_id_with_cvr<MyTuple<Head, Tail...>>().pretty_name() << std::endl;
    std::cout << "MyTuple constructor ptr: " << this << std::endl;
    std::cout << "MyTuple constructor head ptr: " << &head << std::endl;
  }
};

template <std::size_t I, typename T>
struct TupleElement;

template <typename Head, typename... Tail>
struct TupleElement<0, MyTuple<Head, Tail...>> {
  using type = Head;
  using base_tuple_type = MyTuple<Head, Tail...>;
};

template <std::size_t I, typename Head, typename... Tail>
struct TupleElement<I, MyTuple<Head, Tail...>> 
    : TupleElement<I - 1, MyTuple<Tail...>> {};

// get
template <std::size_t I, typename... T>
auto& GetTp(MyTuple<T...>& t) {
  // 拿到对应的基类类型
  using Type = typename TupleElement<I, MyTuple<T...>>::base_tuple_type;
  std::cout << "===========" << std::endl;
  std::cout << boost::typeindex::type_id_with_cvr<Type>().pretty_name() << std::endl;
  std::cout << boost::typeindex::type_id_with_cvr<Type>().pretty_name() << std::endl;
  // 向上转型并访问 head
  return static_cast<Type&>(t).head;
}

template <std::size_t I, typename... T>
struct TupleHelper;

template <typename Head, typename... T>
struct TupleHelper<0, MyTuple<Head, T...>> {
  using Type = MyTuple<Head, T...>;
};

template <std::size_t I, typename Head, typename... T>
struct TupleHelper<I, MyTuple<Head, T...>>
  : TupleHelper<I - 1, MyTuple<T...>> {};

template <std::size_t I, typename... T>
auto& GetTpHelper(MyTuple<T...>& t) {
  using Type = typename TupleHelper<I, MyTuple<T...>>::Type;
  std::cout << "===========" << std::endl;
  std::cout << boost::typeindex::type_id_with_cvr<Type>().pretty_name() << std::endl;
  std::cout << boost::typeindex::type_id_with_cvr<Type>().pretty_name() << std::endl;
  return static_cast<Type&>(t).head;
}

int main() {
    // std::tuple<int, double, std::string> tp = {1, 3.14, "tuple"};
    // std::cout << "Tuple get: " << std::get<0>(tp) << ", " << std::get<1>(tp) << ", " << std::get<2>(tp) << std::endl;

    // MyTuple<int, double, char> mytp(1, 3.14, 'c');
    // auto result0 = GetTp<0>(mytp);
    // auto result1 = GetTp<1>(mytp);
    // auto result2 = GetTp<2>(mytp);
    // std::cout << "MyTuple get: " << result0 << ", " << result1 << ", " << result2 << std::endl;
    MyTuple<int, double, long> t(42, 3.14, 77);

    // 1. 直接访问（拿到 int）
    // std::cout << "MyTuple get: " << GetTpHelper<2>(t) << std::endl;

    return 0;
}