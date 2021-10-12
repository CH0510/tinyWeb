#pragma once

#include <poll.h>

#include <vector>

#include "Poller.h"
#include "../../base/src/Timestamp.h"
#include "Channel.h"

namespace tinyWeb {
namespace net {

class PollPoller : public Poller {
 public:
  PollPoller(EventLoop* loop);
  ~PollPoller();

  virtual Timestamp poll(int timeoutMs, \
                   ChannelList* activeChannels) override;
  virtual void updateChannel(Channel* channel) override;
  virtual void removeChannel(Channel* channel) override;

 private:
  typedef std::vector<struct pollfd> PollFdList;

  void fillActiveChannels(int numEvents, \
                    ChannelList* activeChannels) const;

  PollFdList pollFds_;
};

}  // namespace net
}  // namespace tinyWeb