#include "Channel.h"

#include <poll.h>
#include <sys/epoll.h>

#include <sstream>

#include "../../base/src/Logging.h"

namespace tinyWeb {
namespace net {

static_assert(POLLIN == EPOLLIN && \
              POLLOUT == EPOLLOUT && \
              POLLPRI == EPOLLPRI);

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) \
  : fd_(fd), loop_(loop), event_(0), revent_(0), \
    index_(-1), addToLoop_(false), eventHandling_(false) {
}

Channel::~Channel() {
  assert(addToLoop_ == false);
  assert(eventHandling_ == false);
  if (loop_->isInLoopThread()) {
    assert(!loop_->hasChannel(this));
  }
}

void Channel::handleEvent(Timestamp receiveTime) {
  assert(addToLoop_);
  assert(!eventHandling_);
  eventHandling_ = true;
  LOG_DEBUG << "fd = " << fd_ << " revent: "\
            << reventToString();
  if ((revent_ & POLLHUP) && !(revent_ & POLLIN)) {
    LOG_WARN << "fd = " << fd_ \
             << " Channel::handleEvent() POLLHUP";
    if (closeCallback_) closeCallback_();
  }
  
  if (revent_ & POLLNVAL) {
    LOG_WARN << "fd = " << fd_ \
             << " Channel::handleEvent() POLLNVAL";
  }

  if (revent_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_();
  }
  if (revent_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revent_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }

  eventHandling_ = false;
}

std::string Channel::eventToString() const {
  return eventToString(fd_, event_);
}

std::string Channel::reventToString() const {
  return eventToString(fd_, revent_);
}

std::string Channel::eventToString(int fd, int event_) {
  std::ostringstream oss;
  if (event_ & POLLIN) {
    oss << "POLLIN ";
  }
  if (event_ & POLLPRI) {
    oss << "POLLPRI ";
  }
  if (event_ & POLLOUT) {
    oss << "POLLOUT ";
  }
  if (event_ & POLLRDHUP) {
    oss << "POLLRDHUP ";
  }
  if (event_ & POLLERR) {
    oss << "POLLERR ";
  }
  if (event_ & POLLHUP) {
    oss << "POLLHUP ";
  }
  if (event_ & POLLNVAL) {
    oss << "POLLNVAL ";
  }

  return oss.str();
}

}  // namespace net
}  // namespace tinyWeb
