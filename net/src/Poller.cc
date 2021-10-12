#include "Poller.h"

#include "Channel.h"

namespace tinyWeb {
namespace net {

Poller::Poller(EventLoop* loop) \
  : loop_(loop) {
}

bool Poller::hasChannel(Channel* channel) {
  assertInLoopThread();
  ChannelMap::const_iterator it = channels_.find(channel->fd());
  return it != channels_.end() && it->second == channel;
}
}  // namespace net
}  // namespace tinyWeb
