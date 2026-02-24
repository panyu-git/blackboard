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
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/optional.hpp>
#include <boost/type_traits.hpp>

#include "function.h"
#include "function_base.h"
#include "function_help.h"
#include "thread_pool.h"
#include "logger.h"

namespace bbd {

enum PubType {
  NoSave,
  Save
};
enum SubType {
  Sync,
  Async
};

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
  BlackBoard(BlackBoard&&) = delete;
  BlackBoard& operator=(const BlackBoard&) = delete;
  BlackBoard& operator=(BlackBoard&&) = delete;

 public:
  template <PubType pub_type, typename T, typename DataType = remove_cvf_t<T>>
  bool Pub(const std::string& data_name, T && data) {
    auto data_ptr = std::make_shared<Message>(
                      std::tuple<boost::remove_reference_t<DataType>>(
                        std::make_tuple(std::forward<T>(data))));
    std::map<uint64_t, std::shared_ptr<FunctionBase>> funcs_temp;
    std::map<uint64_t, std::shared_ptr<FunctionBase>> funcs_async_temp;
    {
      std::lock_guard<std::mutex> lk(datas_mutex_);
      if (datas_.count(data_name)) {
        if (datas_[data_name].data_ == nullptr) {
          datas_[data_name].data_ = data_ptr;
          funcs_temp = datas_[data_name].funcs_;
          funcs_async_temp = datas_[data_name].funcs_async_;
        }
        if (datas_[data_name].data_->get_type() ==
            boost::typeindex::type_id<std::tuple<DataType>>().type_info()) {
          if (pub_type == Save) {
            datas_[data_name].data_ = data_ptr;
          }
          funcs_temp = datas_[data_name].funcs_;
          funcs_async_temp = datas_[data_name].funcs_async_;
        } else {
          BBD_ERROR("Message:%s,Pub data type is error,pub type=%s,sub type=%s",
                    data_name.c_str(),
                    boost::typeindex::type_id<std::tuple<DataType>>().
                    type_info().name(),
                    datas_[data_name].data_->get_type().name());
          return false;
        }
      } else {
        if (pub_type == Save) {
          datas_[data_name] = Data(data_ptr);
        } else {
          datas_[data_name] = Data(nullptr);
        }
      }
    }
    for (auto iter : funcs_async_temp) {
      thread_pool_.Enqueue([iter, data_name, data_ptr]() {
        iter.second->invoke(data_name, data_ptr);
      });
    }
    for (auto iter : funcs_temp) {
      iter.second->invoke(data_name, data_ptr);
    }
    return true;
  }
  template <typename T, typename DataType = remove_cvf_t<T>>
  bool Set(const std::string& data_name, T && data) {
    auto data_ptr = std::make_shared<Message>(
                      std::tuple<boost::remove_reference_t<DataType>>(
                        std::make_tuple(std::forward<T>(data))));
    {
      std::lock_guard<std::mutex> lk(datas_mutex_);
      if (datas_.count(data_name)) {
        if (datas_[data_name].data_ == nullptr) {
          datas_[data_name].data_ = data_ptr;
        } else {
          if (datas_[data_name].data_->get_type() ==
              boost::typeindex::type_id<std::tuple<DataType>>().type_info()) {
            datas_[data_name].data_ = data_ptr;
          } else {
            BBD_ERROR("Message:%s,Set data is error,pub type=%s,sub type=%s",
                      data_name.c_str(),
                      boost::typeindex::type_id<std::tuple<DataType>>().
                      type_info().name(),
                      datas_[data_name].data_->get_type().name());
            return false;
          }
        }
      } else {
        datas_[data_name] = Data(data_ptr);
      }
    }
    return true;
  }

  // lambda
  template <SubType sub_type,
            typename F,
            typename FunctionType =
            decltype(&boost::remove_reference_t<F>::operator()),
            enable_if_t<FunctionHelp<FunctionType>::paraCount == 1, int> = 0>
  Handler Sub(std::string name, F && f) {
    auto func_ptr = std::make_shared<Function<
                    FunctionType,
                    typename FunctionHelp<FunctionType>::IndexSequence>>(
                      std::forward<F>(f));
    return Sub<sub_type>(name, func_ptr);
  }
  template <SubType sub_type,
            typename R, typename... Args,
            typename FunctionType = R(*)(boost::remove_cv_t<Args>...),
            enable_if_t<FunctionHelp<FunctionType>::paraCount == 1, int> = 0>
  Handler Sub(std::string name, R(*f)(Args...)) {
    auto func_ptr = std::make_shared<Function<
                    FunctionType,
                    typename FunctionHelp<FunctionType>::IndexSequence>>(
                      std::forward<FunctionType>(f));
    return Sub<sub_type>(name, func_ptr);
  }

  template <SubType sub_type,
            typename R, typename... Args, typename ClassType,
            typename FunctionType = R(*)(boost::remove_cv_t<Args>...),
            enable_if_t<FunctionHelp<FunctionType>::paraCount == 1, int> = 0>
  Handler Sub(std::string name, R(ClassType::*f)(Args...),
           ClassType* class_ptr) {
    std::function<R(Args...)> func =
      transfer(class_ptr, f, IndexSequenceFor<Args...> {});
    auto func_ptr = std::make_shared<Function<
                    FunctionType,
                    typename FunctionHelp<FunctionType>::IndexSequence>>(
                      std::forward<std::function<R(Args...)>>(func));
    return Sub<sub_type>(name, func_ptr);
  }

  template <SubType sub_type>
  Handler Sub(const std::string& name, std::shared_ptr<FunctionBase> func_ptr) {
    uint64_t id = reinterpret_cast<uint64_t>(func_ptr.get());
    {
      std::lock_guard<std::mutex> lk(datas_mutex_);
      if (datas_.count(name)) {
        if (sub_type == Sync) {
          datas_[name].funcs_.emplace(id, func_ptr);
        } else if (sub_type == Async) {
          datas_[name].funcs_async_.emplace(id, func_ptr);
        }
      } else {
        Data data_temp;
        if (sub_type == Sync) {
          data_temp.funcs_.emplace(id, func_ptr);
        } else if (sub_type == Async) {
          data_temp.funcs_async_.emplace(id, func_ptr);
        }
        datas_[name] = data_temp;
      }
    }
    return {name, id};
  }
  // lambda
  template <typename F,
            typename FunctionType =
            decltype(&boost::remove_reference_t<F>::operator())>
  Handler Register(const std::string& name, F && f) {
    auto func_ptr = std::make_shared<Function<
                    FunctionType,
                    typename FunctionHelp<FunctionType>::IndexSequence>>(
                      std::forward<F>(f));
    return Register(name, func_ptr);
  }
  template <typename R, typename... Args,
            typename FunctionType = R(*)(boost::remove_cv_t<Args>...)>
  Handler Register(const std::string& name, R(*f)(Args...)) {
    auto func_ptr = std::make_shared<Function<
                    FunctionType,
                    typename FunctionHelp<FunctionType>::IndexSequence>>(
                      std::forward<FunctionType>(f));
    return Register(name, func_ptr);
  }

  template <typename R, typename... Args, typename ClassType,
            typename FunctionType = R(*)(boost::remove_cv_t<Args>...)>
  Handler Register(const std::string& name, R(ClassType::*f)(Args...),
                ClassType* class_ptr) {
    std::function<R(Args...)> func =
      transfer(class_ptr, f, IndexSequenceFor<Args...> {});
    auto func_ptr = std::make_shared<Function<
                    FunctionType,
                    typename FunctionHelp<FunctionType>::IndexSequence>>(
                      std::forward<std::function<R(Args...)>>(func));
    return Register(name, func_ptr);
  }

  template <typename R = void, typename... Args>
  auto Call(const std::string& name, Args && ... args)->
  typename std::enable_if < !std::is_same<R, void>::value,
  boost::optional<R >>::type {
    auto msg_ptr = std::make_shared<Message>(
                     std::tuple<remove_cvf_t<Args>...>(
                       std::make_tuple(std::forward<Args>(args)...)));
    std::shared_ptr<FunctionBase> func_ptr = nullptr;
    {
      std::lock_guard<std::mutex> lk(rpc_mutex_);
      if (rpc_func_.count(name)) {
        func_ptr = rpc_func_[name];
      } else {
        BBD_ERROR("The message %s call error ,dont find register function");
        return boost::optional<R> {};
      }
    }
    boost::any an = func_ptr->invoke(name, msg_ptr);
    if (!an.empty()) {
      try {
        return boost::optional<R> {boost::any_cast < R&& > (an)};
      } catch (const std::exception& e) {
        BBD_ERROR("The message %s call error,return type do not match");
        return boost::optional<R> {};
      }
    }
    return boost::optional<R> {};
  }

  template <typename R = void, typename... Args>
  auto Call(const std::string& name, Args && ... args) ->
  typename std::enable_if<std::is_same<R, void>::value, bool>::type {
    auto msg_ptr = std::make_shared<Message>(
                     std::tuple<remove_cvf_t<Args>...>(
                       std::make_tuple(std::forward<Args>(args)...)));
    std::shared_ptr<FunctionBase> func_ptr = nullptr;
    {
      std::lock_guard<std::mutex> lk(rpc_mutex_);
      if (rpc_func_.count(name)) {
        func_ptr = rpc_func_[name];
      } else {
        BBD_ERROR("The message %s call error ,dont find register function");
        return false;
      }
    }
    if (func_ptr) {
      func_ptr->invoke(name, msg_ptr);
    }
    return true;
  }

  template <typename... Args>
  bool CallAsync(const std::string& name, Args&&... args) {
    std::shared_ptr<FunctionBase> func_ptr = nullptr;
    auto msg_ptr =
      std::make_shared<Message>(std::make_tuple<Args...>
                                (std::forward<Args>(args) ...));
    {
      std::lock_guard<std::mutex> lk(rpc_mutex_);
      if (rpc_func_.count(name)) {
        func_ptr = rpc_func_[name];
      } else {
        BBD_ERROR("The message %s call error ,dont find register function");
        return false;
      }
    }
    thread_pool_.Enqueue([name, func_ptr, msg_ptr]() {
      func_ptr->invoke(name, msg_ptr);
    });
    return true;
  }

  template <typename T>
  bool Get(const std::string& name, T& data) {  // NOLINT
    {
      std::lock_guard<std::mutex> lk(datas_mutex_);
      if (datas_.count(name) && datas_[name].data_ != nullptr) {
        if (datas_[name].data_->get_type() ==
            boost::typeindex::type_id<std::tuple<T>>().type_info()) {
          boost::any an = datas_[name].data_->get_tuple();
          if (!an.empty()) {
            data = std::get<0>(boost::any_cast<std::tuple<T>>(an));
            return true;
          }
        }
      }
    }
    return false;
  }
  template <typename T>
  boost::optional<T> Get(const std::string& name) {
    {
      std::lock_guard<std::mutex> lk(datas_mutex_);
      if (datas_.count(name) && datas_[name].data_ != nullptr) {
        if (datas_[name].data_->get_type() ==
            boost::typeindex::type_id<std::tuple<T>>().type_info()) {
          boost::any an = datas_[name].data_->get_tuple();
          if (!an.empty()) {
            return boost::optional<T> {
              std::get<0>(boost::any_cast<std::tuple<T>>(an))
            };
          }
        }
      }
    }
    return boost::optional<T> {};
  }

  bool UnSub(Handler handler);

  Handler Register(const std::string& name,
                std::shared_ptr<FunctionBase> func_ptr);

  bool UnRegister(Handler handler);

  std::shared_ptr<BlackBoard> Create(const std::string& name,
                                     size_t thread_num);
  // bbd 恢复初始状态
  void Clear();

  ~BlackBoard() = default;

 public:
  BlackBoard();

  explicit BlackBoard(size_t thread_num);
 private:

  struct Data {
    Data() = default;
    explicit Data(const std::shared_ptr<Message>& data)
      : data_(data) {
    }
    inline void UnSub(uint64_t func_id) {
      auto iter = funcs_.find(func_id);
      if (iter != funcs_.end()) {
        funcs_.erase(iter);
      }
      auto iter_async = funcs_async_.find(func_id);
      if (iter_async != funcs_async_.end()) {
        funcs_async_.erase(iter_async);
      }
    }
    std::shared_ptr<Message> data_ = nullptr;
    std::map<uint64_t, std::shared_ptr<FunctionBase>> funcs_;
    std::map<uint64_t, std::shared_ptr<FunctionBase>> funcs_async_;
  };

  std::mutex datas_mutex_;
  std::mutex rpc_mutex_;
  std::map<std::string, Data> datas_;
  std::map<std::string, std::shared_ptr<FunctionBase>> rpc_func_;
  bbd::ThreadPool thread_pool_;

  std::mutex boards_mutex_;
  std::map<std::string, std::weak_ptr<BlackBoard>> boards_;
};



template <PubType pub_type = Save, typename T>
bool Pub(const std::string& name, T && data) {
  return BlackBoard::GetInstance().Pub<pub_type>(name, data);
}


template <SubType sub_type = Async, typename F>
Handler Sub(std::string name, F && func) {
  return BlackBoard::GetInstance().Sub<sub_type>(name, func);
}

template <SubType sub_type = Async, typename F, typename C>
Handler Sub(std::string name, F && func, C* class_ptr) {
  return BlackBoard::GetInstance().Sub<sub_type>(name, func, class_ptr);
}

template <typename R = void, typename... Args>
typename std::enable_if < !std::is_same<R, void>::value,
         boost::optional<R >>::type
Call(const std::string& name, Args && ... args) {
  return BlackBoard::GetInstance().Call<R>(name, args...);
}

template <typename R = void, typename... Args>
typename std::enable_if<std::is_same<R, void>::value, bool>::type
Call(const std::string& name, Args && ... args) {
  return BlackBoard::GetInstance().Call(name, args...);
}

template <typename... Args>
bool CallAsync(const std::string& name, Args&&... args) {
  return BlackBoard::GetInstance().CallAsync(name, args...);
}

template <SubType sub_type = Async, typename F>
Handler Register(std::string name, F && func) {
  return BlackBoard::GetInstance().Register(name, func);
}

template <SubType sub_type = Async, typename F, typename C>
Handler Register(std::string name, F && func, C* class_ptr) {
  return BlackBoard::GetInstance().Register(name, func, class_ptr);
}

template <typename T>
bool Set(const std::string& name, T && data) {
  return BlackBoard::GetInstance().Set(name, data);
}

template <typename T>
bool Get(const std::string& name, T& data) {  // NOLINT
  return BlackBoard::GetInstance().Get(name, data);
}

template <typename T>
boost::optional<T> Get(const std::string& name) {
  return BlackBoard::GetInstance().Get<T>(name);
}

bool UnSub(Handler handler);

bool UnRegister(Handler handler);

std::shared_ptr<BlackBoard> Create(const std::string& name,
                                   size_t thread_num);

inline void Clear() {
  return BlackBoard::GetInstance().Clear();
}

}  // namespace bbd