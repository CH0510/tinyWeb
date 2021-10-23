#pragma once

#include <unistd.h>
#include <assert.h>

#include <memory>
#include <algorithm>
#include <vector>
#include <functional>

#include "../../base/src/noncopyable.hpp"
#include "../../base/src/CurrentThread.h"
#include "../../base/src/Timestamp.h"
#include "../../base/src/MutexLock.hpp"
#include "../../base/src/Types.h"


namespace tinyWeb {
namespace net {

class Channel;
class Poller;
class TimerQueue;
class TimerId;

class EventLoop : public noncopyable {
 public:
  typedef std::function<void()> PendingFunctor;

  EventLoop();
  ~EventLoop();

  bool isInLoopThread() const {
    return threadId_ == CurrentThread::tid();
  }

  void assertInLoopThread() const {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }

  void updateChannel(Channel* channel);

  void removeChannel(Channel* channel);

  bool hasChannel(Channel* channel) const;

  void loop();

  //
  void quit() {
    quit_ = true;
    // 如果由其他线程调用，则唤醒目标线程
    if (!isInLoopThread()) {
      wakeup();
    }
  }

  // 唤醒阻塞线程，使任务尽快执行
  void handleRead();
  void wakeup();

  void runInLoop(const PendingFunctor& cb);
  void queueInLoop(const PendingFunctor& cb);

  TimerId runAt(const TimerCallback& cb, Timestamp timePoint);
  TimerId runAfter(const TimerCallback& cb, double duration);
  TimerId runEvery(const TimerCallback& cb, double duration, double interval);
  void cancel(TimerId timer);

  static EventLoop* getEventLoopOfCurrentThread();

 private:
  typedef std::vector<Channel*> ChannelList;

  void abortNotInLoopThread() const;
  void printActiveChannels() const;
  void doPendingFunctors();

  bool looping_;  // atomic
  bool quit_;
  const pid_t threadId_;  // 当前线程所属的线程id（tid）
  std::unique_ptr<Poller> poller_;
  Timestamp pollReturnTime_;  // 保存Poller::poll()调用返回的时间
  // 用于处理活跃的事件
  bool eventHandling_;
  Channel* currentChannel_;
  ChannelList activeChannels_;
  // 定时任务
  std::unique_ptr<TimerQueue> timerQueue_;
  // 用于唤醒阻塞的I/O线程使得任务立即得到执行
  const int wakeupFd_;
  std::unique_ptr<Channel> wakeupFdChannel_;
  // 用于将一些用户回调转移到I/O线程执行
  MutexLock mutex_;
  bool callingPendingFunc_;
  std::vector<PendingFunctor> pendingFunctors_;  // Guard by mutex_
};

}   // namespace net
}   // namespace tinyWeb
