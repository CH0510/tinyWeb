#pragma once

#include <string.h>
#include <assert.h>

#include <utility>

#include <boost/circular_buffer.hpp>

#include "MutexLock.hpp"
#include "Condition.hpp"

namespace tinyWeb {

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(size_t bound) \
    : tasks_(bound), \
      notFull_(mutex_), \
      notEmpty_(mutex_) {
      assert(bound > 0);
  }

  void addTask(const T& task) {
    {
        MutexLockGuard lock(mutex_);
        while (tasks_.full()) {
            notFull_.wait();
        }
        assert(!tasks_.full());
        tasks_.push_back(task);
    }
    notEmpty_.notify();
  }

  void addTask(T&& task) {
    {
        MutexLockGuard lock(mutex_);
        while (tasks_.full()) {
            notFull_.wait();
        }
        assert(!tasks_.full());
        tasks_.push_back(std::forward<T>(task));
    }
    notEmpty_.notify();
  }

  T getTask() {
    T temp;
    {
        MutexLockGuard lock(mutex_);
        while (tasks_.empty()) {
            notEmpty_.wait();
        }
        assert(!tasks_.empty());
        temp = std::move(tasks_.front());
        tasks_.pop_front();
    }
    notFull_.notify();

    return temp;
  }

  size_t size() const {
    MutexLockGuard lock(mutex_);
    return tasks_.size();
  }

 private:
  boost::circular_buffer<T> tasks_;
  mutable MutexLock mutex_;
  Condition notFull_;
  Condition notEmpty_;
};

}  // namespace tinyWeb
