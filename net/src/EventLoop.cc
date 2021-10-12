#include "EventLoop.h"

#include <assert.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/signal.h>
#include <stdint.h>

#include <functional>

#include "../../base/src/Logging.h"
#include "Poller.h"
#include "Channel.h"

namespace tinyWeb {
namespace net {

namespace {
__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

int createWakeupFd() {
  int fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (fd < 0) {
    LOG_FATAL << "createWakeupFd() failed";
  }

  return fd;
}

class IgnoreSigPipe {
public:
  IgnoreSigPipe() {
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    MCHECK(sigaction(SIGPIPE, &sa, NULL));
  }
};

IgnoreSigPipe g_ignoreSigPipe;
}

EventLoop::EventLoop() \
  : looping_(false), quit_(false), threadId_(CurrentThread::tid()), \
    poller_(Poller::newDefaultPoller(this)), eventHandling_(false), \
    currentChannel_(NULL), wakeupFd_(createWakeupFd()), \
    wakeupFdChannel_(new Channel(this, wakeupFd_)) {
  LOG_DEBUG << "EventLoop create " << this \
            << " in thread " << threadId_;
  if (t_loopInThisThread) {
    LOG_FATAL << "Another EventLoop " << t_loopInThisThread \
              << " exists in thread " << threadId_;
  } else {
    t_loopInThisThread = this;
  }

  wakeupFdChannel_->setReadCallback( \
        std::bind(&EventLoop::handleRead, this));
  wakeupFdChannel_->enableReading();
}

EventLoop::~EventLoop() {
  LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_ \
            << " destructs in thread " << CurrentThread::tid();
  wakeupFdChannel_->disableAll();
  wakeupFdChannel_->remove();
  ::close(wakeupFd_);
  assert(!looping_);
  t_loopInThisThread = NULL;
}

void EventLoop::abortNotInLoopThread() const {
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " \
            << this << " was created in threadId_ = " << threadId_ \
            << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::loop() {
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  LOG_DEBUG << "EventLoop " << this << " start looping";

  while (!quit_) {
    activeChannels_.clear();
    pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
    if (logLevel() <= LogLevel::DEBUG) {
      printActiveChannels();
    }
    eventHandling_ = true;
    for (Channel* channel : activeChannels_) {
      currentChannel_ = channel;
      channel->handleEvent(pollReturnTime_);
    }
    currentChannel_ = NULL;
    eventHandling_ = false;
  }

  LOG_DEBUG << "EventLoop " << this  << " stop looping";
  looping_ = false;
}

inline EventLoop* EventLoop::getEventLoopOfCurrentThread() {
  return t_loopInThisThread;
}

void EventLoop::updateChannel(Channel* channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) const {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  if (eventHandling_) {
    assert(currentChannel_ == channel || \
            std::find(activeChannels_.begin(), \
                      activeChannels_.end(), channel) == \
                      activeChannels_.end());
  }
  return poller_->hasChannel(channel);
}

void EventLoop::printActiveChannels() const {
  for (const Channel* channel : activeChannels_) {
    LOG_DEBUG << "{" << channel->reventToString() << "}";
  }
}

void EventLoop::handleRead() {
  uint64_t a = 1;
  ssize_t n = ::read(wakeupFd_, &a, sizeof a);
  if (n != sizeof a) {
    LOG_SYSERROR << "EventLoop::handleRead() read eventfd " \
                 << n << " byte instead of 8";
  }
}

void EventLoop::wakeup() {
  uint64_t a = 1;
  ssize_t n = ::write(wakeupFd_, &a, sizeof a);
  if (n != sizeof a) {
    LOG_SYSERROR << "EventLoop::wakeup() write eventfd " \
                 << n << " byte instead of 8";
  }
}

}  // namespace net
}  // namespace tinyWeb
