#pragma once

#include <unordered_map>
#include <vector>

#include "../../base/src/noncopyable.hpp"
#include "EventLoop.h"
#include "../../base/src/Timestamp.h"

namespace tinyWeb {
namespace net {

class Channel;

class Poller : public noncopyable {
 public:
  typedef std::vector<Channel*> ChannelList;

  Poller(EventLoop*);
  virtual ~Poller() { }

  void assertInLoopThread() const {
    loop_->assertInLoopThread();
  }

  bool hasChannel(Channel* channel);

  virtual void updateChannel(Channel* channel) = 0;
  virtual void removeChannel(Channel* channel) = 0;
  virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

  static Poller* newDefaultPoller(EventLoop* loop);

 protected:
  typedef std::unordered_map<int, Channel*> ChannelMap;
  ChannelMap channels_;

 private:
  EventLoop* loop_;
};

}  // namespace net
}  // namespace tinyWeb
