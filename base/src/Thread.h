// @Author Hao Chen
// @Email chenhao980510@163.com
#pragma once

#include <pthread.h>

#include <string>
#include <functional>

#include "CountDownLatch.h"
#include "CurrentThread.h"
#include "Atomic.hpp"
#include "noncopyable.hpp"

namespace tinyWeb {

class Thread : public noncopyable {
 public:
  typedef std::function<void()> ThreadFunc;
  explicit Thread(ThreadFunc threadFunc, \
                const std::string& name = std::string());
  ~Thread();

  void start();
  int join();
  bool started() const { return started_; }
  bool joined() {return joined_; }
  std::string name() const { return name_; }
  pid_t tid() const { return tid_; }
  pthread_t pthreadId() const { return pthreadId_; }

  static int32_t numCreated() { return numCreated_.get(); }

 private:
  void setDefaultName();

  ThreadFunc threadFunc_;
  std::string name_;
  pid_t tid_;
  pthread_t pthreadId_;
  CountDownLatch latch_;
  bool started_;
  bool joined_;

  static AtomicInt32 numCreated_;
};

}  // namespace tinyWeb
