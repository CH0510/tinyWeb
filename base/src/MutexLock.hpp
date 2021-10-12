// @Author Hao Chen
// @Email  chenhao980510@163.com
#pragma once

#include <pthread.h>
#include <stdio.h>
#include <assert.h>


#include "noncopyable.hpp"
#include "CurrentThread.h"
#include "Types.h"

namespace tinyWeb {

class MutexLockGuard;

class MutexLock : public noncopyable {
 public:
  MutexLock() : holder_(0) {
    MCHECK(pthread_mutex_init(&mutex_, NULL));
  }

  ~MutexLock() {
    assert(holder_ == 0);
    MCHECK(pthread_mutex_destroy(&mutex_));
  }

  pthread_mutex_t* get() {
    return &mutex_;
  }

  bool isLockedByThisThread() const {
    return holder_ == CurrentThread::tid();
  }

  void assertLocked() const {
    assert(isLockedByThisThread());
  }

 private:
  friend class MutexLockGuard;
  friend class Condition;

  class UnassignGuard : public noncopyable {
   public:
    explicit UnassignGuard(MutexLock& mutex) : mutex_(mutex) {
      mutex_.unassignHolder();
    }

    ~UnassignGuard() {
      mutex_.assignHolder();
    }
   private:
    MutexLock& mutex_;
  };

  void unassignHolder() {
    holder_ = 0;
  }

  void assignHolder() {
    holder_ = CurrentThread::tid();
  }

  void lock() {
    MCHECK(pthread_mutex_lock(&mutex_));
    assignHolder();
  }

  void unlock() {
    unassignHolder();
    MCHECK(pthread_mutex_unlock(&mutex_));
  }

  pthread_mutex_t mutex_;
  pid_t holder_;
};


class MutexLockGuard : public noncopyable {
 public:
  explicit MutexLockGuard(MutexLock& mutex_lock) : mutex_lock_(mutex_lock) {
    mutex_lock_.lock();
  }

  ~MutexLockGuard() {
    mutex_lock_.unlock();
  }

 private:
  MutexLock& mutex_lock_;
};
}  // namespace tinyWeb

// 临时变量会导致无效的加锁
#define MutexLockGuard(x) "Mising guard object name"
