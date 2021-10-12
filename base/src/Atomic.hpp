// @Author Hao Chen
// @Email chenhao980510@163.com
#pragma once

#include <stdint.h>

#include "noncopyable.hpp"

namespace tinyWeb {

template <typename T>
class AtomicIntegerT : public noncopyable {
 public:
  AtomicIntegerT() : value_(0) { }

  T get() {
    return __atomic_load_n(&value_, __ATOMIC_SEQ_CST);
  }

  T getAndAdd(T x) {
    return __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST);
  }

  T addAndGet(T x) {
    return __atomic_add_fetch(&value_, x, __ATOMIC_SEQ_CST);
  }

  T incrementAndGet() {
    return addAndGet(1);
  }

  T decrementAndGet() {
    return addAndGet(-1);
  }

  void add(T x) {
    addAndGet(x);
  }

  void increment() {
    incrementAndGet();
  }

  void decrement() {
    addAndGet(-1);
  }

  T getAndSet(T newVal) {
    return __atomic_exchange_n(&value_, newVal, __ATOMIC_SEQ_CST);
  }

 private:
  volatile T value_;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;
}  // namespace tinyWeb
