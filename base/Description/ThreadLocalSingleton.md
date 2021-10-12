# ThreadLocalSingleton设计细节
- 首先要理解它主要是一个Singeton，所以构造析构设置为delete
- 因为每个线程都会有一个，所以不需要pthread_once(),只需将对象指针设置为__thread属性即可，但是需要pthread_key_t,由于没有构造和析构，那pthread_key_create()和pthread_key_delete()怎么放置?
  - pthread_key_create()比较好处理，可以在对象指针为空时调用并紧接着调用pthread_setspecific()
  - pthread_key_delete()几乎无处安放
  - 综上，我们需要一个辅助类，完成对pthread_key_*的调用

初始版本：
#include "noncopyable.hpp"
#include <pthread.h>
#include <cstdlib>

namespace tinyWeb {

template <typename T>
class ThreadLocalSingleton : public noncopyable {
public:
    ThreadLocalSingleton()  = delete;

    ~ThreadLocalSingleton() = delete;

    static T* instance() {
        if (value_ == nullptr) {
            value_ = new T;
            ::atexit(&ThreadLocalSingleton::destroy);
        }

        return value_;
    }

private:
    // 没有测试是否是完整的类
    static void destroy() {
        delete value_;
    }
    static __thread T* value_;
};

template <typename T>
__thread T* ThreadLocalSingleton<T>::value_ = nullptr;

}

这样做法的缺陷是，没办法安全的释放所有动态分配的内存（使用::atexit()会导致free同一指针多次，这跟编译器不知道多线程有关？）
所有为了在线程退出时安全的删除分配的数据，可以单独构造一个删除器类，并使用pthread_ket_t来完成每个线程退出时安全的删除分配的数据。