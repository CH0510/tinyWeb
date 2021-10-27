#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../../base/src/noncopyable.hpp"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include "../../base/src/StringPiece.hpp"
#include "Callbacks.h"

namespace tinyWeb {
namespace net {

// IO Thread pool, 非线程安全的，仅能在 baseLoop_ 线程中调用
// 默认 IO Thread 个数为0， 需在调用 start() 前调用 setThreadNum()
class EventLoopThreadPool : public noncopyable {
 public:
  EventLoopThreadPool(EventLoop* baseLoop, const StringArg& name);
  ~EventLoopThreadPool() = default;

  // 设置 IO 线程池中的线程数目，须在调用
  // start() 之前调用
  void setThreadNum(size_t numThreads) {
    assert(started_ == false);
    numThreads_ = numThreads;
  }

  bool started() const { return started_; }

  // 启动 IO Thread 线程池
  // numThreads_(0) : 没有专门的 IO 线程，监听套接字与通信套接字共享一个 EventLoop
  // numThreads_(1) : 监听套接字在 baseLoop_ 中，通信套接字在另一个 EventLoop 中
  // numThreads_(N) : 监听套接字在 baseLoop_ 中，通信套接字均匀的分布在 N 个 IO EventLoopThread 中
  void start(const ThreadInitCallback& cb = ThreadInitCallback());

  // 选取处理 TCP 连接的 IO Thread
  // 目前实现的是 Round-Robin 算法
  EventLoop* getNextLoop();

  std::vector<EventLoop*> getAllLoops() const;

 private:
  EventLoop* baseLoop_;
  std::string name_;
  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop*> loops_;
  size_t numThreads_;
  bool started_;
  int next_;
};

}  // namespace net
}  // namespace tinyWeb