#include "Poller.h"
#include "PollPoller.h"
#include "EPollPoller.h"
#include "EventLoop.h"

namespace tinyWeb {
namespace net {
Poller* Poller::newDefaultPoller(EventLoop* loop) {
  if (::getenv("TINYWEB_USE_POLL")) {
    return new PollPoller(loop);
  } else {
    return new EPollPoller(loop);
  }
}
}
}  // namespace tinyWeb