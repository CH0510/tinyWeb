#pragma once

#include <string>

#include "../../base/src/noncopyable.hpp"
#include "../../base/src/Thread.h"
#include "../../base/src/CountDownLatch.h"
#include "Callbacks.h"

namespace tinyWeb {
namespace net {
class EventLoop;

// 创建一个IO子线程,即one loop per thread
class EventLoopThread : public noncopyable {
 public:
  EventLoopThread(const ThreadInitCallback& callback = ThreadInitCallback(), \
                  const std::string& name = "");
  ~EventLoopThread();
  // 创建子线程并返回在子线程中运行的EventLoop对象
  // 例：
  //   EventLoopThread loopThread;
  //   EventLoop* loop = loopThread.startInLoop();
  //   loop->runAfter(cb, 2);
  //   loop->runInLoop(cb);
  //   ...
  //   loop->quit()
  EventLoop* startInLoop();
  bool started() const {
    return loopThread_.started();
  }

  bool joined() const {
    return loopThread_.joined();
  }

  pid_t tid() const {
    return loopThread_.tid();
  }

  std::string name() const {
    return loopThread_.name();
  }

  void setThreadInitCallback(const ThreadInitCallback& callback) {
    callback_ = callback;
  };

 private:
  void threadFunc();

  Thread loopThread_;
  CountDownLatch latch_;
  EventLoop* loop_;
  ThreadInitCallback callback_;
};
}  // namespace net
}  // namespace tinyWeb