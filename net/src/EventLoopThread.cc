#include "EventLoopThread.h"

#include <unordered_map>

#include "EventLoop.h"
#include "../../base/src/ThreadLocal.hpp"

namespace tinyWeb {
namespace net {

  EventLoopThread::EventLoopThread(const std::string& name) \
    : loopThread_(std::bind(&EventLoopThread::threadFunc, this), name), \
      latch_(1), \
      loop_(NULL) {
  }

  // 这里实现了析构函数，在EventLoopThread对象销毁的时候销毁IO子线程，
  // 但这样绑定了IO子线程和EventLoopThread对象的声明期，
  // 这里不实现也可以，但是如果主线程先退出就会导致IO子线程的非正常退出
  EventLoopThread::~EventLoopThread() {
    if (loop_ != NULL) {
      loop_->quit();
      loopThread_.join();
    }
  }

  EventLoop* EventLoopThread::startInLoop() {
    loopThread_.start();
    latch_.wait();
    assert(loop_);

    return loop_;
  }

  void EventLoopThread::threadFunc() {
    EventLoop loop;
    loop_ = &loop;
    latch_.countDown();
    loop.loop();
    loop_ = NULL;
  }
}  // namespace net
}  // namespace tinyWeb