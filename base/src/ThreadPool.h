#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <functional>

#include "noncopyable.hpp"
#include "Thread.h"
#include "CountDownLatch.h"

namespace tinyWeb {
class ThreadPool : public noncopyable {
public:
  typedef std::function<void()> TaskCallback;

  ThreadPool(const std::string& name = "ThreadPool");
  ~ThreadPool();

  // 在start之前调用，否则就是无界队列
  void setMaxQueueSize(size_t maxQueueSize) {
    maxQueueSize_ = maxQueueSize;
  }

  void setThreadInitCallback(TaskCallback callback) {
    threadInitCallback_ = callback;
  }

  void start(int numThread);
  void stop();
  void addTask(TaskCallback task);
  size_t queueSize() const {
    MutexLockGuard lock(mutex_);
    return tasks_.size();
  }

private:
  typedef std::vector<std::unique_ptr<Thread>> ThreadList;
  typedef std::deque<TaskCallback> TaskQueue;

  TaskCallback getTask();
  void runInThread();
  bool isFull() const {
    mutex_.assertLocked();
    return maxQueueSize_ > 0 && tasks_.size() >= maxQueueSize_;  //只在有界队列时进行检查
  }

  const std::string name_;
  ThreadList threads_;
  mutable MutexLock mutex_;
  Condition notFull_;   // Guardby mutex_
  Condition notEmpty_;  // Guardby mutex_
  size_t maxQueueSize_;
  TaskQueue tasks_;   // Guardby mutex_
  TaskCallback threadInitCallback_;  // Guardby mutex_
  bool running_;  // Guardby mutex_
};
}   // namespace tinyWeb