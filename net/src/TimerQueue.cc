#include "TimerQueue.h"

#include <sys/timerfd.h>
#include <sys/time.h>
#include <assert.h>

#include "EventLoop.h"
#include "Channel.h"
#include "../../base/src/Logging.h"
#include "TimerId.h"
#include "Timer.h"

namespace tinyWeb {
namespace net {
namespace {
  static const int64_t kNanoSecondsPerMicroSecond = 1000;

  int createTimerFd() {
    int timerFd = ::timerfd_create(CLOCK_MONOTONIC,\
                            TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerFd < 0) {
      LOG_FATAL << "createTimerFd() failed";
    }

    return timerFd;
  }

  void readTimerFd(int timerFd) {
    uint64_t v;
    int n = ::read(timerFd, &v, sizeof v);
    if (n != sizeof(uint64_t)) {
      LOG_ERROR << "readTimerFd() from timerfd of " \
                << n << " bytes instead 8 bytes";
    }
  }

  void setExpireTime(Timestamp expireTime, struct itimerspec* itimer) {
    assert(expireTime.valid());
    int64_t microSeconds = expireTime.microSecondsSinceEpoch() - \
                                     Timestamp::now().microSecondsSinceEpoch();
    itimer->it_value.tv_sec = static_cast<time_t>( \
                              microSeconds / Timestamp::kMcroSecondsPerSecond);
    itimer->it_value.tv_nsec = static_cast<long>( \
      microSeconds % Timestamp::kMcroSecondsPerSecond) * kNanoSecondsPerMicroSecond;
  }
}


TimerQueue::TimerQueue(EventLoop* loop) \
  : ownerLoop_(loop), timerFd_(createTimerFd()), \
    timerChannel_(new Channel(loop, timerFd_)), \
    callingTimerCallback_(false) {
  timerChannel_->setReadCallback(\
      std::bind(&TimerQueue::handleRead, this));
  timerChannel_->enableReading();
}

TimerQueue::~TimerQueue() {
  timerChannel_->disableAll();
  timerChannel_->remove();
  ::close(timerFd_);
  for (const Entry& entry : timerQueue_) {
    delete entry.second;
  }
}

TimerId TimerQueue::addTimer(TimerCallback cb, \
        Timestamp timePoint, double interval) {
  Timer* timer = new Timer(this, cb, timePoint, interval);
  ownerLoop_->runInLoop( \
    std::bind(&TimerQueue::addTimerInLoop, this, timer));

  LOG_DEBUG << "TimerQueue::addTimer() sequence: " \
          << timer->sequence();
  
  return TimerId(timer->sequence(), timer);
}

void TimerQueue::addTimerInLoop(Timer* timer) {
  ownerLoop_->assertInLoopThread();
  assert(activeTimer_.find(timer->sequence()) == \
         activeTimer_.end());
  bool earliestChanged = insert(timer);
  if (earliestChanged) {
    adjustTimer();
  }
}

void TimerQueue::cancel(TimerId timer) {
  ownerLoop_->runInLoop( \
    std::bind(&TimerQueue::cancelInLoop, this, timer));
  
  LOG_DEBUG << "TimerQueue::delTimer() sequence: " \
          << timer.sequence();
}

// 已到期的定时器一定会执行
// 即当前待删除的定时器可能在过期队列中，此时将其加入
// 待删除定时器队列即可，reset时在该队列的定时器就不会再重启
void TimerQueue::cancelInLoop(TimerId timerId) {
  ownerLoop_->assertInLoopThread();
  auto it = activeTimer_.find(timerId.sequence());
  // 当前定时器已超时
  if (it == activeTimer_.end()) {
    canceledSet_.insert(timerId.timer_);
    return;
  }

  bool earliestChanged = delTimer(it->second);
  if (earliestChanged) {
    adjustTimer();
  }
}

bool TimerQueue::insert(Timer* timer) {
  activeTimer_[timer->sequence()] = timer;
  auto it = timerQueue_.insert( \
        std::make_pair(timer->expireTime(), timer));
  assert(it.second);
  assert(activeTimer_.size() == timerQueue_.size());
  return it.first == timerQueue_.begin();
}

bool TimerQueue::delTimer(Timer* timer) {
  bool ret = timerQueue_.begin()->second == timer;
  activeTimer_.erase(timer->sequence());
  timerQueue_.erase(std::make_pair(timer->expireTime(), timer));
  delete timer;
  assert(activeTimer_.size() == timerQueue_.size());
  return ret;
}

void TimerQueue::adjustTimer() {
  struct itimerspec itimer;
  memset(&itimer, 0, sizeof(itimer));
  if (timerQueue_.empty() == false) {
    auto it = timerQueue_.begin();
    setExpireTime(it->first, &itimer);
  }

  // 注意这里的第二参数不能为ABSTIME,基础不熟害死人
  // 应填0，即相对时间
  MCHECK(timerfd_settime(timerFd_,  0, \
                    &itimer, NULL));
}

// 该函数回将所有的过期定时器移除定时队列
std::vector<Timer*> TimerQueue::getAllExpired(Timestamp now) {
  Entry last = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  auto end = timerQueue_.lower_bound(last);
  std::vector<Timer*> expireTimerVec;
  for (auto it = timerQueue_.begin(); it != end;) {
    expireTimerVec.push_back(it->second);
    activeTimer_.erase(it->second->sequence());
    it = timerQueue_.erase(it);
  }
  
  assert(timerQueue_.size() == activeTimer_.size());
  return expireTimerVec;
}

void TimerQueue::reset(std::vector<Timer*>& expireTimerVec, Timestamp now) {
  for (Timer* timer : expireTimerVec) {
    if (timer->repeated() && \
        canceledSet_.find(timer) == canceledSet_.end()) {
      timer->restart(now);
      insert(timer);
    } else {
      delete timer;
    }
  }

  adjustTimer();
}

void TimerQueue::handleRead() {
  ownerLoop_->assertInLoopThread();
  readTimerFd(timerFd_);

  callingTimerCallback_ = true;
  std::vector<Timer*> expireTimerVec = getAllExpired(Timestamp::now());
  for (Timer* timer : expireTimerVec) {
    timer->run();
  }
  callingTimerCallback_ = false;

  reset(expireTimerVec, Timestamp::now());
  canceledSet_.clear();
}

}  // namespace net
}  // namespace tinyWeb