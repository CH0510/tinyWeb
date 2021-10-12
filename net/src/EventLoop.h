#pragma once

#include <unistd.h>
#include <assert.h>

#include <memory>
#include <algorithm>
#include <vector>

#include "../../base/src/noncopyable.hpp"
#include "../../base/src/CurrentThread.h"
#include "../../base/src/Timestamp.h"


namespace tinyWeb {
namespace net {

class Channel;
class Poller;

class EventLoop : public noncopyable {
 public:
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
  void quit() {
    quit_ = true;
  }

  void handleRead();
  void wakeup();

  static EventLoop* getEventLoopOfCurrentThread();

 private:
  typedef std::vector<Channel*> ChannelList;

  void abortNotInLoopThread() const;
  void printActiveChannels() const;

  bool looping_;  // atomic
  bool quit_;
  const pid_t threadId_;  // 当前线程所属的线程id
  std::unique_ptr<Poller> poller_;
  Timestamp pollReturnTime_;
  bool eventHandling_;
  Channel* currentChannel_;
  ChannelList activeChannels_;
  const int wakeupFd_;
  std::unique_ptr<Channel> wakeupFdChannel_;
};

}   // namespace net
}   // namespace tinyWeb
