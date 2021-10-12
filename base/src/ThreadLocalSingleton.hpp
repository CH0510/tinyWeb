#pragma once

#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

#include "noncopyable.hpp"

namespace tinyWeb {

template <typename T>
class ThreadLocalSingleton : public noncopyable {
 public:
  ThreadLocalSingleton()  = delete;

  ~ThreadLocalSingleton() = delete;

  static T* instance() {
    if (value_ == nullptr) {
        value_ = new T;
        deleter_.setValue(static_cast<void*>(value_));
    }

    return value_;
  }

 private:
  // 设置该内部类的目的是为了在线程退出的时候安全的删除动态分配的内存，不导致内存泄露
  class Deleter {
   public:
    Deleter() {
        pthread_key_create(&key_, &ThreadLocalSingleton::destroy);
    }

    ~Deleter() {
        pthread_key_delete(key_);
    }

    void setValue(void *obj) {
        // 不能多次设置，会导致内存泄漏
        assert(pthread_getspecific(key_) == nullptr);
        pthread_setspecific(key_, obj);
    }

   private:
    pthread_key_t key_;
  };

  static void destroy(void* value) {
    assert(value == value_);
    typedef char T_must_be_completed_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_completed_type dummy; (void) dummy;
    delete value_;
    value_ = nullptr;
  }

  static __thread T* value_;
  static Deleter deleter_;
};

template <typename T>
__thread T* ThreadLocalSingleton<T>::value_ = nullptr;

template <typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

}  // namespace tinyWeb
