#pragma once

#include <any>
#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>

#include "logger.h"
#include "thread_pool.h"
#include "function_traits.h"

namespace bbd {

enum PubType { NoSave, Save };
enum SubType { Sync, Async };

struct Handler {
  std::string name;
  uint64_t id;
};

class BlackBoard {
 public:
  static BlackBoard& GetInstance() {
    static BlackBoard instance;
    return instance;
  }

  BlackBoard(const BlackBoard&) = delete;
  BlackBoard& operator=(const BlackBoard&) = delete;

  // 1. 注册函数 (现已支持函数指针、Lambda表达式、std::function)
  template <typename F>
  Handler Register(const std::string& name, F&& f) {
    using Traits = detail::function_traits<std::decay_t<F>>;
    using R = typename Traits::R;
    using ArgsTuple = typename Traits::ArgsTuple;

    // 核心类型擦除：将任何 Callable 包装成接收 any 返回 any 的统一接口
    auto type_erased_func = [func = std::forward<F>(f)](std::any args_any) -> std::any {
      auto* tuple_ptr = std::any_cast<ArgsTuple>(&args_any);
      if (!tuple_ptr) {
        throw std::bad_any_cast();
      }
      if constexpr (std::is_void_v<R>) {
        std::apply(func, *tuple_ptr);
        return std::any{};
      } else {
        return std::any(std::apply(func, *tuple_ptr));
      }
    };

    std::lock_guard<std::mutex> lk(rpc_mutex_);
    rpc_func_[name] = std::move(type_erased_func);
    return {name, ++id_counter_}; // 使用原子计数器替代原始指针强转更安全
  }

  // 2. 同步调用 (使用 if constexpr 统一 void 和非 void 返回值)
  template <typename R = void, typename... Args>
  auto Call(const std::string& name, Args&&... args) {
    std::any args_any = std::make_tuple(std::forward<Args>(args)...);
    std::function<std::any(std::any)> func;

    {
      std::lock_guard<std::mutex> lk(rpc_mutex_);
      auto it = rpc_func_.find(name);
      if (it == rpc_func_.end()) {
        BBD_ERROR("Call error: cannot find registered function for '" << name << "'");
        if constexpr (std::is_void_v<R>) return false;
        else return std::optional<R>{};
      }
      func = it->second;
    }

    try {
      std::any ret = func(std::move(args_any));
      if constexpr (std::is_void_v<R>) {
        return true;
      } else {
        return std::optional<R>{std::any_cast<R>(ret)};
      }
    } catch (const std::exception& e) {
      BBD_ERROR("Call error for '" << name << "': param/return type mismatch");
      if constexpr (std::is_void_v<R>) return false;
      else return std::optional<R>{};
    }
  }

  // 3. 异步调用
  template <typename... Args>
  bool CallAsync(const std::string& name, Args&&... args) {
    std::any args_any = std::make_tuple(std::forward<Args>(args)...);
    std::function<std::any(std::any)> func;

    {
      std::lock_guard<std::mutex> lk(rpc_mutex_);
      auto it = rpc_func_.find(name);
      if (it == rpc_func_.end()) {
        BBD_ERROR("Async Call error: cannot find registered function for '" << name << "'");
        return false;
      }
      func = it->second;
    }

    thread_pool_.Enqueue([func = std::move(func), args_any = std::move(args_any), name]() mutable {
      try {
        func(std::move(args_any));
      } catch (const std::exception& e) {
        BBD_ERROR("Async Call error for '" << name << "': param type mismatch");
      }
    });
    return true;
  }

  void Clear() {
    std::lock_guard<std::mutex> lk(rpc_mutex_);
    rpc_func_.clear();
  }

 private:
  BlackBoard() : thread_pool_(std::thread::hardware_concurrency()) {}
  ~BlackBoard() = default;

  std::mutex rpc_mutex_;
  // 统一存储擦除了签名的 Callable
  std::map<std::string, std::function<std::any(std::any)>> rpc_func_;
  ThreadPool thread_pool_;
  std::atomic<uint64_t> id_counter_{0};
};

// 全局便捷接口
template <typename R = void, typename... Args>
auto Call(const std::string& name, Args&&... args) {
  return BlackBoard::GetInstance().Call<R>(name, std::forward<Args>(args)...);
}

template <typename... Args>
bool CallAsync(const std::string& name, Args&&... args) {
  return BlackBoard::GetInstance().CallAsync(name, std::forward<Args>(args)...);
}

template <typename F>
Handler Register(const std::string& name, F&& func) {
  return BlackBoard::GetInstance().Register(name, std::forward<F>(func));
}

// 自动绑定对象实例 (obj) 和成员函数指针 (mem_func)
template <typename ReturnType, typename... Args, typename ClassType>
Handler Register(const std::string& name, ReturnType (ClassType::*mem_func)(Args...), ClassType* obj) {
  // 将成员函数和 this 指针打包成一个普通的 Lambda
  auto bound_func = [obj, mem_func](Args... args) -> ReturnType {
    return (obj->*mem_func)(std::forward<Args>(args)...);
  };
  return BlackBoard::GetInstance().Register(name, std::move(bound_func));
}

inline void Clear() {
  BlackBoard::GetInstance().Clear();
}

}  // namespace bbd