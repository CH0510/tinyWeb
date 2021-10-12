// @Author Hao Chen
// @Email  chenhao980510@163.com
#include "CountDownLatch.h"

namespace tinyWeb {

CountDownLatch::CountDownLatch(int count) \
  : mutex_(), cond_(mutex_), count_(count) {
}

void CountDownLatch::wait() {
  MutexLockGuard lock(mutex_);
  while (count_ > 0) {
    cond_.wait();
  }
}

int CountDownLatch::getCount() const {
  MutexLockGuard lock(mutex_);
  return count_;
}

void CountDownLatch::countDown() {
  MutexLockGuard lock(mutex_);
  --count_;
  if (count_ == 0) {
    cond_.notifyAll();
  }
}

}  // namespace tinyWeb
