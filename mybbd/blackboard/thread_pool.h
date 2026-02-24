#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace bbd {

class ThreadPool {
 public:
  explicit ThreadPool(size_t thread_num = std::thread::hardware_concurrency()) 
      : stop_(false) {
    for (size_t i = 0; i < thread_num; ++i) {
      workers_.emplace_back([this] {
        while (true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(this->queue_mutex_);
            this->condition_.wait(lock, [this] { 
              return this->stop_ || !this->tasks_.empty(); 
            });
            if (this->stop_ && this->tasks_.empty()) return;
            task = std::move(this->tasks_.front());
            this->tasks_.pop();
          }
          task();
        }
      });
    }
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      stop_ = true;
    }
    condition_.notify_all();
    for (std::thread& worker : workers_) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  template <class F>
  void Enqueue(F&& f) {
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      if (stop_) throw std::runtime_error("Enqueue on stopped ThreadPool");
      tasks_.emplace(std::forward<F>(f));
    }
    condition_.notify_one();
  }

 private:
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex queue_mutex_;
  std::condition_variable condition_;
  std::atomic<bool> stop_;
};

}  // namespace bbd