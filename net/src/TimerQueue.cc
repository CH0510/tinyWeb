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

  // 设置timerfd的timerfd_settime的调用参数
  // 参数：
  //   expireTime:超时时间
  //   itimer:传出参数，timerfd_settime的调用参数
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
  MCHECK(::close(timerFd_));
  // 由于是手动管理Timer实例，所以需要手动删除剩余的定时器
  for (const Entry& entry : timerQueue_) {
    delete entry.second;
  }
}

TimerId TimerQueue::addTimer(TimerCallback cb, \
        Timestamp timePoint, double interval) {
  Timer* timer = new Timer(this, cb, timePoint, interval);
  // 将相关工作转移到IO线程中执行，已达到线程安全的要求
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
  // 将相关工作转移到IO线程中执行，已达到线程安全
  ownerLoop_->runInLoop( \
    std::bind(&TimerQueue::cancelInLoop, this, timer));
  
  LOG_DEBUG << "TimerQueue::delTimer() sequence: " \
          << timer.sequence();
}

// 已到期的定时器一定会执行
// 即当前待删除的定时器可能在过期队列中，此时将其加入
// 待删除定时器队列即可，reset时在该队列里的定时器就不会再重启
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
  auto ret = activeTimer_.insert(std::make_pair(timer->sequence(), timer));
  assert(ret.second); (void) ret;
  auto it = timerQueue_.insert( \
        std::make_pair(timer->expireTime(), timer));
  assert(it.second);
  assert(activeTimer_.size() == timerQueue_.size());
  // 判断其是否插入了更早超时的定时
  return it.first == timerQueue_.begin();
}

bool TimerQueue::delTimer(Timer* timer) {
  // 判断其是否删除了最早超时的定时器
  bool ret = timerQueue_.begin()->second == timer;
  size_t n = activeTimer_.erase(timer->sequence());
  assert(n == 1); (void)n;
  n = timerQueue_.erase(std::make_pair(timer->expireTime(), timer));
  assert(n == 1); (void)n;
  delete timer;
  assert(activeTimer_.size() == timerQueue_.size());
  return ret;
}

// 调用timerfd_settime的第二参数为0，即相对时间；
// 不能为ABSTIME，其表示绝对时间。因为设置的itimer参数是相对当前的时间。
// 如果定时队列为空就会取消timerfd的定时
void TimerQueue::adjustTimer() {
  struct itimerspec itimer;
  memset(&itimer, 0, sizeof(itimer));
  if (timerQueue_.empty() == false) {
    auto it = timerQueue_.begin();
    setExpireTime(it->first, &itimer);
  }

  MCHECK(timerfd_settime(timerFd_,  0, \
                    &itimer, NULL));
}

// 该函数回将所有的过期定时器移除定时队列，
// 其会将所有的超时定时从timerQueue_和activeTimer_中移除
// 之后可调用reset负责重新插入周期性的定时器
std::vector<Timer*> TimerQueue::getAllExpired(Timestamp now) {
  Entry last = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  auto end = timerQueue_.lower_bound(last);
  std::vector<Timer*> expireTimerVec;
  for (auto it = timerQueue_.begin(); it != end;) {
    expireTimerVec.push_back(it->second);
    size_t n = activeTimer_.erase(it->second->sequence());
    assert(n == 1); (void)n;
    it = timerQueue_.erase(it);
  }
  
  assert(timerQueue_.size() == activeTimer_.size());
  return expireTimerVec;
}

// 将所有超时定时器中的周期定时器重新启动
// 由于调用cancel而取消的定时器将不会在重新的启动
// 
bool TimerQueue::reset(std::vector<Timer*>& expireTimerVec, Timestamp now) {
  bool earliestChanged = false;
  for (Timer* timer : expireTimerVec) {
    if (timer->repeated() && \
        canceledSet_.find(timer) == canceledSet_.end()) {
      timer->restart(now);
      earliestChanged |= insert(timer);
    } else {
      delete timer;
    }
  }

  return earliestChanged;
}

void TimerQueue::handleRead() {
  ownerLoop_->assertInLoopThread();
  Timestamp now = Timestamp::now();
  readTimerFd(timerFd_);

  callingTimerCallback_ = true;
  std::vector<Timer*> expireTimerVec = getAllExpired(now);
  for (Timer* timer : expireTimerVec) {
    timer->run();
  }
  callingTimerCallback_ = false;

  // 这里直接使用handleRead调用时的时间
  // 而不是再获取一个Timestamp::now()，
  // 作为复位的时间，以提高定时的精确度
  reset(expireTimerVec, now);
  // 超时后总是删除前面的已超时定时器
  // 所以总是进行调整
  adjustTimer();
  canceledSet_.clear();
}

}  // namespace net
}  // namespace tinyWeb