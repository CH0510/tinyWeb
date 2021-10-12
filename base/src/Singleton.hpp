#pragma once

#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

#include "noncopyable.hpp"

namespace tinyWeb {

template <typename T>
class Singleton : public noncopyable {
 public:
  static T* instance();

 private:
  Singleton() = delete;
  ~Singleton() = delete;


  static void init() {
    value_ = new T();
    ::atexit(&destroy);
  }

  static void destroy() {
    delete value_;
    value_ = nullptr;
  }

  static T* value_;
  static pthread_once_t once_;
};

template <typename T>
T* Singleton<T>::value_ = nullptr;

template <typename T>
pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::instance() {
  pthread_once(&Singleton::once_, &Singleton::init);
  assert(value_ != nullptr);
  return value_;
}

}  // namespace tinyWeb
