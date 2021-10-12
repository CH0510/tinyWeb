#include "PollPoller.h"

#include <utility>

#include "../../base/src/Logging.h"

namespace tinyWeb {
namespace net {

PollPoller::PollPoller(EventLoop* loop) \
  : Poller(loop) {
}

PollPoller::~PollPoller() {
}

Timestamp PollPoller::poll(int timeoutMs, \
                ChannelList* activeChannels) {
  LOG_DEBUG << "fd total = " << channels_.size();
  int numEvents = ::poll(pollFds_.data(), pollFds_.size(), timeoutMs);
  int saveErrno = errno;
  Timestamp now = Timestamp::now();
  
  if (numEvents > 0) {
    LOG_DEBUG << "wake up , numEvents = " \
              << numEvents;
    fillActiveChannels(numEvents, activeChannels);
  } else if (numEvents == 0) {
    LOG_DEBUG << "nothing happened";
  } else {
    if (saveErrno != EINTR) {
      errno = saveErrno;
      LOG_SYSERROR << "PollPoller::poll()";
    }
  }

  return now;
}

void PollPoller::updateChannel(Channel* channel) {
  assertInLoopThread();
  int fd = channel->fd();
  LOG_DEBUG << "fd = " \
            << fd << " event = " << channel->events();
  
  if (channel->index() < 0) {
    // 一个新的文件描述符
    assert(channels_.find(fd) == channels_.end());
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = channel->events();
    pfd.revents = 0;
    pollFds_.push_back(pfd);
    channel->setIndex(pollFds_.size() - 1);
    channels_[fd] = channel;
  } else {
    // 已经存在的文件描述符，更新其关注的事件
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    int index = channel->index();
    assert(0 <= index && index < static_cast<int>(pollFds_.size()));
    struct pollfd& pfd = pollFds_[index];
    assert(pfd.fd == fd || 
           pfd.fd == -fd - 1);
    pfd.fd = fd;
    pfd.events = channel->events();
    pfd.revents = 0;
    if (channel->isNoneEvent()) {
      // 忽略这个文件描述符
      pfd.fd = -channel->fd() - 1;
    }
  }
}

void PollPoller::removeChannel(Channel* channel) {
  assertInLoopThread();
  int fd = channel->fd();
  LOG_DEBUG << "PollPoller::removeChannel() fd = " << fd;
  assert(channels_.find(fd) != channels_.end());
  assert(channels_[fd] == channel);
  assert(channel->isNoneEvent());
  int index = channel->index(); (void)index;
  assert(0 <= index && index < static_cast<int>(channels_.size()));
  const struct pollfd& pollfd = pollFds_[index];
  assert(pollfd.fd == -fd - 1 && \
         pollfd.events == channel->events());
  size_t n = channels_.erase(fd);
  assert(n == 1); (void)n;
  if (index == static_cast<int>(pollFds_.size() - 1)) {
    pollFds_.pop_back();
  } else {
    int channelAtEnd = pollFds_.back().fd;
    std::iter_swap(pollFds_.begin() + index, \
                   pollFds_.end() - 1);
    if (channelAtEnd < 0) {
      channelAtEnd = -channelAtEnd - 1;
    }
    channels_[channelAtEnd]->setIndex(index);
    pollFds_.pop_back();
  }
}

void PollPoller::fillActiveChannels(int numEvents, \
                ChannelList* activeChannels) const {
  for (PollFdList::const_iterator pfd = pollFds_.begin(); \
       numEvents > 0 && pfd != pollFds_.end(); ++pfd) {
    if (pfd->revents > 0) {
      --numEvents;
      ChannelMap::const_iterator it = channels_.find(pfd->fd);
      assert(it != channels_.end());
      Channel* channel = it->second;
      assert(channel->fd() == pfd->fd);
      channel->setRevent(pfd->revents);
      activeChannels->push_back(channel);
    }
  }
}

}  // namespace net
}  // namespace tinyWeb