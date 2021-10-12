#pragma once

#include <pthread.h>

#include <utility>

#include "noncopyable.hpp"

namespace tinyWeb {

template <typename T>
class ThreadLocal : public noncopyable {
 public:
  ThreadLocal() {
    pthread_key_create(&key_, &ThreadLocal::destroy);
  }

  ~ThreadLocal() {
    pthread_key_delete(key_);
  }

  template <typename... Args>
  T* value(Args&&... args) {
    T* obj = static_cast<T*>(pthread_getspecific(key_));
    if (obj == nullptr) {
        T* newObj = new T(std::forward<Args>(args)...);
        pthread_setspecific(key_, newObj);
        obj = newObj;
    }

    return obj;
  }

 private:
  static void destroy(void * x) {
    T* obj = static_cast<T*>(x);
    // 检测是否为不完整的类型
    typedef char T_must_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_complete_type dummy; (void)dummy;
    delete obj;
  }

  pthread_key_t key_;
};

}  // namespace tinyWeb
