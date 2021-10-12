#include "EPollPoller.h"


#include "../../base/src/Logging.h"
#include "Channel.h"

namespace tinyWeb {
namespace net {

namespace {
  const int kNew = -1;
  const int kAdded = 1;
  const int kDeleted = 2;
}

static const int kInitListSize = 16;

EPollPoller::EPollPoller(EventLoop* loop) \
  : Poller(loop), \
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)), \
    eventList_(kInitListSize) {
  if (epollfd_ < 0) {
    LOG_SYSFATAL << "EPollPoller::EpollPoller()";
  }
}

EPollPoller::~EPollPoller() {
  ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels) {
  LOG_DEBUG << "fd total count = " << channels_.size();
  int numEvents = ::epoll_wait(epollfd_, eventList_.data(), \
                               static_cast<int>(eventList_.size()), \
                               timeoutMs);
  int saveErrno = errno;
  Timestamp now = Timestamp::now();

  if (numEvents > 0) {
    LOG_DEBUG << numEvents << " events happened";
    fillActiveChannels(numEvents, activeChannels);
    if (static_cast<int>(eventList_.size()) == numEvents) {
      eventList_.resize(2 * eventList_.size());
    }
  } else if (numEvents == 0) {
    LOG_DEBUG << "nothing happened";
  } else {
    if (saveErrno != EINTR) {
      errno = saveErrno;
      LOG_SYSERROR << "EPollPoller::poll()";
    }
  }

  return now;
}

void EPollPoller::updateChannel(Channel* channel) {
  assertInLoopThread();
  const int fd = channel->fd();
  const int index = channel->index();
  LOG_DEBUG << "fd = " << fd << " event = " \
            << channel->eventToString() \
            << " index = " << index;
  if (index == kNew || index == kDeleted) {
    // a new one
    if (index == kNew) {
      assert(channels_.find(fd) == channels_.end());
      channels_[fd] = channel;
    } else {
      assert(channels_.find(fd) != channels_.end());
      assert(channels_[fd] == channel);
    }

    update(EPOLL_CTL_ADD, channel);
    channel->setIndex(kAdded);
  } else {
    // already exists
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL, channel);
      channel->setIndex(kDeleted);
    } else {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}

void EPollPoller::removeChannel(Channel* channel) {
  assertInLoopThread();
  int fd = channel->fd();
  assert(channels_.find(fd) != channels_.end());
  assert(channels_[fd] == channel);
  assert(channel->index() == kDeleted && \
         channel->isNoneEvent());
  size_t n = channels_.erase(fd);
  assert(n == 1); (void)n;

  channel->setIndex(kNew);
}

void EPollPoller::fillActiveChannels(int numEvents, \
                        ChannelList* activeChannels) const {
  for (EventList::const_iterator pfd = eventList_.begin(); \
       numEvents > 0 && pfd != eventList_.end(); ++pfd) {
    Channel* channel = static_cast<Channel*>(pfd->data.ptr);
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_.find(channel->fd())->second == channel);
    channel->setRevent(pfd->events);
    activeChannels->push_back(channel);
    --numEvents;
  }
}

void EPollPoller::update(int operation, Channel* channel) {
  struct epoll_event event;
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  LOG_DEBUG << "epoll_ctl() op = " << operationToString(operation) \
            << " fd = " << fd << " events = " << channel->eventToString();
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    if (operation == EPOLL_CTL_DEL) {
      LOG_SYSERROR << "epoll_ctl() op = " << operationToString(operation) \
                   << " fd = " << fd;
    } else {
      LOG_SYSFATAL << "epoll_ctl() op = " << operationToString(operation) \
                   << " fd = " << fd;
    }
  }
}

const char* EPollPoller::operationToString(int op) {
  switch (op) {
    case EPOLL_CTL_ADD:
      return "ADD";
    case EPOLL_CTL_DEL:
      return "DEL";
    case EPOLL_CTL_MOD:
      return "MOD";
    default:
      return "unknow op";
  }
}

}  // namespace net
}  // namespace tinyWeb