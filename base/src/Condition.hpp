// @Author Hao Chen
// @Email chenhao980510@163.com
#pragma once

#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include "MutexLock.hpp"

namespace tinyWeb {

class Condition : noncopyable {
 public:
  explicit Condition(MutexLock& mutex) : mutex_(mutex) {
    ConditionAttr attr;
    attr.setMonotonicClock();
    MCHECK(pthread_cond_init(&pcond_, attr.get()));
  }

  ~Condition() {
    MCHECK(pthread_cond_destroy(&pcond_));
  }

  void wait() {
    // 释放锁前消除holder,并在唤醒后重新设置holder
    MutexLock::UnassignGuard ug(mutex_);
    MCHECK(pthread_cond_wait(&pcond_, mutex_.get()));
  }

  bool waitForSecond(double seconds) {
    struct timespec wait_time;
    MCHECK(clock_gettime(CLOCK_MONOTONIC, &wait_time));
    const int64_t kNanoSecondsPerSecond = 1000000000;
    int64_t nonseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond) \
                         + wait_time.tv_nsec;
    wait_time.tv_sec += nonseconds / kNanoSecondsPerSecond;
    wait_time.tv_nsec = nonseconds % kNanoSecondsPerSecond;

    // 释放锁前消除holder,并在唤醒后重新设置holder
    MutexLock::UnassignGuard ug(mutex_);
    return ETIMEDOUT == pthread_cond_timedwait( \
                          &pcond_, mutex_.get(), &wait_time);
  }

  void notify() {
    MCHECK(pthread_cond_signal(&pcond_));
  }

  void notifyAll() {
    MCHECK(pthread_cond_broadcast(&pcond_));
  }

 private:
  // attribute helper class
  class ConditionAttr : noncopyable {
   public:
    ConditionAttr() {
        MCHECK(pthread_condattr_init(&cond_attr_));
    }

    void setMonotonicClock() {
        MCHECK(pthread_condattr_setclock(&cond_attr_, CLOCK_MONOTONIC));
    }

    pthread_condattr_t* get() {
        return &cond_attr_;
    }

    ~ConditionAttr() {
        MCHECK(pthread_condattr_destroy(&cond_attr_));
    }
   private:
    pthread_condattr_t cond_attr_;
  };

  pthread_cond_t pcond_;
  MutexLock& mutex_;
};

}  // namespace tinyWeb
