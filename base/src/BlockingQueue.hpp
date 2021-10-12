#pragma once

#include <string.h>

#include <deque>
#include <utility>

#include "MutexLock.hpp"
#include "Condition.hpp"

namespace tinyWeb {

template <typename T>
class BlockingQueue {
 public:
  BlockingQueue() : cond_(mutex_) {
  }

  void addTask(const T& task) {
    {
        MutexLockGuard lock(mutex_);
        tasks_.emplace_back(task);
    }
    cond_.notify();
  }

  void addTask(T&& task) {
    {
        MutexLockGuard lock(mutex_);
        tasks_.emplace_back(std::forward<T>(task));
    }
    cond_.notify();
  }

  T getTask() {
    T temp;
    {
        MutexLockGuard lock(mutex_);
        while (tasks_.empty()) {
            cond_.wait();
        }
        assert(tasks_.empty() == false);

        temp = std::move(tasks_.front());
        tasks_.pop_front();
    }
    return temp;
  }

  size_t size() const {
    MutexLockGuard lock(mutex_);
    return tasks_.size();
  }

 private:
  std::deque<T> tasks_;
  mutable MutexLock mutex_;
  Condition cond_;
};

}  // namespace tinyWeb
