// @Author Hao Chen
// @Email  chenhao980810@163.com
#pragma once

#include "noncopyable.hpp"
#include "MutexLock.hpp"
#include "Condition.hpp"

namespace tinyWeb {

class CountDownLatch : public noncopyable {
 public:
  explicit CountDownLatch(int count);
  void wait();
  void countDown();
  int getCount() const;

 private:
  mutable MutexLock mutex_;
  Condition cond_;
  int count_;
};

}  // namespace tinyWeb
