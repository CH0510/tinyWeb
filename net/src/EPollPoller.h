#pragma once

#include <sys/epoll.h>

#include <vector>

#include "Poller.h"
#include "EventLoop.h"
#include "../../base/src/Timestamp.h"

namespace tinyWeb {
namespace net {

class EPollPoller : public Poller {
 public:
  EPollPoller(EventLoop *loop);
  ~EPollPoller();
  
  virtual Timestamp poll(int timeoutMs, \
                         ChannelList* activeChannels) override;
  virtual void updateChannel(Channel* channel) override;
  virtual void removeChannel(Channel* channel) override;
 private:
  typedef std::vector<struct epoll_event> EventList;

  void fillActiveChannels(int numEvent, \
                          ChannelList* activeChannels) const;
  void update(int operation, Channel* channel);
  static const char* operationToString(int op);

  int epollfd_;
  EventList eventList_;
};
}  // namespace net
}  // namespace tinyWeb